#ifndef DC_SKY_H
#define DC_SKY_H

#include "detection.h"

struct dc_skycolor {
  vec3 zenith;
  vec3 horizon;      // day, night, rain or underwater
  vec3 sunlight;     // pale light of the low sun, only on the horizon around it
  float sunlightAmount; // [0.0, 1.0] how much low-sun light there is
  vec3 sunDir;       // normalized
};

// light below the horizon is gone
float dcGlowVisibility(float lightHeight) {
  return smoothstep(-0.08, 0.12, lightHeight);
}

// Day, night, rain and underwater, with zenith and horizon only. Sunrise and
// sunset have no sky color of their own.
dc_skycolor dcSkyColors(dc_environment env) {
  dc_skycolor s;

  // soft day/night change, a hard step would jump at sunset
  float f = 1.0 + 2.0*(1.0 - max(-env.dayFactor, 0.0));
  float nightFactor = 1.0 - smoothstep(-0.12, 0.12, env.dayFactor);
  s.zenith = mix(DC_DAY_ZENITH_COL, DC_NIGHT_ZENITH_COL*f, nightFactor);
  s.horizon = mix(DC_DAY_HORIZON_COL, DC_NIGHT_HORIZON_COL*f, nightFactor);

  // light of the low sun: strongest while the sun is on the horizon, almost gone at night
  float lowSun = 1.0 - env.dayFactor*env.dayFactor;
  lowSun *= lowSun*lowSun;
  lowSun *= mix(1.0, lowSun*lowSun, nightFactor);

  // detection reports full rain under water, so it is masked here
  float rain = (env.underwater || env.nether || env.end) ? 0.0 : env.rainFactor;
  float zh = dot(s.zenith, vec3_splat(0.33));
  float hh = dot(s.horizon, vec3_splat(0.33));
  float rainMix = rain*DC_SKY_RAIN_MIX;
  s.zenith = mix(s.zenith, DC_RAIN_ZENITH_COL*zh, rainMix);
  s.horizon = mix(s.horizon, DC_RAIN_HORIZON_COL*hh, rainMix);

  s.sunlight = DC_SUNLIGHT_COL;
  // there is no sunlight once the sun is below the horizon
  s.sunlightAmount = lowSun*dcGlowVisibility(env.sunDir.y)*(1.0 - rainMix);
  s.sunDir = env.sunDir;

  if (env.underwater) {
    vec3 underwaterFog = env.fogCol*env.fogCol*DC_UNDERWATER_TINT;
    s.zenith = mix(2.0*underwaterFog, underwaterFog*zh, 0.8);
    s.horizon = mix(2.0*underwaterFog, underwaterFog*hh, 0.8);
    s.sunlightAmount = 0.0;
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
  vec3 horizon = mix(s.horizon, s.sunlight, s.sunlightAmount*nearSun);

  vec3 sky = mix(s.zenith, horizon, gradient);

  // the zenith is darker than the horizon
  return sky*(0.5 + 0.5*gradient);
}

// angular radius of the visible disc inside the sun and moon quad, for size 1.0
// (the disc is 25% of the quad: 0.25 * 70 / 300)
#define DC_SUNMOON_DISC_RADIUS 0.0583

// Soft halo around the sun and the moon: a bright core fading into a wide faint
// glow. Added to the sky before color correction. The light of the sun stays
// pale and only warms a little at sunrise and sunset.
vec3 dcSunMoonGlow(vec3 dir, dc_environment env) {
  float day = smoothstep(0.0, 0.4, env.dayFactor);
  float night = 1.0 - smoothstep(-0.22, 0.0, env.dayFactor);
  float twilight = clamp(1.0 - day - night, 0.0, 1.0);

  float sunAngle = sqrt(2.0*max(1.0 - dot(dir, env.sunDir), 0.0))/(DC_SUNMOON_DISC_RADIUS*DC_SUN_SIZE);
  float moonAngle = sqrt(2.0*max(1.0 - dot(dir, env.moonDir), 0.0))/(DC_SUNMOON_DISC_RADIUS*DC_MOON_SIZE);
  float sunGlow = exp(-sunAngle*1.1)*0.45 + exp(-sunAngle*0.22)*0.14;
  float moonGlow = exp(-moonAngle*1.2)*0.30 + exp(-moonAngle*0.25)*0.08;

  vec3 sunCol = mix(vec3(1.0, 0.95, 0.85), vec3(1.0, 0.88, 0.66), twilight*0.6);
  vec3 glow = sunCol*sunGlow*dcGlowVisibility(env.sunDir.y)*(1.0 - night)
            + vec3(0.55, 0.68, 1.0)*moonGlow*dcGlowVisibility(env.moonDir.y)*night;

  float rain = (env.underwater || env.nether || env.end) ? 0.0 : env.rainFactor;
  return glow*DC_SUNMOON_GLOW*(1.0 - 0.8*rain);
}

#endif
