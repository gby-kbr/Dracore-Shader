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

// The fog of the End, made to be almost invisible: it has the color of the panorama in the
// direction it is looking, so it melts into the sky instead of standing as a wall, it is very
// thin, and it only closes the edge of the render distance. As  color*fog.a + fog.rgb,
// worked out once per vertex.
// relPos: from the camera to the surface, renderDist: in blocks
vec4 dcEndFog(vec3 relPos, float renderDist) {
  float dist = length(relPos);
  float dirY = relPos.y/max(dist, 0.0001);
  // bright blue haze at the horizon, deep blue above and below
  vec3 lin = DC_END_FOG_HORIZON*exp(-6.0*abs(dirY)) + DC_END_FOG_DEEP;
  vec3 col = dcEndSkyColor(lin);
  float thin = 1.0 - exp(-DC_END_FOG_DENSITY*dist);
  float edge = smoothstep(DC_END_FOG_EDGE, 1.0, dist/max(renderDist, 1.0));
  float amount = max(thin, edge);
  return vec4(col*amount, 1.0 - amount);
}

#endif
