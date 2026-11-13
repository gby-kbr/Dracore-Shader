// Torch (block light) color is taken from newb-x-mcbe
// (MIT License, Copyright (c) 2024 devendrn, see licenses/newb-x-mcbe-MIT.txt).
// The color selection per dimension and the way torch light is added on top of
// the ambient light (torch/(1+lum)) follow newb. The brightness follows the
// vanilla block light curve, because this material has no HDR lighting or tonemap.

#ifndef LIGHTING_H
#define LIGHTING_H

#include "detection.h"
#include "utils.h"

vec3 dcTorchColor(bool underwater, bool end, bool nether) {
  if (underwater) {
    return DC_UNDERWATER_TORCH_COL;
  } else if (end) {
    return DC_END_TORCH_COL;
  } else if (nether) {
    return DC_NETHER_TORCH_COL;
  }
  return DC_OVERWORLD_TORCH_COL;
}

// ambient: light without any block light
// blockLight: vanilla block light brightness [0.0, 1.0], the same for torches, lanterns, glowstone, lava, ...
vec3 dcAddTorchLight(vec3 ambient, vec3 torchColor, float blockLight) {
  // keep the brightness of the vanilla curve and only replace its color
  vec3 torchLight = DC_TORCHLIGHT_INTENSITY*blockLight*torchColor/luminance(torchColor);
  float lum = luminance(ambient);
  return ambient + torchLight/(1.0+lum);
}

#endif
