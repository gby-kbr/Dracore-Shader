// Sky colors (zenith and horizon) are mixed the way newb-x-mcbe does
// (MIT License, Copyright (c) 2024 devendrn, see licenses/newb-x-mcbe-MIT.txt).
// The gradient in nlRenderSky() and the smooth day/night change come from Bedroalism-Shader.
// Overworld only. No End sky, no horizon edge, no rainbow, no sun glow.

#ifndef SKY_H
#define SKY_H

#include "detection.h"

struct nl_skycolor {
  vec3 zenith;
  vec3 horizon;
};

nl_skycolor nlOverworldSkyColors(nl_environment env) {
  nl_skycolor s;
  float f = 1.0 + 2.0*(1.0-max(-env.dayFactor, 0.0));
  float nightFactor = 1.0 - smoothstep(-0.12, 0.12, env.dayFactor); // soft change, a hard step jumps at sunset
  s.zenith = mix(NL_DAY_ZENITH_COL, NL_NIGHT_ZENITH_COL*f, nightFactor);
  s.horizon = mix(NL_DAY_HORIZON_COL, NL_NIGHT_HORIZON_COL*f, nightFactor);

  float dawnFactor = 1.0-env.dayFactor*env.dayFactor;
  dawnFactor *= dawnFactor*dawnFactor;
  dawnFactor *= mix(1.0, dawnFactor*dawnFactor, nightFactor);
  s.zenith = mix(s.zenith, NL_DAWN_ZENITH_COL, dawnFactor);
  s.horizon = mix(s.horizon, NL_DAWN_HORIZON_COL, dawnFactor);

  float zh = dot(s.zenith, vec3_splat(0.33));
  float hh = dot(s.horizon, vec3_splat(0.33));
  float rainMix = env.rainFactor*NL_SKY_RAIN_MIX_FACTOR;
  s.zenith = mix(s.zenith, NL_RAIN_ZENITH_COL*zh, rainMix);
  s.horizon = mix(s.horizon, NL_RAIN_HORIZON_COL*hh, rainMix);

  if (env.underwater) {
    vec3 underwaterFog = env.fogCol*env.fogCol*NL_UNDERWATER_TINT;
    s.zenith = mix(2.0*underwaterFog, underwaterFog*zh, 0.8);
    s.horizon = mix(2.0*underwaterFog, underwaterFog*hh, 0.8);
  }

  return s;
}

nl_skycolor nlSkyColors(nl_environment env) {
  return nlOverworldSkyColors(env);
}

// viewDir: normalized, y up
vec3 nlRenderSky(nl_skycolor s, vec3 viewDir) {
  float up = max(viewDir.y, 0.0); // below the horizon the sky keeps the horizon color
  float vh = 1.0 - up*up;
  float gradient = vh*vh*vh; // a long soft fade from the horizon to the zenith
  vec3 sky = mix(s.zenith, s.horizon, gradient);
  return sky*(0.5 + 0.3*gradient);
}

#endif
