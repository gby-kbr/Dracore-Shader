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

  // a light S-curve: deeper shadows and brighter highlights, so the picture has depth and snap
  col = mix(col, col*col*(3.0 - 2.0*col), DC_CONTRAST);

  // vibrance: lifts only the muted colors (grass, wood, stone, the sky near the horizon) and leaves colors that
  // are already strong alone, so nothing turns neon like a plain saturation boost would
  float lum = dcLuminance(col);
  float chroma = max(max(col.r, col.g), col.b) - min(min(col.r, col.g), col.b);
  col = mix(vec3_splat(lum), col, 1.0 + DC_VIBRANCE*(1.0 - smoothstep(0.0, 0.6, chroma)));

  // balance: a little warmth in the light parts and a little cool blue in the shadows
  float shadow = 1.0 - smoothstep(0.0, 0.5, lum);
  float light = smoothstep(0.35, 1.0, lum);
  col *= vec3_splat(1.0) + DC_COLOR_BALANCE*(light*vec3(0.06, 0.02, -0.07) + shadow*vec3(-0.04, -0.01, 0.06));

  return col;
}

#endif
