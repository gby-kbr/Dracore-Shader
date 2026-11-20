#ifndef DC_ENDSKY_H
#define DC_ENDSKY_H

// The End sky is a finished 360 degree panorama (textures/environment/end_sky.png, 4032x2016),
// looked up by view direction instead of the vanilla cube-face UVs.
// dir: normalized view direction, y up
vec2 dcEndSkyUV(vec3 dir) {
  float lon = atan2(dir.x, dir.z);               // -pi..pi, 0 = +z
  float lat = asin(clamp(dir.y, -1.0, 1.0));     // -pi/2..pi/2
  return vec2(lon*0.15915494 + 0.5, 0.5 - lat*0.31830989);
}

// The panorama is a finished picture, so it does not go through the general color
// correction, which would flatten it. This curve gives it the same look as in
// Bedroalism (a fit of that pipeline, error under 0.01 on 0..1): soft shoulder, darks
// kept dark.
vec3 dcEndSkyColor(vec3 tex) {
  vec3 x = max(tex*DC_END_SKY_BRIGHTNESS, vec3_splat(0.0));
  return vec3_splat(1.0) - exp(-2.9*pow(x, vec3_splat(1.32)));
}

#endif
