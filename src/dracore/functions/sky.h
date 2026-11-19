#ifndef DC_SKY_H
#define DC_SKY_H

#include "detection.h"

struct dc_skycolor {
  vec3 zenith;
  vec3 horizon;      // day, night, rain or underwater
  vec3 sunlight;     // pale light of the low sun, only on the horizon around it
  float dawn;        // [0.0, 1.0] how much sunrise/sunset light there is
  vec3 sunDir;       // normalized
};

// Day, night, dawn, rain and underwater, mixed the way Newb does, with zenith
// and horizon only.
dc_skycolor dcSkyColors(dc_environment env) {
  dc_skycolor s;

  // soft day/night change, a hard step would jump at sunset
  float f = 1.0 + 2.0*(1.0 - max(-env.dayFactor, 0.0));
  float nightFactor = 1.0 - smoothstep(-0.12, 0.12, env.dayFactor);
  s.zenith = mix(DC_DAY_ZENITH_COL, DC_NIGHT_ZENITH_COL*f, nightFactor);
  s.horizon = mix(DC_DAY_HORIZON_COL, DC_NIGHT_HORIZON_COL*f, nightFactor);

  // strongest while the sun is on the horizon, almost gone at night
  float dawnFactor = 1.0 - env.dayFactor*env.dayFactor;
  dawnFactor *= dawnFactor*dawnFactor;
  dawnFactor *= mix(1.0, dawnFactor*dawnFactor, nightFactor);
  // the sky itself only turns a muted lavender, there is no orange or red
  s.zenith = mix(s.zenith, DC_DAWN_ZENITH_COL, dawnFactor);
  s.horizon = mix(s.horizon, DC_DAWN_HORIZON_COL, dawnFactor);

  // detection reports full rain under water, so it is masked here
  float rain = (env.underwater || env.nether || env.end) ? 0.0 : env.rainFactor;
  float zh = dot(s.zenith, vec3_splat(0.33));
  float hh = dot(s.horizon, vec3_splat(0.33));
  float rainMix = rain*DC_SKY_RAIN_MIX;
  s.zenith = mix(s.zenith, DC_RAIN_ZENITH_COL*zh, rainMix);
  s.horizon = mix(s.horizon, DC_RAIN_HORIZON_COL*hh, rainMix);

  s.sunlight = DC_DAWN_SUNLIGHT_COL;
  s.dawn = dawnFactor*(1.0 - rainMix);
  s.sunDir = env.sunDir;

  if (env.underwater) {
    vec3 underwaterFog = env.fogCol*env.fogCol*DC_UNDERWATER_TINT;
    s.zenith = mix(2.0*underwaterFog, underwaterFog*zh, 0.8);
    s.horizon = mix(2.0*underwaterFog, underwaterFog*hh, 0.8);
    s.dawn = 0.0;
  }

  return s;
}

// viewDir: normalized, y up
vec3 dcRenderSky(dc_skycolor s, vec3 viewDir) {
  // below the horizon the sky keeps the horizon color
  float up = max(viewDir.y, 0.0);

  // one long soft fade from the horizon to the zenith
  float vh = 1.0 - up*up;
  float gradient = vh*vh*vh;

  // the light of the sun only shows on the horizon around it, fading out softly
  float nearSun = smoothstep(0.3, 1.0, dot(viewDir, s.sunDir));
  nearSun *= nearSun;
  vec3 horizon = mix(s.horizon, s.sunlight, s.dawn*nearSun);

  vec3 sky = mix(s.zenith, horizon, gradient);

  // the zenith is darker than the horizon
  return sky*(0.5 + 0.5*gradient);
}

#endif
