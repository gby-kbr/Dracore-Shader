#ifndef DC_WET_H
#define DC_WET_H

#include "fog.h"

// Wet ground. In the rain every surface the sky can reach is covered, not only patches: it gets darker and
// richer in color, and mirrors the sky like a thin film of water. The film is weak looking straight down and
// strong at a low angle, but never zero, so the ground under the player and slopes show it too.
// Everything is closed form, the only extra cost is one sky lookup per pixel and only while it rains.

// One ring of a rain drop per cell: [0.0, 1.0] where the ring is. p: world xz in blocks
float dcRainRing(vec2 p, float t) {
  vec2 q = p*3.0;
  vec2 cell = floor(q);
  float h = dcHash(cell);
  float phase = fract(t*0.8 + h*7.0);
  vec2 centre = vec2_splat(0.5) + 0.3*vec2(dcHash(cell + 11.0) - 0.5, dcHash(cell + 23.0) - 0.5);
  float d = length(fract(q) - centre);
  float ring = smoothstep(0.07, 0.0, abs(d - phase*0.5))*(1.0 - phase)*(1.0 - phase);
  return ring*step(0.5, h); // not every cell has a drop
}

// col: lit color of the surface, gPos: world position, normal: of the face, viewDir: from the camera to the
// surface, wet: [0.0, 1.0] from the vertex shader (rain times open sky), skyLight: [0.0, 1.0]
vec3 dcWetSurface(vec3 col, vec3 gPos, vec3 normal, vec3 viewDir, float wet, float skyLight, float camDist, float t, dc_skycolor sky) {
  float up = smoothstep(0.3, 0.9, normal.y); // slopes count too, a wall is only a little wet
  float cover = wet*mix(DC_WET_SIDES, 1.0, up);

  // wet things are darker and a little more saturated
  float lum = dot(col, vec3(0.299, 0.587, 0.114));
  col = mix(vec3_splat(lum), col, 1.0 + 0.3*cover)*(1.0 - DC_WET_DARKEN*cover);

  if (up < 0.01) {
    return col;
  }

  // drops hitting the film, near the camera only
  float ring = dcRainRing(gPos.xz, t) + 0.6*dcRainRing(gPos.xz*1.7 + 7.3, t*1.3);
  ring *= DC_WET_RIPPLES*wet*(1.0 - smoothstep(12.0, 30.0, camDist));

  // the sky in the mirror direction, bent a little by the rings
  vec3 r = reflect(viewDir, normal);
  r = normalize(vec3(r.x, abs(r.y)*(1.0 - 0.5*ring) + 0.04, r.z));
  vec3 mirror = dcRenderSky(sky, r)*(0.2 + 0.8*skyLight*skyLight);
  float cosR = clamp(-dot(viewDir, normal), 0.0, 1.0);
  float refl = DC_WET_REFLECTION*(0.35 + 0.65*dcFresnel(cosR, 0.04))*up*wet;
  return mix(col, mirror, refl) + ring*vec3(0.20, 0.22, 0.28)*skyLight*up;
}

#endif
