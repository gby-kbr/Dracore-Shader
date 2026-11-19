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

// Color and opacity of the water surface. cosR is the angle between the view
// and the surface normal, taken as an absolute value, so the surface is seen
// the same from above and from under the water: clear when looked at straight,
// more opaque and more mirror-like at a low angle.
// color: the vanilla water color, lit. baseAlpha: the vanilla water alpha.
// Returns the strength of the reflection to add on top.
float dcWaterShade(inout vec4 color, float baseAlpha, float cosR) {
  cosR = abs(cosR);
  float fresnel = dcFresnel(cosR, DC_WATER_FRESNEL);
  float opacity = 1.0 - cosR;

  color.rgb *= DC_WATER_COLOR*(1.0 - 0.8*fresnel);
  color.a = mix(baseAlpha*DC_WATER_TRANSPARENCY, 1.0, opacity*opacity);

  return fresnel*DC_WATER_REFLECTION;
}


// Everything the sky shows in one direction, for the reflection on the water:
// gradient, glow, the sun and moon discs, and the clouds in front of them.
// discRadiance: x sun, y moon, 0.0 hides the discs
vec3 dcSkyRadiance(vec3 dir, dc_environment env, dc_skycolor sky, vec2 discRadiance, sampler2D noiseTex, float time) {
  vec4 clouds = dcClouds(noiseTex, dir, env, sky, time);
  vec3 behind = dcRenderSky(sky, dir) + dcSunMoonGlow(dir, env) + dcSunMoonDisc(dir, env, discRadiance.x, discRadiance.y);
  return mix(behind, clouds.rgb, clouds.a);
}

#endif
