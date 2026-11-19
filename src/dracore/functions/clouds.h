#ifndef DC_CLOUDS_H
#define DC_CLOUDS_H

#include "sky.h"

// Cumulus clouds: one flat layer drawn in the sky pass. Three reads of one
// small noise texture (see script/generate_cloud_noise.py):
//   R coverage, G puffs, B fine detail.
// Puffy lighting comes from comparing the cloud density with the density a
// little closer to the light: the side facing the light is bright, the side
// facing away is a soft grey-blue.

float dcCloudDensityField(vec4 n) {
  return mix(n.r, n.g, 0.55) + (n.b - 0.5)*DC_CLOUD_ROUGHNESS;
}

// returns rgb (before color correction) and coverage in alpha
vec4 dcClouds(sampler2D noiseTex, vec3 dir, dc_environment env, float time) {
  float up = dir.y;
  float horizonFade = smoothstep(0.0, 0.22, up);
  if (horizonFade <= 0.0) return vec4(0.0, 0.0, 0.0, 0.0);

  // flat layer: the farther from the zenith, the more squeezed the clouds look
  vec2 uv = dir.xz/(up + 0.12);
  uv += vec2(1.0, 0.35)*time*DC_CLOUD_WIND;

  // slow bend so the clouds do not look like a repeating tile
  vec2 warp = (texture2D(noiseTex, uv*0.09).rg - 0.5)*0.12;
  vec2 p = (uv + warp)*DC_CLOUD_SCALE;

  float rain = (env.underwater || env.nether || env.end) ? 0.0 : env.rainFactor;
  float nightFactor = 1.0 - smoothstep(-0.12, 0.12, env.dayFactor);

  // the light that shapes the clouds: the sun by day, the moon by night
  vec3 lightDir = mix(env.sunDir, env.moonDir, nightFactor);
  vec2 lightFlat = lightDir.xz/max(length(lightDir.xz), 0.0001);
  float lightLen = 0.045*(1.3 - 0.7*clamp(lightDir.y, 0.0, 1.0));

  vec4 shape = texture2D(noiseTex, p);
  vec4 toLight = texture2D(noiseTex, p + lightFlat*lightLen*DC_CLOUD_SCALE);

  float f = dcCloudDensityField(shape);
  float fl = dcCloudDensityField(toLight);

  float cover = mix(DC_CLOUD_COVER, 0.95, rain);
  float edge = 1.0 - cover;
  float soft = DC_CLOUD_SOFTNESS;
  float density = smoothstep(edge, edge + soft, f);
  float densityL = smoothstep(edge, edge + soft, fl);

  // thick middle of a cloud, away from its edge
  float core = smoothstep(edge + soft, edge + soft + 0.35, f);

  // lit side: 1 where the light side is thinner than this spot (open to the light)
  float lit = clamp(0.60 + (density - densityL)*2.2 - core*0.55, 0.0, 1.0);

  // colors, linear and before color correction
  vec3 litCol = mix(DC_CLOUD_NIGHT_LIT_COL, DC_CLOUD_LIT_COL, 1.0 - nightFactor);
  vec3 shadeCol = mix(DC_CLOUD_NIGHT_SHADE_COL, DC_CLOUD_SHADE_COL, 1.0 - nightFactor);
  vec3 col = mix(shadeCol, litCol, lit);

  // thin edges towards the light shine through
  float toward = max(dot(dir, lightDir), 0.0);
  float silver = pow(toward, 6.0)*(1.0 - core)*density*DC_CLOUD_SILVER;
  col += litCol*silver*(1.0 - nightFactor);

  // rain clouds are flat and grey
  float grey = dot(col, vec3(0.33, 0.33, 0.33));
  col = mix(col, vec3_splat(grey)*DC_CLOUD_RAIN_DARK, rain*0.8);

  return vec4(col, density*horizonFade);
}

#endif
