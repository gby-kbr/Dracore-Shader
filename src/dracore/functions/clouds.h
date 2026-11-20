#ifndef DC_CLOUDS_H
#define DC_CLOUDS_H

#include "sky.h"

// Cumulus clouds: one flat layer drawn in the sky pass. Three reads of one
// small noise texture (see script/generate_cloud_noise.py):
//   R coverage, G puffs, B fine detail.
// Puffy lighting comes from comparing the cloud density with the density a
// little closer to the light: the side facing the light is bright, the side
// facing away is a soft grey-blue.

// density of the cloud at one spot: big shapes from the coverage and puff
// channels, then eaten away by two layers of detail so the edges are ragged
// and billowy instead of smooth blobs
float dcCloudDensityField(vec4 n, vec4 d, vec4 d2) {
  float base = mix(n.r, n.g, DC_CLOUD_PUFFINESS);
  return base + (d.g - 0.5)*DC_CLOUD_ROUGHNESS + (d2.g - 0.5)*DC_CLOUD_ROUGHNESS*0.5;
}

// returns rgb (before color correction) and coverage in alpha.
// The layer lies at a fixed height in the world (DC_CLOUD_HEIGHT), so it stays where it is in the sky when the
// camera moves: flying towards the clouds brings them closer, they do not run away upwards. The clouds are lit
// evenly, so there is one color and the cost is three reads of the noise texture.
// camPos: camera position in the world. skyGlow: the glow of the sun or moon in this direction, it tints the clouds near it
vec4 dcClouds(sampler2D noiseTex, vec3 dir, dc_environment env, dc_skycolor skyColors, vec3 camPos, vec3 skyGlow, float time) {
  float up = dir.y;
  float horizonFade = smoothstep(0.0, 0.22, up);
  if (horizonFade <= 0.0) return vec4(0.0, 0.0, 0.0, 0.0);

  // where the ray meets the flat layer, in blocks, then in the units of the noise (128 blocks is one unit, so the
  // clouds look the same as before for a camera 128 blocks below the layer). The farther from the zenith, the
  // more squeezed the clouds look
  // never closer than 128 blocks: flying up to the layer would blow the clouds up and make them look frozen overhead
  float toLayer = max(DC_CLOUD_HEIGHT - camPos.y, 128.0);
  vec2 uv = (camPos.xz + dir.xz*toLayer/(up + 0.12) + vec2(1.0, 0.35)*time*DC_CLOUD_WIND)*(1.0/128.0);
  vec2 p = uv*DC_CLOUD_SCALE;

  float rain = (env.underwater || env.nether || env.end) ? 0.0 : env.rainFactor;
  float nightFactor = 1.0 - smoothstep(-0.12, 0.12, env.dayFactor);

  // detail follows the big shape, so it bends with it
  vec2 q = p*DC_CLOUD_DETAIL_SCALE;
  vec4 shape = texture2D(noiseTex, p);
  vec4 detail = texture2D(noiseTex, q + shape.rg*0.12);
  vec4 detail2 = texture2D(noiseTex, q*2.3 + shape.gb*0.12);

  float cover = mix(DC_CLOUD_COVER, 0.95, rain);
  float edge = 1.0 - cover;
  float density = smoothstep(edge, edge + DC_CLOUD_SOFTNESS, dcCloudDensityField(shape, detail, detail2));

  // color, linear and before color correction. By day the clouds take the color of the
  // horizon with the brightness kept; the horizon already includes rain, so rain clouds turn grey on their own
  vec3 col = mix(DC_CLOUD_NIGHT_LIT_COL, DC_CLOUD_LIT_COL, 1.0 - nightFactor);
  vec3 horizonTint = skyColors.horizon/max(dcLuminance(skyColors.horizon), 0.0001);
  col *= mix(vec3_splat(1.0), horizonTint, DC_CLOUD_HORIZON_TINT*(1.0 - nightFactor));

  // near the sun the clouds take the color of its glow, most when the sun is low
  float lowSun = 1.0 - smoothstep(0.0, 0.5, env.sunDir.y);
  float tint = clamp(dcLuminance(skyGlow)*DC_CLOUD_GLOW_TINT*(0.35 + 0.65*lowSun)*(1.0 - nightFactor), 0.0, 1.0);
  vec3 glowCol = DC_SUNGLOW_COL/max(dcLuminance(DC_SUNGLOW_COL), 0.0001);
  col = mix(col, col*glowCol, tint) + skyGlow*(0.3*DC_CLOUD_GLOW_TINT);

  // rain clouds are flat and grey
  float grey = dot(col, vec3(0.33, 0.33, 0.33));
  col = mix(col, vec3_splat(grey)*DC_CLOUD_RAIN_DARK, rain*0.8);

  return vec4(col, density*horizonFade);
}

#endif
