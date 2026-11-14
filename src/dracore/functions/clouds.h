#ifndef DC_CLOUDS_H
#define DC_CLOUDS_H

#include "sky.h"

// ---------------------------------------------------------------------------------------------
// Clouds: one layer of soft streaks drawn on the sky, read from textures/environment/cloud_noise.png (made by
// tools/generate_cloud_noise.py; R large shapes, G medium shapes, B fine fibers).
// Per sky pixel that is 4 texture reads and no loop:
//   1. a read of the large shapes bends the coordinates, so the streaks curve
//   2. the clouds themselves, stretched a little along the wind
//   3. the fine fibers at the edges
//   4. a read moved toward the sun: it tells how much cloud the sunlight crosses, that is the shading
// The water reflection and the sun and moon use dcCloudsLite, with 2 reads.
// ---------------------------------------------------------------------------------------------

#define DC_CLOUD_ALONG  vec2(-0.2, 0.98)  // the direction of the wind on the layer
#define DC_CLOUD_ACROSS vec2(-0.98, -0.2)

// a read of the clouds without the light. p: the coordinates on the layer, uv: where the base was read
// returns x: soft density, y: density with the fibers off, z: where the edge of the clouds is
vec3 dcCloudBase(sampler2D tex, vec3 dir, dc_environment env, float time, out vec2 p, out vec2 uv) {
  // no early return below the horizon: a texture read in a branch that changes inside one screen quad gives a line
  // there. The clouds fade out to nothing at the horizon anyway
  float up = max(dir.y, 0.0);
  float lift = 1.0/(up*0.6 + 0.45); // the layer is far away at the horizon
  vec2 plane = dir.xz*lift;
  p = vec2(dot(plane, DC_CLOUD_ALONG), dot(plane, DC_CLOUD_ACROSS));
  p.x -= time*DC_CLOUD_WIND*0.05;

  // the large shapes bend the streaks
  vec2 w = texture2D(tex, vec2(p.x*0.05 + 0.3, p.y*0.06)).rg - 0.5;
  p += vec2(w.g*0.6, w.r)*DC_CLOUD_WARP;

  uv = vec2(p.x*0.16, p.y*0.16*DC_CLOUD_STRETCH);
  vec4 n = texture2D(tex, uv);
  float soft = n.r*0.45 + n.g*0.55;
  float edge = 1.0 - clamp(DC_CLOUD_COVER + 0.3*env.rainFactor, 0.0, 1.0);
  return vec3(soft, soft, edge);
}

// how much clouds hide the sky in a direction: the sun and the moon are behind them
float dcCloudCover(sampler2D tex, vec3 dir, dc_environment env, float time) {
  vec2 p, uv;
  vec3 base = dcCloudBase(tex, dir, env, time, p, uv);
  float dens = smoothstep(base.z, base.z + DC_CLOUD_SOFTNESS, base.x);
  return dens*DC_CLOUD_OPACITY*smoothstep(0.0, 0.16, max(dir.y, 0.0));
}

