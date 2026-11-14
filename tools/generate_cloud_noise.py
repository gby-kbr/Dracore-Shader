"""Generates the noise texture that the clouds read (mcpack/textures/environment/cloud_noise.png).

256x256 RGBA, tiles seamlessly. Every channel is histogram-equalized (uniform 0..1), so a threshold in the
shader is also the fraction of the sky that stays clear. The noise has a power law spectrum, so it is smooth
and soft without hard edges.
  R  large soft shapes: decide where the clouds are.
  G  medium shapes: the body of the streaks.
  B  fine detail: the thin fibers at the edge. The shader stretches it along the wind.

Usage: python3 tools/generate_cloud_noise.py
Needs: numpy, Pillow
"""
import os

import numpy as np
from PIL import Image

N = 256
OUTPUT = os.path.join(os.path.dirname(__file__), "..", "mcpack", "textures", "environment", "cloud_noise.png")


def equalize(a):
    r = np.argsort(np.argsort(a.ravel()))
    return (r / (r.size - 1.0)).reshape(a.shape)


def spectral(rng, beta, kmin, kmax):
    """Tileable noise with a power law spectrum k^-beta, only the frequencies kmin..kmax (cycles per tile)."""
    f = np.fft.fftfreq(N) * N
    fx, fy = np.meshgrid(f, f)
    k = np.sqrt(fx ** 2 + fy ** 2)
    amp = np.where((k >= kmin) & (k <= kmax), np.maximum(k, 1e-3) ** (-beta / 2.0), 0.0)
    spec = amp * (rng.standard_normal((N, N)) + 1j * rng.standard_normal((N, N)))
    return np.real(np.fft.ifft2(spec))


def main():
    rng = np.random.default_rng(20241114)
    channels = [
        equalize(spectral(rng, 3.4, 1, 6)),
        equalize(spectral(rng, 2.8, 2, 20)),
        equalize(spectral(rng, 2.2, 6, 64)),
    ]
    img = np.stack(channels + [np.ones((N, N))], axis=-1)
    os.makedirs(os.path.dirname(OUTPUT), exist_ok=True)
    Image.fromarray((img * 255.0 + 0.5).astype(np.uint8), "RGBA").save(OUTPUT, optimize=True)
    print("written", os.path.normpath(OUTPUT))


if __name__ == "__main__":
    main()
