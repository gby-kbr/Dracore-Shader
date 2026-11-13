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

/* Color correction */
#define NL_TONEMAP_TYPE 3              // 1:Exponential, 2:Reinhard, 3:Extended Reinhard, 4:ACES
#define NL_GAMMA 1.33                  // 0.3 low ~ 2.0 high
//#define NL_EXPOSURE 1.3              // [toggle] 0.5 dark ~ 3.0 bright
//#define NL_SATURATION 1.4            // [toggle] 0.0 grayscale ~ 4.0 super saturated
//#define NL_TINT                      // [toggle] enable light/dark tone tinting
#define NL_TINT_LOW  vec3(0.3,0.5,1.4) // color tint for dark tone
#define NL_TINT_HIGH vec3(1.4,0.7,0.3) // color tint for light tone

/* Sky (Overworld only, no End sky) */
#define NL_SKY_VOID_FACTOR     0.5
#define NL_SKY_VOID_DARKNESS   0.3
#define NL_SKY_RAIN_MIX_FACTOR 0.9

/* Sky colors - zenith=top, horizon=bottom */
#define NL_DAWN_ZENITH_COL   vec3(0.1,0.4,0.7)
#define NL_DAWN_HORIZON_COL  vec3(3.0,0.4,0.4)
#define NL_DAWN_EDGE_COL     vec3(2.0,0.8,0.8)
#define NL_DAY_ZENITH_COL    vec3(0.3,0.9,2.0)
#define NL_DAY_HORIZON_COL   vec3(1.0,1.6,1.8)
#define NL_DAY_EDGE_COL      vec3(1.44,1.56,1.62)
#define NL_NIGHT_ZENITH_COL  vec3(0.008,0.048,0.08)
#define NL_NIGHT_HORIZON_COL vec3(0.02,0.06,0.1)
#define NL_NIGHT_EDGE_COL    vec3(0.04,0.08,0.1)
#define NL_RAIN_ZENITH_COL   vec3(0.47,0.51,0.56)
#define NL_RAIN_HORIZON_COL  vec3(0.6,0.6,0.6)

/* Rainbow */
#define NL_RAINBOW           // [toggle] enable rainbow in sky
#define NL_RAINBOW_CLEAR 0.0 // 0.3 subtle ~ 1.0 bright during clear weather
#define NL_RAINBOW_RAIN  0.4 // 0.3 subtle ~ 1.0 bright during rain weather

/* Underwater */
#define NL_UNDERWATER_TINT vec3(0.9,1.0,0.9) // fog tint color when underwater


#endif
