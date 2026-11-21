#ifndef DC_UTILS_H
#define DC_UTILS_H

#define DC_PI       3.141592

// 2D rotation matrix for an angle in radians
mat2 dcRmat2(float t) {
  float sint = sin(t);
  float cost = cos(t);
  return mtxFromRows(vec2(cost, -sint), vec2(sint, cost));
}

float dcDegToRad(float t) { return 0.0174533*t; }

float dcLuminance(vec3 x) { return dot(x, vec3(0.21, 0.71, 0.08)); }

#endif
