#ifndef DC_SKY_H
#define DC_SKY_H

#include "detection.h"

struct dc_skycolor {
  vec3 zenith;
  vec3 horizon; // day, night, rain or underwater
};

// colors with nothing set, for variables that are filled in only on some paths
dc_skycolor dcEmptySkyColors() {
  dc_skycolor s;
  s.zenith = vec3_splat(0.0);
  s.horizon = vec3_splat(0.0);
  return s;
}

// light below the horizon is gone
float dcGlowVisibility(float lightHeight) {
  return smoothstep(-0.25, 0.10, lightHeight); // wide: the glow of a sun just below the horizon fades slowly
}

// Day, night, rain and underwater, with zenith and horizon only. Sunrise and
// sunset have no color of their own, neither in the sky nor in the sun.
dc_skycolor dcSkyColors(dc_environment env) {
  dc_skycolor s;

  // soft day/night change, a hard step would jump at sunset
  float f = 1.0 + 2.0*(1.0 - max(-env.dayFactor, 0.0));
  float nightFactor = 1.0 - smoothstep(-0.12, 0.12, env.dayFactor);
  s.zenith = mix(DC_DAY_ZENITH_COL, DC_NIGHT_ZENITH_COL*f, nightFactor);
  s.horizon = mix(DC_DAY_HORIZON_COL, DC_NIGHT_HORIZON_COL*f, nightFactor);

  // detection reports full rain under water, so it is masked here
  float rain = (env.underwater || env.nether || env.end) ? 0.0 : env.rainFactor;
  float zh = dot(s.zenith, vec3_splat(0.33));
  float hh = dot(s.horizon, vec3_splat(0.33));
  float rainMix = rain*DC_SKY_RAIN_MIX;
  s.zenith = mix(s.zenith, DC_RAIN_ZENITH_COL*zh, rainMix);
  s.horizon = mix(s.horizon, DC_RAIN_HORIZON_COL*hh, rainMix);

  if (env.underwater) {
    vec3 underwaterFog = env.fogCol*env.fogCol*DC_UNDERWATER_TINT;
    s.zenith = mix(2.0*underwaterFog, underwaterFog*zh, 0.8);
    s.horizon = mix(2.0*underwaterFog, underwaterFog*hh, 0.8);
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

  vec3 sky = mix(s.zenith, s.horizon, gradient);

  // the zenith is darker than the horizon
  return sky*(0.5 + 0.5*gradient);
}

// angular radius of the visible disc inside the sun and moon quad, for size 1.0
// (the disc is 25% of the quad: 0.25 * 70 / 300)
#define DC_SUNMOON_DISC_RADIUS 0.0583

// Glow around the sun and the moon, added to the sky before color correction.
// The sun has a soft core, wide soft tails and a haze that hugs the horizon
// around it. A low sun shines through more air, so its glow is brighter and
// wider; that only changes the brightness, the color of the sun's light is the
// same all day and does not change at sunrise or sunset.
vec3 dcSunMoonGlow(vec3 dir, dc_environment env) {
  float night = 1.0 - smoothstep(-0.22, 0.0, env.dayFactor);

  float sunAngle = sqrt(2.0*max(1.0 - dot(dir, env.sunDir), 0.0))/(DC_SUNMOON_DISC_RADIUS*DC_SUN_SIZE);
  float moonAngle = sqrt(2.0*max(1.0 - dot(dir, env.moonDir), 0.0))/(DC_SUNMOON_DISC_RADIUS*DC_MOON_SIZE);

  float low = 1.0 - smoothstep(0.0, 0.5, env.sunDir.y);
  // light has no sharp edge: a soft core, then two slowly thinning tails (1/(1 + x^2), halfway at 5 and
  // 18 disc radii) that fade into the whole sky without any visible rim
  float radial = 0.72*exp(-sunAngle*0.9) + 0.54/(1.0 + 0.04*sunAngle*sunAngle) + 0.27/(1.0 + 0.0031*sunAngle*sunAngle);

  // haze along the horizon in the direction of the sun, wide
  vec2 viewFlat = dir.xz/max(length(dir.xz), 0.0001);
  vec2 sunFlat = env.sunDir.xz/max(length(env.sunDir.xz), 0.0001);
  float aroundSun = smoothstep(-0.3, 1.0, dot(viewFlat, sunFlat));
  aroundSun *= aroundSun;
  float up = max(dir.y, 0.0);
  float fade = 1.0 - up*up;
  float horizonHaze = 0.85*aroundSun*fade*fade*low;

  float sunGlow = (radial*(0.35 + 0.65*low) + horizonHaze)*(1.0 + low);
  float moonGlow = exp(-moonAngle*1.2)*0.30 + exp(-moonAngle*0.25)*0.08 + 0.06/(1.0 + 0.05*moonAngle*moonAngle);

  // the sun glows only at dawn and dusk, not when it is high
  float dawn = 1.0 - smoothstep(0.05, 0.35, env.sunDir.y);
  vec3 glow = DC_SUNGLOW_COL*sunGlow*dawn*dcGlowVisibility(env.sunDir.y)*(1.0 - night)
            + vec3(0.55, 0.68, 1.0)*moonGlow*dcGlowVisibility(env.moonDir.y)*night;

  float rain = (env.underwater || env.nether || env.end) ? 0.0 : env.rainFactor;
  return glow*DC_SUNMOON_GLOW*(1.0 - rain);
}


// The sun and moon discs, for the reflection on the water (the game draws the
// real discs only in the sky). Same color all day, no dawn color.
// dir: normalized direction of the ray. sunRadiance and moonRadiance set how
// bright each disc is
vec3 dcSunMoonDisc(vec3 dir, dc_environment env, float sunRadiance, float moonRadiance) {
  // the soft edge is the same share of the disc as in the quad
  float sunOuter = cos(DC_SUNMOON_DISC_RADIUS*DC_SUN_SIZE);
  float sunInner = cos(DC_SUNMOON_DISC_RADIUS*DC_SUN_SIZE*0.96);
  float moonOuter = cos(DC_SUNMOON_DISC_RADIUS*DC_MOON_SIZE);
  float moonInner = cos(DC_SUNMOON_DISC_RADIUS*DC_MOON_SIZE*0.96);

  // like the real quads: each disc is there as long as it is above the horizon, the same at dawn and dusk
  float sun = smoothstep(sunOuter, sunInner, dot(dir, env.sunDir))*smoothstep(-0.06, 0.0, env.sunDir.y);
  float moon = smoothstep(moonOuter, moonInner, dot(dir, env.moonDir))*smoothstep(-0.06, 0.0, env.moonDir.y);

  float rain = (env.underwater || env.nether || env.end) ? 0.0 : env.rainFactor;
  return (vec3(1.15, 1.10, 1.00)*(sun*sunRadiance) + vec3(0.62, 0.70, 0.90)*(moon*moonRadiance))*(1.0 - rain);
}

// A simple sun glow over the terrain: a haze band along the horizon towards the sun (at dawn and dusk) and a
// soft bloom round the sun, which also darkens the color it covers a little. Applied to the color of a surface
// after the fog. rd: view direction, sunDir: direction of the sun, rain: [0.0, 1.0]
vec3 dcTerrainSunGlow(vec3 rd, vec3 sunDir, vec3 color, float rain) {
  float hl = length(rd.xz);
  float sunHz = length(sunDir.xz);
  vec2 sunDirXZ = sunHz > 0.0001 ? sunDir.xz/sunHz : vec2(1.0, 0.0);
  float az = hl > 0.0001 ? dot(rd.xz/hl, sunDirXZ) : 0.0;
  float sunAmt = max(dot(rd, sunDir), 0.0001);
  float s2 = sunAmt*sunAmt;
  float s4 = s2*s2;
  float s16 = s4*s4*s4*s4;
  float s64 = s16*s16*s16*s16;
  float azPos = max(az*0.5 + 0.5, 0.0001);
  float sunset = 1.0 - smoothstep(0.05, 0.35, sunDir.y);
  float band = exp(-abs(rd.y)*6.0)*pow(azPos, 1.5)*sunset;
  float glowWeight = band*0.40 + s4*0.18 + s16*0.32 + s64*0.45;
  // only while the sun is up, never in rain
  glowWeight *= smoothstep(-0.1, 0.05, sunDir.y)*(1.0 - rain)*DC_TERRAIN_GLOW;
  float horFade = smoothstep(-0.2, 0.05, rd.y);
  return color*(1.0 - clamp(glowWeight*0.7, 0.0, 0.5)*horFade) + DC_SUNGLOW_COL*(glowWeight*horFade);
}

#endif
