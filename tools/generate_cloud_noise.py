"""Generates the noise texture that the clouds read (mcpack/textures/environment/cloud_noise.png).

1024x1024 RGB, tiles seamlessly. It is made of hair-like fibers that follow one curved flow field, like cirrus
fibratus: the fibers are not stretched noise, they are drawn along curved lines (line integral convolution), so
they bend, taper and split. All channels follow the same flow, so they can be read at the same coordinates.
  R  fine fibers: single hairs, short.
  G  medium streaks: bundles of fibers, longer.
  B  large swooshes: where the thick clouds are and where the sky stays clear.
Every channel is stretched to use 0..1 evenly, so a threshold in the shader is the fraction of the sky left clear.

Usage: python3 tools/generate_cloud_noise.py
Needs: numpy, scipy, Pillow
"""
import os

import numpy as np
from PIL import Image
from scipy.ndimage import gaussian_filter, map_coordinates

N = 1024
OUTPUT = os.path.join(os.path.dirname(__file__), "..", "mcpack", "textures", "environment", "cloud_noise.png")
STEP = 2.0  # pixels per step along the flow
rng = np.random.default_rng(7)
yy, xx = np.mgrid[0:N, 0:N].astype(np.float32)


def band_noise(lo, hi, slope=1.0):
    """tileable noise with energy only between lo and hi cycles per tile"""
    f = np.fft.fftfreq(N)[:, None]*N
    g = np.fft.fftfreq(N)[None, :]*N
    r = np.sqrt(f*f + g*g) + 1e-6
    amp = (r**-slope)*((r >= lo) & (r <= hi))
    spec = amp*np.exp(2j*np.pi*rng.random((N, N)))
    n = np.real(np.fft.ifft2(spec))
    return ((n - n.mean())/n.std()).astype(np.float32)


def equalize(a, lo=0.0, hi=1.0):
    """rank transform: a uniform 0..1 histogram"""
    order = np.argsort(a.ravel())
    out = np.empty(a.size, np.float32)
    out[order] = np.linspace(lo, hi, a.size, dtype=np.float32)
    return out.reshape(a.shape)


# the flow: a slow sweep that turns about 40 degrees across the tile, plus small bends
theta = -0.25 + 0.26*band_noise(1, 2.2, 0.6) + 0.05*band_noise(3, 6, 0.8)
fx, fy = np.cos(theta), np.sin(theta)


def lic(src, length, jitter=0.5):
    """smears src along the flow, forward and back, with a kernel that tapers to the ends"""
    steps = int(length)
    acc = np.zeros_like(src)
    wsum = 0.0
    per_pixel = 1.0 + jitter*(band_noise(2, 12, 0.5)*0.5)  # fibers of different length
    for sign in (1.0, -1.0):
        px, py = xx.copy(), yy.copy()
        for i in range(steps):
            vx = map_coordinates(fx, [py, px], order=1, mode="grid-wrap")
            vy = map_coordinates(fy, [py, px], order=1, mode="grid-wrap")
            px += sign*STEP*vx
            py += sign*STEP*vy
            w = np.clip(1.0 - i/(steps*per_pixel), 0.0, 1.0)**1.5
            acc += w*map_coordinates(src, [py, px], order=1, mode="grid-wrap")
            wsum += w
    return acc/np.maximum(wsum, 1e-6)


def sparse(seed_noise, power):
    """white noise where only a few pixels are bright, so the fibers stay apart"""
    return np.exp(power*seed_noise)


fine = lic(sparse(rng.standard_normal((N, N)).astype(np.float32), 0.5), 34)
medium_src = band_noise(12, 90, 0.4)
medium = lic(sparse(medium_src, 0.45), 70)
large = lic(band_noise(3, 22, 1.0), 140)
# a little of the medium streaks make the swooshes fibrous too
large = large + 0.25*(medium - medium.mean())/medium.std()*large.std()

fine = gaussian_filter(fine, 1.6, mode="wrap")  # no single-pixel bristles, they flicker when the sky moves
medium = gaussian_filter(medium, 1.0, mode="wrap")
large = gaussian_filter(large, 1.2, mode="wrap")
out = np.stack([equalize(fine), equalize(medium), equalize(large)], axis=-1)
os.makedirs(os.path.dirname(OUTPUT), exist_ok=True)
Image.fromarray((out*255 + 0.5).astype(np.uint8), "RGB").save(OUTPUT, optimize=True)
print("wrote", os.path.abspath(OUTPUT))