// the color of the clouds (rgb, before the tonemap) and how much they cover the sky behind (a)
// soft: shape, dm: shape on the way to the sun, d: shape with the fibers, edge: where the clouds begin
vec4 dcCloudShade(vec3 dir, dc_environment env, dc_skycolor skycol, float soft, float dm, float d, float edge) {
  float up = max(dir.y, 0.0);
  float rain = env.rainFactor;
  float dens = smoothstep(edge, edge + DC_CLOUD_SOFTNESS, d);

  float day = max(env.dayFactor, 0.0);
  float night = 1.0 - smoothstep(-0.12, 0.12, env.dayFactor);
  float twi = 1.0 - env.dayFactor*env.dayFactor;
  twi = twi*twi*twi;

  float body = smoothstep(edge + 0.05, edge + 0.45, soft); // the core is deeper than the rim
  float lit = clamp(0.5 + (soft - dm)*10.0 - body*0.5, 0.0, 1.0);
  float rim = 1.0 - smoothstep(edge, edge + 0.35, d);      // thin edges let the light through

  // at sunrise and sunset the sun still lights the clouds, with a warm color
  float sunAmount = day + 0.7*twi;
  vec3 sunCol = mix(vec3(1.0, 0.95, 0.86), vec3(1.0, 0.70, 0.50), twi)*sunAmount*(1.0 - rain*0.7);
  vec3 moonCol = vec3(0.30, 0.38, 0.55)*night;

  // the clouds take their color from the sky
  float skyAmt = 0.55 + 0.25*day + 0.2*twi;
  vec3 zl = skycol.zenith*skyAmt + moonCol*0.2;
  vec3 hl = skycol.horizon*skyAmt + moonCol*0.2;
  vec3 grey = vec3_splat(dot(mix(zl, hl, 0.5), vec3(0.33, 0.33, 0.33))*1.15);
  vec3 amb = grey + (zl - grey)*0.3 + (hl - grey)*0.9;
  amb = mix(amb, vec3_splat(dot(amb, vec3(0.33, 0.33, 0.33))*1.35), day*0.55*(1.0 - twi)*(1.0 - rain)); // white in the day
  amb += skycol.dawnHorizon*(0.12*twi*(1.0 - rain));
  amb = mix(amb, vec3_splat(dot(amb, vec3(0.33, 0.33, 0.33)))*0.75, rain*0.8);

  // at sunrise and sunset the clouds that face the sun glow, the ones that look away stay dim
  vec2 sx = normalize(env.sunDir.xz + vec2(1e-4, 0.0));
  float facing = pow(max(dot(normalize(dir.xz + vec2(1e-4, 0.0)), sx), 0.0), 2.0);
  vec3 glow = vec3(1.0, 0.55, 0.32)*(3.0*twi*(0.3 + 0.7*facing)*(1.0 - rain));

  vec3 col = amb*(0.75 - 0.35*body) + (sunCol*(1.0 + 1.5*twi) + moonCol)*(lit*(0.55 + 0.35*rim)) + glow*((0.5 + 0.5*rim)*(1.0 - 0.3*body));
  col += sunCol*(pow(max(dot(dir, env.sunDir), 0.0), 6.0)*rim*dens*0.6); // silver lining

  // white, a little blue, in the day
  col = mix(col, vec3_splat(dot(col, vec3(0.33, 0.33, 0.33)))*vec3(1.0, 1.0, 1.06)*3.2, 0.7*day*(1.0 - twi)*(1.0 - rain));
  col *= 1.0 + 0.9*day*(1.0 - twi)*(1.0 - rain);

  // the tonemap whitens bright colors: the dusk clouds stay lower and keep a pink color
  col *= mix(1.0, 0.8, twi*(1.0 - rain));
  col = mix(col, col*vec3(1.3, 0.72, 0.78), twi*0.85*(1.0 - rain));

  // rain and night: a plain grey in rain, the color of the sky at night
  vec3 skyHere = dcRenderSky(skycol, dir);
  float skyL = dot(skyHere, vec3(0.33, 0.33, 0.33));
  vec3 rainCol = vec3_splat(skyL)*(1.5 - 0.4*body);
  vec3 nightCol = skyHere*(2.2 - 0.4*body);
  col = mix(col, rainCol, rain);
  col = mix(col, nightCol, night*(1.0 - rain));

  // clouds fade into the haze at the horizon
  return vec4(col, dens*DC_CLOUD_OPACITY*smoothstep(0.0, 0.16, up));
}

// the full clouds, for the sky: 4 reads
vec4 dcClouds(sampler2D tex, vec3 dir, dc_environment env, dc_skycolor skycol, float time) {
  vec2 p, uv;
  vec3 base = dcCloudBase(tex, dir, env, time, p, uv);
  float fiber = texture2D(tex, vec2(p.x*0.12, p.y*7.0) + 0.5).b; // fine fibers along the streak, like hair
  float d = base.x + (fiber - 0.5)*DC_CLOUD_FIBER;

  // how much cloud lies between the point and the sun: the side facing the sun is bright
  vec2 sx = normalize(env.sunDir.xz + vec2(1e-4, 0.0));
  vec2 toSun = vec2(dot(sx, DC_CLOUD_ALONG), dot(sx, DC_CLOUD_ACROSS));
  vec4 m = texture2D(tex, uv + vec2(toSun.x, toSun.y*DC_CLOUD_STRETCH)*(0.012*(0.4 + 0.6*max(env.sunDir.y, 0.0)) + 0.004));
  float dm = m.r*0.45 + m.g*0.55;
  vec4 shade = dcCloudShade(dir, env, skycol, base.x, dm, d, base.z);
  shade.rgb *= 1.0 + (fiber - 0.5)*0.3; // the fibers show inside the cloud too
  return shade;
}

// the clouds for the reflection in the water: 2 reads, no fibers and no shading toward the sun
vec4 dcCloudsLite(sampler2D tex, vec3 dir, dc_environment env, dc_skycolor skycol, float time) {
  vec2 p, uv;
  vec3 base = dcCloudBase(tex, dir, env, time, p, uv);
  return dcCloudShade(dir, env, skycol, base.x, base.x - 0.03, base.x, base.z);
}

#endif
