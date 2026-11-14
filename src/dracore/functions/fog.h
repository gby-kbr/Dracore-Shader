// Lightweight version of the Bedroalism-Shader fog.
// - height fog with a closed form (no ray marching), thicker low and thinner high
// - a low mist that clings to the water and the ground, with one cheap sample along the ray
//   instead of the noise octaves used in Bedroalism
// Everything is worked out per vertex and handed to the pixels as color*mul + add.

#ifndef FOG_H
#define FOG_H

#include "detection.h"
#include "sky.h"
#include "tonemap.h"
#include "water.h"

#define DC_SEA_LEVEL 63.0

// light that survives a path of length d through the air is exp(-optical depth). The density thins
// out with height, density(y) = d0*exp(-b*(y - sea level)), and its integral along a ray has a closed form
float dcFogOpticalDepth(float camY, float dirY, float dist, float rain, float dawn) {
  float b = DC_FOG_FALLOFF*(1.0 - 0.4*rain);
  float density = DC_FOG_DENSITY*(1.0 + DC_FOG_RAIN*rain)*(1.0 + DC_FOG_DAWN*dawn);
  float x = b*dist*dirY;
  float k = abs(x) < 0.001 ? 1.0 - 0.5*x : (1.0 - exp(-x))/x;
  float d0 = density*exp(-b*max(camY - DC_SEA_LEVEL, -15.0));
  return min(d0*dist*k, 8.0);
}

// low mist that clings to the water and the ground. The density falls quickly with height, so it has a
// closed form too. The patches drift with the wind and are sampled once, in the middle of the ray

float dcGroundMist(float camY, float dirY, float dist, vec2 midXZ, float t, float rain, float dawn) {
  float b = DC_GROUND_MIST_FALLOFF;
  float strength = DC_GROUND_MIST*(0.6 + 1.4*dawn + rain);
  float x = b*dist*dirY;
  float k = abs(x) < 0.001 ? 1.0 - 0.5*x : (1.0 - exp(-x))/x;
  float d0 = strength*exp(-b*max(camY - DC_SEA_LEVEL, -10.0));
  vec2 q = midXZ + vec2(1.0, 0.45)*2.5*t;
  float patchFactor = 0.5 + 0.5*sin(0.045*q.x + 0.03*q.y)*sin(0.03*q.y - 0.02*q.x);
  return min(d0*dist*k*(3.0*patchFactor*patchFactor), 8.0);
}

// relPos: camera to surface. camPos: absolute camera position. skyLight: 0.0 ~ 1.0, caves get no sky fog
// vanillaFogColor, vanillaFog: the game's fog, it still hides the edge of the render distance
// The Nether and the End keep the vanilla fog, under water there is a clear haze.
void dcFogTerms(vec3 relPos, vec3 camPos, float skyLight, float t, dc_environment env, dc_skycolor skycol, vec3 vanillaFogColor, float vanillaFog, out vec3 fogMul, out vec3 fogAdd) {
  if (env.end || env.nether) {
    fogMul = vec3_splat(1.0 - vanillaFog);
    fogAdd = vanillaFogColor*vanillaFog;
    return;
  }
  if (env.underwater) {
    // clear water haze instead of the thick vanilla fog. The light that comes from a surface at distance d keeps
    // exp(-density*d) of its color, the rest is the light that the water scatters on the way
    float keep = exp(-DC_UNDERWATER_DENSITY*length(relPos));
    fogMul = vec3_splat(keep);
    fogAdd = dcWaterScatterColor(env, skyLight)*(1.0 - keep);
    return;
  }
  float dist = length(relPos);
  vec3 dir = relPos/max(dist, 0.0001);
  float dawn = 1.0 - env.dayFactor*env.dayFactor;
  dawn = dawn*dawn*dawn;

  float open = smoothstep(0.15, 0.75, skyLight);
  float fog = 1.0 - exp(-dcFogOpticalDepth(camPos.y, dir.y, dist, env.rainFactor, dawn)*(0.3 + 0.7*open));
  float amount = max(fog, vanillaFog);

  // the air is the color of the sky in that direction, so the fog meets the sky. The chunks have no tonemap
  // of their own, the sky has, so it is applied here
  vec3 fogCol = mix(vanillaFogColor, colorCorrection(dcRenderSky(skycol, dir)), open);

  // ground mist: a little whiter than the fog during the day
  vec2 midXZ = camPos.xz + relPos.xz*0.6;
  float ground = 1.0 - exp(-dcGroundMist(camPos.y, dir.y, dist, midXZ, t, env.rainFactor, dawn)*open);
  vec3 mistCol = fogCol*(1.0 + 0.6*max(env.dayFactor, 0.0));

  fogMul = vec3_splat((1.0 - amount)*(1.0 - ground));
  fogAdd = fogCol*(amount*(1.0 - ground)) + mistCol*ground;
}

#endif
