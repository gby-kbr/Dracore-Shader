"""Generates the tileable sun reflection texture used for the glitter of the sun on water.

Channels (128x128 RGBA, tiles in both directions):
  R  sparkle : fine glitter, spread evenly over 0..1 (average 0.5)
  G  streaks : the same glitter stretched along one direction
  B  unused (0)
  A  255

You can replace the png with your own: R is read as the glitter mask.
Run: python3 script/generate_sun_reflection.py
Needs numpy and Pillow.
"""
import os
import numpy as np
from PIL import Image

SIZE = 128
SEED = 5
OUT = os.path.join(os.path.dirname(__file__), "..", "mcpack", "textures", "environment", "sun_reflection.png")


def spectral(rng, slope, stretch=1.0):
    white = rng.standard_normal((SIZE, SIZE))
    f = np.fft.fftfreq(SIZE)
    r = np.sqrt((f[:, None] * stretch) ** 2 + f[None, :] ** 2)
    r[0, 0] = 1.0
    return np.fft.ifft2(np.fft.fft2(white) / r ** slope).real


def equalize(a):
    ranks = np.argsort(np.argsort(a.ravel()))
    return (ranks / (a.size - 1)).reshape(a.shape)


def main():
    rng = np.random.default_rng(SEED)
    sparkle = equalize(0.65 * equalize(spectral(rng, 0.7)) + 0.35 * equalize(spectral(rng, 1.4)))
    streaks = equalize(spectral(rng, 0.9, stretch=4.0))
    img = np.stack([sparkle, streaks, np.zeros_like(sparkle), np.ones_like(sparkle)], axis=-1)
    Image.fromarray((img * 255 + 0.5).astype(np.uint8), "RGBA").save(OUT)
    print("wrote", os.path.normpath(OUT))


if __name__ == "__main__":
    main()
