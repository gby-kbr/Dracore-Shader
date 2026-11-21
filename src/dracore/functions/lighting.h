#ifndef DC_LIGHTING_H
#define DC_LIGHTING_H

#include "utils.h"

// Light of torches and of every block that glows (lava, glowstone, lanterns, sea lanterns...): the game lights
// them all through the block light of the lightmap. The vanilla color of that light is kept and made warmer:
// where the block light dominates over the sky light it is multiplied by a warm color that only shifts the hue
// (the brightness stays). Each place has its own warmth: the overworld, under water, the Nether and the End.
// lightMap: the vanilla lightmap color. blockLight, skyLight: lightmap uv, [0.0, 1.0]
// day: how much sun there is, [0.0, 1.0], place: 0.0 overworld, 1.0 the End, 2.0 the Nether
vec3 dcTorchLight(vec3 lightMap, float blockLight, float skyLight, float day, float place, bool underwater) {
  float att = smoothstep(0.1, 1.0, blockLight);
  att *= att;
  // how much daylight reaches the point: the warmth only shows in the shade, in caves and at night
  float sky = smoothstep(0.3, 0.9, skyLight)*(place < 0.5 ? day + 0.05 : 0.0);
  float share = att/(att + sky + 0.02);

  vec3 torch = DC_TORCH_COL;
  if (place > 1.5) {
    torch = DC_TORCH_NETHER_COL;
  } else if (place > 0.5) {
    torch = DC_TORCH_END_COL;
  } else if (underwater) {
    torch = DC_TORCH_UNDERWATER_COL;
  }
  vec3 warm = torch/max(dot(torch, vec3(0.333, 0.333, 0.333)), 0.001); // same brightness
  return lightMap*mix(vec3_splat(1.0), warm, clamp(share, 0.0, 1.0));
}

// Color of the sunlight on the world: warm at noon, at dawn and dusk the color of the sun glow. Only places open to
// the sky get it, and rain takes it away. sunHeight: sun direction y, skyLight: lightmap uv, rain: [0.0, 1.0]
vec3 dcSunLightTint(float sunHeight, float skyLight, float rain) {
  float open = smoothstep(0.4, 0.95, skyLight);
  float noon = smoothstep(0.25, 0.85, sunHeight)*DC_SUNLIGHT_NOON;
  float dawn = (1.0 - smoothstep(0.05, 0.35, sunHeight))*smoothstep(-0.12, 0.02, sunHeight)*DC_SUNLIGHT_DAWN;
  vec3 noonCol = DC_SUNLIGHT_NOON_COL/max(dcLuminance(DC_SUNLIGHT_NOON_COL), 0.001);
  vec3 dawnCol = DC_SUNGLOW_COL/max(dcLuminance(DC_SUNGLOW_COL), 0.001);
  vec3 tint = mix(vec3_splat(1.0), noonCol, noon);
  tint = mix(tint, dawnCol, dawn);
  return mix(vec3_splat(1.0), tint, open*(1.0 - rain));
}

// Night in the overworld: covered places go dark and the open sky keeps a dim cool moonlight, torch light is
// kept. night: [0.0, 1.0], zero in other places. torch, skyLit: lightmap uv.
vec3 dcNightLight(vec3 col, vec2 lightmapUV, float night) {
  float torch = smoothstep(0.1, 0.8, lightmapUV.x);
  float skyLit = smoothstep(0.3, 0.9, lightmapUV.y);
  float keep = max(torch, DC_MOONLIGHT*skyLit);
  col *= 1.0 - DC_NIGHT_DARKNESS*night*(1.0 - keep);
  // the tint of the moon, only where sky light reaches and no torch lights the surface
  return col*mix(vec3_splat(1.0), DC_MOONLIGHT_COL, night*skyLit*(1.0 - torch));
}

#endif
