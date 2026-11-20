#ifndef DC_UNDERWATER_H
#define DC_UNDERWATER_H

#include "utils.h"
#include "sky.h"
#include "clouds.h"
#include "water.h"

// Tint of the water on the light, worked out per pixel. horizonCol is the
// horizon color of the underwater sky, litY the sky light squared.
vec3 dcUnderwaterTint(vec3 horizonCol, float litY) {
  return mix(normalize(horizonCol), vec3_splat(0.6), litY*0.6);
}

// The sky seen from under the water. The sky shader draws the water surface's
// colors only through the water mesh, so the sky itself is shown as it is above
// (sun glow and clouds) and fades to the underwater color towards the horizon.
// A slow wobble bends the view a little, as the surface would.
vec3 dcUnderwaterSky(sampler2D noiseTex, vec3 dir, dc_environment env, vec3 camPos, float time) {
  // what the sky looks like above the water
  dc_environment dry = env;
  dry.underwater = false;
  dry.rainFactor = 0.0;

  vec2 wobble = dcWaterSlope(dir.xz/(dir.y + 0.35)*3.0, time);
  vec3 skyDir = normalize(vec3(dir.x + wobble.x*DC_UNDERWATER_SKY_WOBBLE, max(dir.y, 0.0), dir.z + wobble.y*DC_UNDERWATER_SKY_WOBBLE));

  dc_skycolor dryColors = dcSkyColors(dry);
  vec3 glow = dcSunMoonGlow(skyDir, dry);
  vec3 above = dcRenderSky(dryColors, skyDir) + glow;
  vec4 clouds = dcClouds(noiseTex, skyDir, dry, dryColors, camPos, glow, time);
  above = mix(above, clouds.rgb, clouds.a);

  vec3 below = dcRenderSky(dcSkyColors(env), dir);
  return mix(below, above, smoothstep(0.0, 0.6, dir.y)*DC_UNDERWATER_SKY_VISIBILITY);
}

// Fog under the water. Clear water: the light is lost slowly and evenly, in a light turquoise, so you can
// see far, and nothing is hidden behind a blue wall. As  color*fog.a + fog.rgb, dist in blocks
vec4 dcUnderwaterFog(float dist) {
  float amount = 1.0 - exp(-DC_UNDERWATER_DENSITY*dist);
  return vec4(DC_UNDERWATER_FOG_COL*amount, 1.0 - amount);
}

#endif
