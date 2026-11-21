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

// ACES film curve, Narkowicz fit: filmic contrast, deep shadows and a bright, saturated midtone
// https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
vec3 dcAcesFilm(vec3 x) {
  return clamp((x*(2.51*x + 0.03))/(x*(2.43*x + 0.59) + 0.14), 0.0, 1.0);
}

// ACES with the input and output color matrices (Stephen Hill's fit): the most film-like, but it shifts
// bright colors towards white and pulls greens darker
// https://github.com/TheRealMJP/BakingLab/blob/master/BakingLab/ACES.hlsl
vec3 dcAcesHill(vec3 c) {
  c = vec3(dot(vec3(0.59719, 0.35458, 0.04823), c),
           dot(vec3(0.07600, 0.90834, 0.01566), c),
           dot(vec3(0.02840, 0.13383, 0.83777), c));
  vec3 a = c*(c + 0.0245786) - 0.000090537;
  vec3 b = c*(0.983729*c + 0.4329510) + 0.238081;
  c = a/b;
  return clamp(vec3(dot(vec3(1.60475, -0.53108, -0.07367), c),
                    dot(vec3(-0.10208, 1.10813, -0.00605), c),
                    dot(vec3(-0.00327, -0.07276, 1.07602), c)), 0.0, 1.0);
}

// Final color correction: exposure -> tonemap -> gamma.
// Applied once per material, on the last color written to the screen.
vec3 dcColorCorrection(vec3 col) {
  col *= DC_EXPOSURE;

#if DC_TONEMAP == 1
  col = dcAcesFilm(col);
#elif DC_TONEMAP == 2
  col = dcAcesHill(col);
#else
  col = dcReinhardJodie(col);
#endif

  col = pow(col, vec3_splat(1.0 / DC_GAMMA));

  return max(col, vec3_splat(0.0));
}

#endif
