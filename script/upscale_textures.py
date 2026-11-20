"""Makes the weather and particle textures at twice the resolution, with round instead of square edges.

Sources (imported from Newb) are in script/sources/. For every texture the picture is enlarged four times with
the nearest pixel, softened a little so the stair steps become curves, and averaged down to twice the size.
Colors are handled premultiplied by alpha, so the edges never pick up dark or white fringes.

weather.png: only the rain streaks and snow cells are kept (rows 0-39); the vanilla color squares below them
are removed, they would be drawn as squares in the game.

Usage: python3 script/upscale_textures.py   (needs numpy, scipy and Pillow)
"""
import os

import numpy as np
from PIL import Image
from scipy import ndimage

HERE = os.path.dirname(__file__)
OUT = os.path.join(HERE, "..", "mcpack", "textures")
SOFTEN = 1.25  # blur radius at 4x scale, in pixels of the 4x picture


def upscale2x(img):
    rgba = np.asarray(img.convert("RGBA")).astype(np.float32) / 255.0
    h, w = rgba.shape[:2]
    pre = rgba.copy()
    pre[..., :3] *= pre[..., 3:4]
    big = pre.repeat(4, 0).repeat(4, 1)
    big = ndimage.gaussian_filter(big, (SOFTEN, SOFTEN, 0), mode="nearest")
    small = big.reshape(h * 2, 2, w * 2, 2, 4).mean((1, 3))
    a = small[..., 3:4]
    rgb = np.where(a > 1e-4, small[..., :3] / np.maximum(a, 1e-4), 0.0)
    out = np.concatenate([np.clip(rgb, 0, 1), np.clip(a, 0, 1)], -1)
    return Image.fromarray((out * 255 + 0.5).astype(np.uint8), "RGBA")


def main():
    weather = Image.open(os.path.join(HERE, "sources", "weather_newb.png")).convert("RGBA")
    weather = weather.crop((0, 0, 64, 40))
    canvas = Image.new("RGBA", (64, 64), (0, 0, 0, 0))
    canvas.paste(weather, (0, 0))
    path = os.path.join(OUT, "environment", "weather.png")
    upscale2x(canvas).save(path, optimize=True)
    print("wrote", os.path.normpath(path))

    particles = Image.open(os.path.join(HERE, "sources", "particles_newb.png"))
    path = os.path.join(OUT, "particle", "particles.png")
    upscale2x(particles).save(path, optimize=True)
    print("wrote", os.path.normpath(path))


if __name__ == "__main__":
    main()
