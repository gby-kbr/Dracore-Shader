#ifndef DC_DETECTION_H
#define DC_DETECTION_H

#include "utils.h"

struct dc_environment {
  bool end;
  bool nether;
  bool underwater;
  float rainFactor; // [0.0, 1.0]
  float dayFactor;  // [-1.0, 1.0]
  vec3 sunDir;      // normalized
  vec3 moonDir;     // normalized
  vec3 fogCol;      // [0.0, 1.0]
};

bool dcDetectEnd(vec3 FOG_COLOR) {
  // the End fog is set in fogs/the_end_fog_setting.json to one exact color that the overworld
  // never has (DC_END_FOG_COL)
  vec3 d = abs(FOG_COLOR - DC_END_FOG_COL);
  return max(max(d.r, d.g), d.b) < 0.006;
}

bool dcDetectNether(vec3 FOG_COLOR, vec2 FOG_CONTROL) {
  // FOG_CONTROL.x depends on render distance, range roughly (0.03, 0.14)
  float expectedFogX = 0.029 + (0.09*FOG_CONTROL.y*FOG_CONTROL.y);

  // nether wastes, basalt deltas, crimson forest, warped forest, soul sand valley
  bool netherFogCtrl = (FOG_CONTROL.x<0.14 && abs(FOG_CONTROL.x-expectedFogX) < 0.02);
  bool netherFogCol = (FOG_COLOR.r+FOG_COLOR.g)>0.0;

  // underlava counts as nether
  bool underLava = FOG_CONTROL.x == 0.0 && FOG_COLOR.b == 0.0 && FOG_COLOR.g < 0.18 && FOG_COLOR.r-FOG_COLOR.g > 0.1;

  return (netherFogCtrl && netherFogCol) || underLava;
}

bool dcDetectUnderwater(vec3 FOG_COLOR, vec2 FOG_CONTROL) {
  return FOG_CONTROL.x==0.0 && FOG_CONTROL.y<0.8 && (FOG_COLOR.b>FOG_COLOR.r || FOG_COLOR.g>FOG_COLOR.r);
}

float dcDetectRain(vec3 FOG_CONTROL) {
  // FOG_CONTROL.z is the render distance in blocks (chunks * 16);
  // the clear-weather FOG_CONTROL.x value depends on it
  vec2 clear = vec2(0.5 + 20.0/FOG_CONTROL.z, 1.0);
  vec2 rain = vec2(0.23, 0.70);
  vec2 factor = clamp((FOG_CONTROL.xy-clear)/(rain-clear), vec2(0.0,0.0), vec2(1.0,1.0));
  float val = factor.x*factor.y;
  return val*val*(3.0 - 2.0*val);
}

dc_environment dcCalculateSunParams(dc_environment env, float TIME_OF_DAY) {
  float t = 2.0 * DC_PI * TIME_OF_DAY;
  // The game's own sun rises in the east (+x). Newb mirrors this because its
  // sun quad is mirrored; Dracore keeps the game's direction.
  vec3 sunDir = vec3(-sin(t), cos(t), 0.0);
  vec3 moonDir = vec3(-sunDir.x, -sunDir.y, 0.0);
  env.dayFactor = sunDir.y;

  sunDir.yz = mul(dcRmat2(-dcDegToRad(DC_SUN_PATH_TILT)), sunDir.yz);
  sunDir.xz = mul(dcRmat2(dcDegToRad(DC_SUN_PATH_YAW)), sunDir.xz);

  moonDir.yz = mul(dcRmat2(-dcDegToRad(DC_MOON_PATH_TILT)), moonDir.yz);
  moonDir.xz = mul(dcRmat2(dcDegToRad(DC_MOON_PATH_YAW)), moonDir.xz);

  env.sunDir = sunDir;
  env.moonDir = moonDir;

  return env;
}

dc_environment dcDetectEnvironment(float TIME_OF_DAY, vec3 FOG_COLOR, vec3 FOG_CONTROL) {
  dc_environment env;
  env.end = dcDetectEnd(FOG_COLOR);
  env.nether = dcDetectNether(FOG_COLOR, FOG_CONTROL.xy);
  env.underwater = dcDetectUnderwater(FOG_COLOR, FOG_CONTROL.xy);
  env.rainFactor = dcDetectRain(FOG_CONTROL.xyz);
  env.fogCol = FOG_COLOR;
  env = dcCalculateSunParams(env, TIME_OF_DAY);
  return env;
}

// Only what the sky color needs, without the sun and moon directions. For the pixel shader, where
// working out the sun path for every pixel would be a waste.
dc_environment dcDetectEnvironmentLite(float TIME_OF_DAY, vec3 FOG_COLOR, vec3 FOG_CONTROL) {
  dc_environment env;
  env.end = dcDetectEnd(FOG_COLOR);
  env.nether = dcDetectNether(FOG_COLOR, FOG_CONTROL.xy);
  env.underwater = dcDetectUnderwater(FOG_COLOR, FOG_CONTROL.xy);
  env.rainFactor = dcDetectRain(FOG_CONTROL.xyz);
  env.fogCol = FOG_COLOR;
  env.dayFactor = cos(2.0*DC_PI*TIME_OF_DAY);
  env.sunDir = vec3_splat(0.0);
  env.moonDir = vec3_splat(0.0);
  return env;
}

// An environment with nothing set, for variables that are filled in only on some paths
dc_environment dcEmptyEnvironment() {
  dc_environment env;
  env.end = false;
  env.nether = false;
  env.underwater = false;
  env.rainFactor = 0.0;
  env.dayFactor = 0.0;
  env.sunDir = vec3_splat(0.0);
  env.moonDir = vec3_splat(0.0);
  env.fogCol = vec3_splat(0.0);
  return env;
}

#endif
