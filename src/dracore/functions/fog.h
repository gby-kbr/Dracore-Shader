#ifndef DC_FOG_H
#define DC_FOG_H

#include "sky.h"

// Light fog without ray marching. Two layers, both with a closed form:
//   1. the air, thicker near the ground and thinning with height, thicker in rain
//   2. low mist patches that cling to the water and the ground, brighter towards the sun
// Everything is worked out once per vertex in the vertex shader (dcFog), and the
// pixels only do  color*fog.a + fog.rgb.

// Light that survives a path of length dist is exp(-optical depth). The density
// thins out with height, density(y) = d0*exp(-b*(y - base height)), and its integral
// along a straight ray has a closed form.
// camY: height of the camera, dirY: height part of the normalized ray
float dcFogOpticalDepth(float camY, float dirY, float dist, float rain) {
  float b = DC_FOG_FALLOFF*(1.0 - 0.5*rain);
  float density = DC_FOG_DENSITY*(1.0 + DC_FOG_RAIN*rain);
  float x = b*dist*dirY;
  float k = abs(x) < 0.001 ? 1.0 - 0.5*x : (1.0 - exp(-x))/x;
  float d0 = density*exp(-b*max(camY - DC_FOG_BASE_HEIGHT, -15.0));
  return min(d0*dist*k, 8.0);
}

// value noise for the mist patches
float dcHash(vec2 p) {
  p = fract(p*vec2(0.1031, 0.1030));
  p += dot(p, p.yx + 33.33);
  return fract((p.x + p.y)*p.x);
}

float dcNoise(vec2 p) {
  vec2 i = floor(p);
  vec2 f = fract(p);
  f = f*f*(3.0 - 2.0*f);
  return mix(mix(dcHash(i), dcHash(i + vec2(1.0, 0.0)), f.x),
             mix(dcHash(i + vec2(0.0, 1.0)), dcHash(i + vec2(1.0, 1.0)), f.x), f.y);
}

// thickness of the mist patches, 0.0 in the gaps and up to 3.0 inside a bank,
// drifting with the wind. p: world xz in blocks
float dcMistPatch(vec2 p, float t) {
  vec2 q = (p + vec2(1.0, 0.45)*DC_MIST_WIND*t)*DC_MIST_SCALE;
  float n = 0.55*dcNoise(q) + 0.30*dcNoise(q*2.1 + 7.0) + 0.15*dcNoise(q*4.3 + 3.0);
  return 3.0*smoothstep(1.0 - DC_MIST_COVER, 1.0 - DC_MIST_COVER + 0.22, n);
}

// Low mist: density falls quickly with height, so it has a closed form too. The
// patches are looked up at two points along the ray, not only at its end.
float dcGroundMist(vec2 camXZ, vec2 relXZ, float camY, float dirY, float dist, float t, float rain) {
  float b = DC_MIST_FALLOFF;
  float strength = DC_MIST_DENSITY*(1.0 + DC_MIST_RAIN*rain);
  float x = b*dist*dirY;
  float k = abs(x) < 0.001 ? 1.0 - 0.5*x : (1.0 - exp(-x))/x;
  float d0 = strength*exp(-b*max(camY - DC_FOG_BASE_HEIGHT, -10.0));
  float lengthDepth = d0*dist*k;
  if (lengthDepth < 0.002) {
    return 0.0; // from high up or very close the mist is too thin to see, the noise is skipped
  }
  float thickness = 0.5*(dcMistPatch(camXZ + relXZ*0.45, t) + dcMistPatch(camXZ + relXZ*0.9, t));
  return min(lengthDepth*thickness, 8.0);
}

// The fog as  color*fog.a + fog.rgb  (a: how much of the scene survives).
// relPos: from the camera to the surface. camXZ, camY: camera position in the world.
// skyLight: 0.0 ~ 1.0, caves get no sky fog. renderDist: render distance in blocks,
// the fog also hides the edge of the world. Overworld only.
vec4 dcFog(vec3 relPos, vec2 camXZ, float camY, float skyLight, float renderDist, float t, dc_environment env, dc_skycolor sky) {
  float dist = length(relPos);
  vec3 dir = relPos/max(dist, 0.0001);
  float rain = env.rainFactor;
  float open = smoothstep(0.15, 0.75, skyLight);

  float fog = 1.0 - exp(-dcFogOpticalDepth(camY, dir.y, dist, rain)*(0.3 + 0.7*open));
  // the edge of the render distance is always hidden
  float edge = smoothstep(0.75, 1.0, dist/max(renderDist, 1.0));
  float amount = max(fog, edge);

  // the air is the color of the sky in that direction, so the fog meets the sky; rain darkens it
  vec3 fogCol = dcRenderSky(sky, dir)*(1.0 - DC_FOG_RAIN_DARKEN*rain);

  // mist: whiter than the sky and brighter towards the sun, which keeps the same color all day
  float ground = 1.0 - exp(-dcGroundMist(camXZ, relPos.xz, camY, dir.y, dist, t, rain)*open);
  float night = 1.0 - smoothstep(-0.22, 0.0, env.dayFactor);
  float s = max(dot(dir, env.sunDir), 0.0);
  float toSun = (0.6*pow(s, 4.0) + 1.4*pow(s, 24.0))*(1.0 - night)*(1.0 - 0.8*rain);
  vec3 mistCol = fogCol*(1.0 + DC_MIST_BRIGHTNESS*(1.0 - night)) + DC_SUNGLOW_COL*toSun*DC_MIST_SUNLIGHT;

  float surviving = (1.0 - amount)*(1.0 - ground);
  return vec4(fogCol*(amount*(1.0 - ground)) + mistCol*ground, surviving);
}

// The same fog without the mist patches, for the rain and snow particles: there are many of them
// and they do not need the noise.
vec4 dcFogAir(vec3 relPos, float camY, float skyLight, float renderDist, dc_environment env, dc_skycolor sky) {
  float dist = length(relPos);
  vec3 dir = relPos/max(dist, 0.0001);
  float rain = env.rainFactor;
  float open = smoothstep(0.15, 0.75, skyLight);
  float fog = 1.0 - exp(-dcFogOpticalDepth(camY, dir.y, dist, rain)*(0.3 + 0.7*open));
  float amount = max(fog, smoothstep(0.75, 1.0, dist/max(renderDist, 1.0)));
  vec3 fogCol = dcRenderSky(sky, dir)*(1.0 - DC_FOG_RAIN_DARKEN*rain);
  return vec4(fogCol*amount, 1.0 - amount);
}

#endif
