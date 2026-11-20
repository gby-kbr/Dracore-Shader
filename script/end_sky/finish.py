"""Glow of the black hole, tone mapping, dithering and the PNG."""
import numpy as np, os, sys
from PIL import Image
from scipy import ndimage
from common import *
import bh

# The glow is a stack of many Gaussians whose widths grow geometrically (sigma in degrees). Their weights fall
# off slowly with the width, which makes a smooth halo with a power-law fall-off: no visible shelves or rings, the
# light thins out gradually over a large part of the sky. The tint goes from white in the core to blue far away.
GLOW_SIGMAS = tuple(0.30*1.62**i for i in range(10))          # 0.3 .. ~21 degrees
GLOW_POWER = float(os.environ.get("DC_GLOW_POWER", "0.48"))   # higher: more light far from the black hole
GLOW_GAIN = float(os.environ.get("DC_GLOW_GAIN", "0.8"))
CORE, MID, FAR = np.array((1.00, 1.00, 1.00)), np.array((0.62, 0.80, 1.00)), np.array((0.30, 0.50, 0.95))

def glow_layers():
    out = []
    n = len(GLOW_SIGMAS) - 1
    for i, sg in enumerate(GLOW_SIGMAS):
        t = i/n
        tint = CORE*(1 - smoothstep(0.0, 0.45, t)) + MID*(smoothstep(0.0, 0.45, t) - smoothstep(0.45, 1.0, t)) + FAR*smoothstep(0.45, 1.0, t)
        out.append((sg, GLOW_GAIN*0.30*sg**GLOW_POWER, tint))
    return out

def blur_wrap(img, sx, sy):
    """Gaussian blur of an equirectangular layer, wrapping in longitude. Wide blurs run on a reduced copy."""
    f = max(k for k in range(1, max(2, int(min(sx, sy)/8)) + 1) if img.shape[0] % k == 0 and img.shape[1] % k == 0)
    if f > 1:
        small = img.reshape(img.shape[0]//f, f, img.shape[1]//f, f).mean((1, 3))
        small = ndimage.gaussian_filter(small, (sy/f, sx/f), mode=('nearest', 'wrap'))
        return ndimage.zoom(small, f, order=1, mode='grid-wrap')
    return ndimage.gaussian_filter(img, (sy, sx), mode=('nearest', 'wrap'))

def finish(lin, emit, win, W, H, out_png, bloom=1.0, reflect=0.0):
    j0, j1, i0, i1 = win
    ppd = W/360.0
    e = emit.mean(-1).astype(np.float32)
    kx = 1.0/np.cos(BH_EL)                       # longitude is stretched at the elevation of the black hole
    glow = np.zeros_like(lin)
    for sigma_deg, wgt, tint in glow_layers():
        g = blur_wrap(e, sigma_deg*ppd*kx, sigma_deg*ppd)
        glow += (g*wgt*bloom)[..., None]*np.array(tint, np.float32)
    # the shadow of the black hole stays dark: the glow lives outside of it
    D0 = bh.frame()[0]
    d, la, lo = grid(W, H)
    psi = np.arccos(np.clip(d @ D0, -1, 1))
    psi_s = np.arcsin(5.196*np.sqrt(1 - 2/bh.R0)/bh.R0)          # angular radius of the shadow
    keep = smoothstep(0.80*psi_s, 1.02*psi_s, psi)
    glow *= (0.04 + 0.96*keep)[..., None]
    x = lin + glow
    # tone: soft shoulder, keeps the colour of the highlights
    white = 5.0
    lum = x.max(-1, keepdims=True)
    y = np.clip(x*(1 + lum/(white*white))/(1 + lum), 0, 1)
    srgb = np.where(y <= 0.0031308, 12.92*y, 1.055*np.power(y, 1/2.4) - 0.055)
    rng = np.random.default_rng(3)
    srgb = srgb + (rng.random(srgb.shape, dtype=np.float32) - 0.5)/255.0       # dither, hides banding in the dark sky
    img = (np.clip(srgb, 0, 1)*255 + 0.5).astype(np.uint8)
    Image.fromarray(img, 'RGB').save(out_png, optimize=False)
    return img

if __name__ == '__main__':
    W = int(sys.argv[1]); H = W//2
    lin = np.load('linear_nobloom.npy'); emit = np.load('emit.npy'); win = np.load('win.npy')
    finish(lin, emit, win, W, H, sys.argv[2])
