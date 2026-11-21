#ifndef DC_WATER_H
#define DC_WATER_H

#include "utils.h"
#include "clouds.h"

// Thin, light waves: four sine waves that all run roughly the same way, from long
// to short, so the surface reads as fine ripples and not as round bumps. The slope
// is worked out directly, no texture and no noise, so it costs almost nothing.
// p: world position on the water (xz), t: time in seconds
vec2 dcWaterSlope(vec2 p, float t) {
  p *= DC_WATER_WAVE_SCALE;
  float s = DC_WATER_WAVE_SPEED*t;

  // a very slow bend so the crests are not perfectly straight
  p += 0.12*vec2(sin(0.5*p.y + 0.3*s), sin(0.4*p.x - 0.2*s));

  vec2 k0 = vec2(2.5, 0.0);
  vec2 k1 = vec2(3.3, 1.6);
  vec2 k2 = vec2(4.9, -2.9);
  vec2 k3 = vec2(5.8, 6.9);
  return k0*(0.16*cos(dot(k0, p) + 1.1*s))
       + k1*(0.13*cos(dot(k1, p) + 1.6*s + 1.0))
       + k2*(0.10*cos(dot(k2, p) + 2.1*s + 2.0))
       + k3*(0.07*cos(dot(k3, p) + 2.7*s + 3.0));
}

// Normal of the top of the water, pointing up. The waves fade out far away, where
// they would only flicker.
vec3 dcWaterNormal(vec2 p, float t, float camDist) {
  float fade = 1.0 - smoothstep(30.0, 120.0, camDist);
  vec2 g = dcWaterSlope(p, t)*(DC_WATER_BUMP*fade);
  return normalize(vec3(-g.x, 1.0, -g.y));
}

// Schlick's approximation
float dcFresnel(float cosR, float r0) {
  float a = 1.0 - cosR;
  float a2 = a*a;
  return r0 + (1.0 - r0)*a2*a2*a;
}

// Color and opacity of the water surface. Seen the same from every angle: looking straight down, the waves
// show as much as at a low angle, and the water is as clear.
// color: the vanilla water color, lit. baseAlpha: the vanilla water alpha.
// Returns the strength of the reflection to add on top.
float dcWaterShade(inout vec4 color, float baseAlpha) {
  color.rgb *= DC_WATER_COLOR*(1.0 - 0.8*DC_WATER_MIRROR);
  color.a = baseAlpha*DC_WATER_TRANSPARENCY;
  return DC_WATER_MIRROR*DC_WATER_REFLECTION;
}

// How much the waves show as light and dark ripples, [-1.0, 1.0] about, in any view direction. The
// reflection alone hardly changes when looking down, this keeps the waves visible there.
float dcWaterRipple(vec2 p, float t, float camDist) {
  float fade = 1.0 - smoothstep(30.0, 120.0, camDist);
  return dot(dcWaterSlope(p, t), vec2(0.6, 0.8))*DC_WATER_WAVE_SHADE*fade;
}

// Everything the sky shows in one direction, for the reflection on the water:
// gradient, glow, the sun and moon discs, and the clouds in front of them.
// discRadiance: x sun, y moon, 0.0 hides the discs. camPos: camera position in the world
vec3 dcSkyRadiance(vec3 dir, dc_environment env, dc_skycolor sky, vec2 discRadiance, sampler2D noiseTex, vec3 camPos, float time) {
  vec3 glow = dcSunMoonGlow(dir, env);
  vec4 clouds = dcClouds(noiseTex, dir, env, sky, camPos, glow, time);
  vec3 behind = dcRenderSky(sky, dir) + glow + dcSunMoonDisc(dir, env, discRadiance.x, discRadiance.y);
  // the water shows the clouds fainter than the sky does, so the surface is not a copy of the sky
  return mix(behind, clouds.rgb, clouds.a*DC_WATER_CLOUD_REFLECTION);
}

#endif
