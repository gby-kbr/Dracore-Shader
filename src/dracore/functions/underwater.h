#ifndef DC_UNDERWATER_H
#define DC_UNDERWATER_H

#include "utils.h"
#include "sky.h"
#include "clouds.h"
#include "water.h"

// simple random number from a 2D position
float dcFastRand(vec2 n) {
  return fract(37.45*sin(dot(n, vec2(4.36, 8.28))));
}

// Caustic pattern, the same as in Aerotion (its "disp" function): a moving
// sheet of noise bent by a sine.
// pos: block position tiled every 16 blocks (0..1), t: time scaled by the wave speed
float dcCausticPattern(vec3 pos, float t) {
  float n = sin(8.0*DC_PI_HALF*(pos.x + pos.y*pos.z) + 0.7*t);
  pos.y += t + 0.8*n;
  float p = floor(pos.y);
  return (0.8 + 0.2*n)*mix(dcFastRand(pos.xz + p), dcFastRand(pos.xz + p + 1.0), pos.y - p);
}

// Light added under the water, the same as Aerotion's underwater lighting: a base
// brightness and the caustics where the sky reaches.
// Worked out in the vertex shader, one value per vertex, and blended across the
// face by the GPU (that is what keeps the pattern soft). lit: block light and sky
// light squared, uv1: block light and sky light, tiledCpos: block position tiled
// every 16 blocks.
float dcUnderwaterLightAdd(vec2 lit, vec2 uv1, vec3 tiledCpos, float t) {
  float add = DC_UNDERWATER_BRIGHTNESS;
  if (uv1.y < 0.9) {
    float caustics = dcCausticPattern(tiledCpos, DC_WATER_WAVE_SPEED*t);
    caustics *= 3.0*caustics;
    add += DC_CAUSTIC_INTENSITY*caustics*(0.15 + lit.y + lit.x*0.7);
  }
  return add;
}

// Tint of the water on the light, worked out per pixel. horizonCol is the
// horizon color of the underwater sky, litY the sky light squared.
vec3 dcUnderwaterTint(vec3 horizonCol, float litY) {
  return mix(normalize(horizonCol), vec3_splat(0.6), litY*0.6);
}

// The sky seen from under the water. The sky shader draws the water surface's
// colors only through the water mesh, so the sky itself is shown as it is above
// (sun glow and clouds) and fades to the underwater color towards the horizon.
// A slow wobble bends the view a little, as the surface would.
vec3 dcUnderwaterSky(sampler2D noiseTex, vec3 dir, dc_environment env, float time) {
  // what the sky looks like above the water
  dc_environment dry = env;
  dry.underwater = false;
  dry.rainFactor = 0.0;

  vec2 wobble = dcWaterSlope(dir.xz/(dir.y + 0.35)*3.0, time);
  vec3 skyDir = normalize(vec3(dir.x + wobble.x*DC_UNDERWATER_SKY_WOBBLE, max(dir.y, 0.0), dir.z + wobble.y*DC_UNDERWATER_SKY_WOBBLE));

  dc_skycolor dryColors = dcSkyColors(dry);
  vec3 above = dcRenderSky(dryColors, skyDir) + dcSunMoonGlow(skyDir, dry);
  vec4 clouds = dcClouds(noiseTex, skyDir, dry, dryColors, time);
  above = mix(above, clouds.rgb, clouds.a);

  vec3 below = dcRenderSky(dcSkyColors(env), dir);
  return mix(below, above, smoothstep(0.0, 0.6, dir.y)*DC_UNDERWATER_SKY_VISIBILITY);
}

#endif
