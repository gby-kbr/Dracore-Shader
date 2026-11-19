#ifndef DC_TONEMAP_H
#define DC_TONEMAP_H

#include "utils.h"

// Reinhard-Jodie tonemap.
// Reinhard driven by luminance, then blended per channel by the per-channel
// result, so saturated colors keep their hue instead of skewing towards
// whichever channel compresses first.
// https://64.github.io/tonemapping/#reinhard-jodie
vec3 dcReinhardJodie(vec3 x) {
  const vec3 lw = vec3(0.2126, 0.7152, 0.0722);

  float l = dot(x, lw);
  vec3 tv = x / (1.0 + x);

  return mix(x / (1.0 + l), tv, tv);
}

// Final color correction: exposure -> tonemap -> gamma.
// Applied once per material, on the last color written to the screen.
vec3 dcColorCorrection(vec3 col) {
#ifdef DC_EXPOSURE
  col *= DC_EXPOSURE;
#endif

  col = dcReinhardJodie(col);

  col = pow(col, vec3_splat(1.0 / DC_GAMMA));

  return col;
}

// Inverse of dcColorCorrection, used to bring an already corrected color
// (e.g. the fog color in the nether) back to linear space.
// Reinhard-Jodie is identical to simple Reinhard for gray colors, so this
// is exact for gray and an approximation for saturated colors.
vec3 dcColorCorrectionInv(vec3 col) {
  col = pow(col, vec3_splat(DC_GAMMA));

  col = min(col, vec3_splat(0.999));
  col = col / (1.0 - col);

#ifdef DC_EXPOSURE
  col /= DC_EXPOSURE;
#endif

  return col;
}

#endif
