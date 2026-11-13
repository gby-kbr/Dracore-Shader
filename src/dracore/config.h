#ifndef DC_CONFIG_H
#define DC_CONFIG_H

// Celestial path settings used by calculateSunParams() in functions/detection.h.
// Values match newb-x-mcbe defaults.

#ifndef NL_SUN_PATH_YAW
  #define NL_SUN_PATH_YAW    15.0
#endif
#ifndef NL_MOON_PATH_YAW
  #define NL_MOON_PATH_YAW   17.0
#endif
#ifndef NL_SUN_PATH_TILT
  #define NL_SUN_PATH_TILT   31.0
#endif
#ifndef NL_MOON_PATH_TILT
  #define NL_MOON_PATH_TILT -28.0
#endif

#endif
