#ifndef DC_WAVE_H
#define DC_WAVE_H

// Swaying plants and swinging lanterns. Everything is worked out per vertex, only for
// the vertices near the camera, with a few sines and no textures or noise.

// Smooth wind at a position, about [-1, 1]: a swell rolling across the world, bent a
// little in frequency, times a slow gust. Every vertex reads the wind at its own
// position, so neighbouring blocks never tear apart.
float dcWind(vec3 gPos, float t, float rain) {
  float a = t*DC_WAVE_SPEED + dot(gPos, vec3(0.13, 0.05, 0.07));
  float swell = sin(a + 0.8*sin(0.37*a));
  float gust = 0.5 + 0.5*sin(0.4*t + dot(gPos.xz, vec2(0.02, 0.015)));
  return swell*(0.4 + 0.6*gust)*(1.0 + DC_WAVE_RAIN*rain);
}

// A plant leans downwind. weight: 0.0 at the roots, 1.0 at the tips. fade: 0.0 ~ 1.0
// with the distance, skyLight: 0.0 ~ 1.0, plants in a cave stand still
void dcPlantSway(inout vec3 worldPos, vec3 gPos, float weight, float fade, float skyLight, float t, float rain) {
  float w = DC_PLANT_WAVE*weight*fade*skyLight*(1.0 + dcWind(gPos, t, rain));
  worldPos.xz += DC_WIND_DIR*w;
  worldPos.y -= 0.5*w*w; // the length of the stem stays the same
}

// A lantern swings from the top of its chain. depth: how far below the top of the
// block the vertex is (0.0 ~ 1.0), block: one value for the whole block, so the
// lantern and its chain move as one piece
void dcLanternSwing(inout vec3 worldPos, vec3 block, float depth, float fade, float t, float rain) {
  vec2 swing = DC_LANTERN_WAVE*fade*vec2(dcWind(block, t, rain), dcWind(block.zyx + 7.0, 1.3*t, rain));
  worldPos.xz += swing*depth;
  worldPos.y -= 0.5*dot(swing, swing)*depth; // it swings on its chain, so it also rises a little
}

// Which vertices are plants, from the shape of the vanilla models (the block-local
// position bPos and the position inside the texture tile).
// color: vertex color, texPosY: 0.0 at the top of the texture tile, 1.0 at its bottom
void dcWavePlants(inout vec3 worldPos, vec3 gPos, vec3 bPos, vec4 color, float texPosY, float skyLight, float camDist, bool isTree, float t, float rain) {
  bool isColored = color.r != color.g || color.r != color.b;
  bool isTop = texPosY < 0.5;
  bool isPlants = color.r/color.g < 1.9;
  vec2 bPosC = abs(bPos.xz - 0.5);
  bool isVines = (bPosC.x == 0.453125 && bPos.z == 0.0) || (bPosC.y == 0.453125 && bPos.x == 0.0);
  bool isFarmPlant = bPos.y == 0.9375 && (bPosC.x == 0.25 || bPosC.y == 0.25);
  bool isLeafLitter = bPos.y == 0.015625 && (fract(bPos.x*2.0) + fract(bPos.z*2.0)) == 0.0;
  bool waves = ((isTree || isPlants || isVines) && isColored && !isLeafLitter) || (isFarmPlant && isTop);
  if (!waves) {
    return;
  }

  float weight = isTree ? 0.5 : 1.0;
  if (isVines) {
    weight = fract(0.01 + 0.5*gPos.y);
  } else if (isPlants && isColored && !isTree && !isTop) {
    weight = 0.0; // the roots stay in the ground
  }

  float fade = 1.0 - smoothstep(0.5, 1.0, camDist/DC_WAVE_RANGE);
  dcPlantSway(worldPos, gPos, weight, fade, skyLight, t, rain);
}

// Hanging lanterns and their chains. Only the ones that give light move, and
// only the hanging ones (the tests are the vanilla models).
void dcWaveLantern(inout vec3 worldPos, vec3 gPos, vec3 bPos, float texPosY, float torchLight, float camDist, float t, float rain) {
  vec2 bPosC = abs(bPos.xz - 0.5);
  bool y6875 = bPos.y == 0.6875;
  bool y5625 = bPos.y == 0.5625;
  bool isLantern = ((y6875 || y5625) && bPosC.x == 0.125) || ((y5625 || bPos.y == 0.125) && bPosC.x == 0.1875);
  bool isChain = bPosC.x == 0.0625 && y6875;
  if (y5625 && (texPosY < 0.001 || (texPosY > 0.43 && texPosY < 0.93))) {
    isLantern = false; // the top of a lantern that stands on the ground
  }
  if (torchLight > 0.6 && (isChain || isLantern)) {
    vec3 block = floor(gPos + 0.5 - bPos + 0.01); // one value for the whole block
    float fade = 1.0 - smoothstep(0.5, 1.0, camDist/DC_WAVE_RANGE);
    dcLanternSwing(worldPos, block, 1.0 - bPos.y, fade, t, rain);
  }
}

#endif
