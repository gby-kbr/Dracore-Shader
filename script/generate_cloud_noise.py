"""Generates the tileable cloud noise texture used by Dracore's cumulus clouds.

Channels (256x256 RGBA, tiles in both directions):
  R  coverage : large soft blobs, decides where clouds are
  G  puffs    : inverted Worley noise, round cauliflower-like bumps
  B  detail   : fine noise that roughens the edges
  A  255

Run: python3 script/generate_cloud_noise.py
Needs numpy and Pillow.
"""
import os
import numpy as np
from PIL import Image

SIZE = 256
SEED = 11
OUT = os.path.join(os.path.dirname(__file__), "..", "mcpack", "textures", "environment", "cloud_noise.png")


def spectral(rng, slope):
    """Tileable noise with a 1/f^slope spectrum, normalized to 0..1."""
    white = rng.standard_normal((SIZE, SIZE))
    f = np.fft.fftfreq(SIZE)
    r = np.sqrt(f[:, None] ** 2 + f[None, :] ** 2)
    r[0, 0] = 1.0
    out = np.fft.ifft2(np.fft.fft2(white) / r ** slope).real
    return out


def worley(rng, cells):
    """Tileable Worley noise: distance to the nearest feature point, 0..1."""
    pts = rng.random((cells, cells, 2))
    ys, xs = np.mgrid[0:SIZE, 0:SIZE] / SIZE * cells
    best = np.full((SIZE, SIZE), 9.0)
    cy = np.floor(ys).astype(int)
    cx = np.floor(xs).astype(int)
    for dy in (-1, 0, 1):
        for dx in (-1, 0, 1):
            ny, nx = cy + dy, cx + dx
            p = pts[ny % cells, nx % cells]
            d = np.hypot(ys - (ny + p[..., 0]), xs - (nx + p[..., 1]))
            best = np.minimum(best, d)
    return np.clip(best, 0.0, 1.0)


def equalize(a):
    """Spread values evenly over 0..1 so thresholds behave the same everywhere."""
    flat = a.ravel()
    ranks = np.argsort(np.argsort(flat))
    return (ranks / (flat.size - 1)).reshape(a.shape)


def main():
    rng = np.random.default_rng(SEED)
    coverage = equalize(spectral(rng, 2.0))
    puffs = (1 - worley(rng, 6)) * 0.5 + (1 - worley(rng, 12)) * 0.3 + (1 - worley(rng, 24)) * 0.2
    puffs = equalize(puffs)
    detail = equalize(spectral(rng, 1.0))
    img = np.stack([coverage, puffs, detail, np.ones_like(coverage)], axis=-1)
    Image.fromarray((img * 255 + 0.5).astype(np.uint8), "RGBA").save(OUT)
    print("wrote", os.path.normpath(OUT))


if __name__ == "__main__":
    main()
