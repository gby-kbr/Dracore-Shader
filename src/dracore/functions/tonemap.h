#ifndef DC_TONEMAP_H
#define DC_TONEMAP_H

#include "utils.h"

// sRGB to linear light. Textures are stored in sRGB, the lighting and the tonemap work in linear light
vec3 dcSRGBToLinear(vec3 c) {
  return max(mix(c/12.92, pow(0.947867*c + 0.0521327, vec3_splat(2.4)), step(0.04045, c)), 0.0);
}

// ACES film curve, Narkowicz fit
// https://knarkowicz.wordpress.com/2016/01/06/aces-filmic-tone-mapping-curve/
vec3 dcAcesFilm(vec3 x) {
  return clamp((x*(2.51*x + 0.03))/(x*(2.43*x + 0.59) + 0.14), 0.0, 1.0);
}

// Final color correction: exposure -> ACES film -> linear to sRGB.
// Applied once per material, on the last color written to the screen.
vec3 dcColorCorrection(vec3 col) {
  col = dcAcesFilm(col*DC_EXPOSURE);
  return mix(col*12.92, 1.055*pow(col, vec3_splat(1.0/2.4)) - 0.055, step(0.0031308, col));
}

#endif
