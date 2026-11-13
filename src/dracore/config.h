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

/* Color correction: exposure, Reinhard-Jodie tonemap, gamma */
#define NL_EXPOSURE 1.3                // 0.5 dark ~ 3.0 bright
#define NL_GAMMA 1.33                  // 0.3 low ~ 2.0 high

/* Sky (Overworld only, no End sky) */
#define NL_SKY_RAIN_MIX_FACTOR 0.9

/* Sky colors - zenith=top, horizon=bottom */
#define NL_DAWN_ZENITH_COL   vec3(0.1,0.4,0.7)
#define NL_DAWN_HORIZON_COL  vec3(3.0,0.4,0.4)
#define NL_DAY_ZENITH_COL    vec3(0.3,0.9,2.0)
#define NL_DAY_HORIZON_COL   vec3(1.0,1.6,1.8)
#define NL_NIGHT_ZENITH_COL  vec3(0.008,0.048,0.08)
#define NL_NIGHT_HORIZON_COL vec3(0.02,0.06,0.1)
#define NL_RAIN_ZENITH_COL   vec3(0.47,0.51,0.56)
#define NL_RAIN_HORIZON_COL  vec3(0.6,0.6,0.6)

/* Underwater */
#define NL_UNDERWATER_TINT vec3(0.9,1.0,0.9) // fog tint color when underwater

/* Torch (block light) colors */
#define NL_TORCHLIGHT_INTENSITY 1.0  // 0.5 weak ~ 3.0 bright
#define NL_OVERWORLD_TORCH_COL  vec3(1.0,0.52,0.18)
#define NL_UNDERWATER_TORCH_COL vec3(1.0,0.52,0.18)
#define NL_NETHER_TORCH_COL     vec3(1.0,0.52,0.18)
#define NL_END_TORCH_COL        vec3(1.0,0.52,0.18)

/* Fog, overworld only */
#define NL_FOG_DENSITY 0.003  // 0.0015 thin ~ 0.012 thick, light lost per block at sea level
#define NL_FOG_FALLOFF 0.02   // 0.008 deep layer ~ 0.05 thin ground mist, thinning with height
#define NL_FOG_RAIN    2.0    // 0.0 none ~ 5.0 dense, extra fog in rain
#define NL_FOG_DAWN    1.5    // 0.0 none ~ 4.0 dense, extra mist at sunrise and sunset
#define NL_GROUND_MIST 0.002  // 0.0 none ~ 0.006 thick, low patchy mist over water and ground
#define NL_GROUND_MIST_FALLOFF 0.08 // 0.05 deep layer ~ 0.25 thin layer hugging the ground

#endif
