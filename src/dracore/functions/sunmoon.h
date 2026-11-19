#ifndef DC_SUNMOON_H
#define DC_SUNMOON_H

#include "utils.h"

// The sun and moon quads are placed here instead of by the game, so they
// follow the same tilted path as dcCalculateSunParams. The quad is 70 units
// wide at a distance of 300, as in Newb.
#define DC_SUNMOON_DIST  300.0
#define DC_SUNMOON_SCALE 70.0

// position: the quad vertex of the game, xz in [-1, 1]
// angle: time of day * 2 pi
vec3 dcSunMoonPosition(vec3 position, float angle, bool isSun) {
  float size = isSun ? DC_SUN_SIZE : DC_MOON_SIZE;
  float tilt = dcDegToRad(isSun ? DC_SUN_PATH_TILT : DC_MOON_PATH_TILT);
  float yaw = dcDegToRad(isSun ? DC_SUN_PATH_YAW : DC_MOON_PATH_YAW);

  // same circle as the light direction: the sun at (-sin, cos), the moon on the other side
  vec2 along = vec2(cos(angle), sin(angle)); // tangent of the circle, the quad's x axis
  vec2 circle = isSun ? vec2(-sin(angle), cos(angle)) : vec2(sin(angle), -cos(angle));
  float x = isSun ? position.x : -position.x;

  vec3 p = vec3(circle*DC_SUNMOON_DIST + along*(x*DC_SUNMOON_SCALE*size),
                position.z*DC_SUNMOON_SCALE*size);
  p.yz = mul(dcRmat2(-tilt), p.yz);
  p.xz = mul(dcRmat2(yaw), p.xz);
  return p;
}

#endif
