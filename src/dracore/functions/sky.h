// Sky colors (zenith and horizon) are mixed the way newb-x-mcbe does
// (MIT License, Copyright (c) 2024 devendrn, see licenses/newb-x-mcbe-MIT.txt).
// The gradient in dcRenderSky() and the smooth day/night change come from Bedroalism-Shader.
// Overworld only. No End sky, no horizon edge, no rainbow, no sun glow.

#ifndef SKY_H
#define SKY_H

#include "detection.h"

struct dc_skycolor {
  vec3 zenith;
  vec3 horizon;      // day, night and rain only
  vec3 dawnHorizon;
  float dawn;        // [0.0, 1.0] how much dawn there is, it only reaches the horizon near the sun
  vec3 sunDir;
};

dc_skycolor dcOverworldSkyColors(dc_environment env) {
  dc_skycolor s;
  float f = 1.0 + 2.0*(1.0-max(-env.dayFactor, 0.0));
  float nightFactor = 1.0 - smoothstep(-0.12, 0.12, env.dayFactor); // soft change, a hard step jumps at sunset
  s.zenith = mix(DC_DAY_ZENITH_COL, DC_NIGHT_ZENITH_COL*f, nightFactor);
  s.horizon = mix(DC_DAY_HORIZON_COL, DC_NIGHT_HORIZON_COL*f, nightFactor);

  float dawnFactor = 1.0-env.dayFactor*env.dayFactor;
  dawnFactor *= dawnFactor*dawnFactor;
  dawnFactor *= mix(1.0, dawnFactor*dawnFactor, nightFactor);
  s.zenith = mix(s.zenith, DC_DAWN_ZENITH_COL, dawnFactor);

  float zh = dot(s.zenith, vec3_splat(0.33));
  float hh = dot(s.horizon, vec3_splat(0.33));
  float rainMix = env.rainFactor*DC_SKY_RAIN_MIX_FACTOR;
  s.zenith = mix(s.zenith, DC_RAIN_ZENITH_COL*zh, rainMix);
  s.horizon = mix(s.horizon, DC_RAIN_HORIZON_COL*hh, rainMix);

  // the dawn color only reaches the horizon around the sun, see dcRenderSky()
  s.dawnHorizon = DC_DAWN_HORIZON_COL;
  s.dawn = dawnFactor*(1.0 - rainMix);
  s.sunDir = env.sunDir;

  if (env.underwater) {
    s.dawn = 0.0;
    vec3 underwaterFog = env.fogCol*env.fogCol*DC_UNDERWATER_TINT;
    s.zenith = mix(2.0*underwaterFog, underwaterFog*zh, 0.8);
    s.horizon = mix(2.0*underwaterFog, underwaterFog*hh, 0.8);
  }

  return s;
}

dc_skycolor dcSkyColors(dc_environment env) {
  return dcOverworldSkyColors(env);
}

// viewDir: normalized, y up
vec3 dcRenderSky(dc_skycolor s, vec3 viewDir) {
  float up = max(viewDir.y, 0.0); // below the horizon the sky keeps the horizon color
  float vh = 1.0 - up*up;
  float gradient = vh*vh*vh; // a long soft fade from the horizon to the zenith
  float nearSun = smoothstep(-0.2, 0.9, dot(viewDir, s.sunDir)); // dawn color only around the sun
  vec3 horizon = mix(s.horizon, s.dawnHorizon, s.dawn*nearSun);
  vec3 sky = mix(s.zenith, horizon, gradient);
  return sky*(0.5 + 0.3*gradient);
}

#endif
