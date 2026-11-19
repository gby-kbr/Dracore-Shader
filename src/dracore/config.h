#ifndef DC_CONFIG_H
#define DC_CONFIG_H

/*
  DRACORE SHADER CONFIG
  Global options for the shader. Values must be decimal (32.0, not 32).
*/

// Color correction
#define DC_GAMMA 1.33     // 0.3 low ~ 2.0 high
#define DC_EXPOSURE 1.3   // [toggle] 0.5 dark ~ 3.0 bright

// Sun and moon path (degrees)
#define DC_SUN_PATH_YAW    15.0
#define DC_MOON_PATH_YAW   17.0
#define DC_SUN_PATH_TILT   31.0
#define DC_MOON_PATH_TILT -28.0

#endif
