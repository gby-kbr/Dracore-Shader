// Lightweight water. The idea follows newb-x-mcbe (MIT License, Copyright (c) 2024 devendrn,
// see licenses/newb-x-mcbe-MIT.txt): the vanilla water is detected in the vertex shader, its color,
// opacity and sky reflection are worked out per vertex, and the fragment shader only replaces the
// water texture by a plain color and adds the reflection.
// Differences from newb: the waves are two long sine waves with an exact slope instead of noise,
// there is no cloud or torch reflection, and everything is one cheap evaluation per vertex.

#ifndef WATER_H
#define WATER_H

#include "detection.h"
#include "sky.h"
#include "tonemap.h"

#define DC_SUN_GLITTER_RADIANCE 40.0 // brightness of the sun disc in the reflection
#define DC_MOON_GLITTER_RADIANCE 6.0

// fresnel - Schlick's approximation
float dcFresnel(float cosR, float r0) {
  float a = 1.0 - cosR;
  float a2 = a*a;
  return r0 + (1.0 - r0)*a2*a2*a;
}

// small, calm ripples: three waves crossing each other, 3 to 7 blocks long so that one sample per vertex shows them.
// h: [-1.0, 1.0], slope: derivative of h along x and z
void dcWaterWaves(vec2 p, highp float t, out float h, out vec2 slope) {
  const vec2 d1 = vec2(0.8, 0.6);
  const vec2 d2 = vec2(-0.5, 0.866);
  const vec2 d3 = vec2(0.94, -0.34);
  float a1 = 0.9*dot(p, d1) + 0.7*t;
  float a2 = 1.4*dot(p, d2) + 0.9*t;
  float a3 = 2.1*dot(p, d3) + 1.2*t;
  h = 0.5*sin(a1) + 0.3*sin(a2) + 0.2*sin(a3);
  slope = (0.45*cos(a1))*d1 + (0.42*cos(a2))*d2 + (0.42*cos(a3))*d3;
}

// the sun or the moon, whichever is brighter now, as the light that glitters on the water. The reflection of its
// disc is drawn per pixel in the fragment shader, only the direction and the color are worked out here.
// color includes the brightness, in the same units as the sky before the tonemap, it is 0.0 when no light is up
void dcWaterLight(dc_environment env, float skyLight, out vec3 lightDir, out vec3 lightCol) {
  float nightF = 1.0 - smoothstep(-0.12, 0.12, env.dayFactor);
  float twilight = 1.0 - env.dayFactor*env.dayFactor;
  twilight = twilight*twilight*twilight;
  float sunVis = smoothstep(-0.08, 0.12, env.sunDir.y)*(1.0 - nightF); // light under the horizon is gone
  float moonVis = smoothstep(-0.08, 0.12, env.moonDir.y)*nightF;
  vec3 sunCol = mix(vec3(1.15, 1.10, 1.00), vec3(1.50, 0.55, 0.25), twilight)*(DC_SUN_GLITTER_RADIANCE*sunVis);
  vec3 moonCol = vec3(0.62, 0.70, 0.90)*(DC_MOON_GLITTER_RADIANCE*moonVis);
  bool useSun = sunVis >= moonVis;
  lightDir = useSun ? env.sunDir : env.moonDir;
  lightCol = (useSun ? sunCol : moonCol)*(1.0 - 0.8*env.rainFactor)*smoothstep(0.35, 0.9, skyLight); // none in rain, shade or caves
}

// color: the vertex color of the water, replaced here. wPos: lowered a little by the waves
// viewDir: normalized, camera to surface. camPos: absolute camera position. skyLight: 0.0 ~ 1.0
// returns 1.0 in alpha as the water flag (rgb is 0.0, the sky is not reflected)
// glint: the reflected direction (xyz) and the fresnel (w), for the sun and moon reflection per pixel
// lightDir, lightCol: the light that glitters, see dcWaterLight()
vec4 dcWater(inout vec4 color, inout vec3 wPos, dc_skycolor skycol, dc_environment env, vec3 viewDir, vec3 camPos, float skyLight, float camDist, highp float t, out vec4 glint, out vec3 lightDir, out vec3 lightCol) {
  glint = vec4_splat(0.0);
  lightDir = vec3(0.0, 1.0, 0.0);
  lightCol = vec3_splat(0.0);
  float h;
  vec2 slope;
  dcWaterWaves(wPos.xz + camPos.xz, DC_WATER_WAVE_SPEED*t, h, slope);

  // the waves lower the surface a little, only close to the camera
  wPos.y -= DC_WATER_WAVE*(0.5 + 0.5*h)*(1.0 - clamp((camDist - 8.0)*0.125, 0.0, 1.0));

  if (env.underwater) {
    // from below the water is a plain color, no reflection
    color.rgb = DC_WATER_COLOR;
    color.a = DC_WATER_OPACITY;
    return vec4(0.0, 0.0, 0.0, 1.0);
  }

  vec3 nrm = normalize(vec3(-slope.x*DC_WATER_BUMP, 1.0, -slope.y*DC_WATER_BUMP));
  float cosR = max(dot(-viewDir, nrm), 0.0);
  vec3 reflDir = viewDir + 2.0*cosR*nrm; // reflect(viewDir, nrm)

  float fresnel = dcFresnel(cosR, DC_WATER_REFLECTIVITY);
  float opacity = 1.0 - cosR;

  glint = vec4(reflDir, fresnel);
  dcWaterLight(env, skyLight, lightDir, lightCol);

  color.rgb = DC_WATER_COLOR*(1.0 - 0.8*fresnel);
  color.a = mix(DC_WATER_OPACITY, 1.0, opacity*opacity); // clearer from above, more solid at a low angle

  return vec4(0.0, 0.0, 0.0, 1.0); // only the sun and moon are reflected, the sky is not
}

#endif
