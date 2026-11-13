// Dracore color correction: exposure, Reinhard-Jodie tonemap, gamma.
// No saturation and no tint. Exposure and gamma settings are the newb-x-mcbe ones
// (MIT License, Copyright (c) 2024 devendrn, see licenses/newb-x-mcbe-MIT.txt).
// Reinhard-Jodie idea: https://64.github.io/tonemapping/#reinhard-jodie

#ifndef TONEMAP_H
#define TONEMAP_H

#include "utils.h"

// Reinhard on the luminance keeps the hue of saturated colors, Reinhard per channel keeps them
// from turning grey. The per channel result decides how much of each is used
vec3 nlReinhardJodie(vec3 col) {
  float l = luminance(col);
  vec3 perChannel = col/(1.0 + col);
  vec3 byLuminance = col/(1.0 + l);
  return mix(byLuminance, perChannel, perChannel);
}

vec3 colorCorrection(vec3 col) {
  col *= NL_EXPOSURE;
  col = nlReinhardJodie(col);
  return pow(col, vec3_splat(1.0/NL_GAMMA));
}

#endif
