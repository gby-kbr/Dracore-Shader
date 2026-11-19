#ifndef DC_CONFIG_H
#define DC_CONFIG_H

/*
  DRACORE SHADER CONFIG
  Global options for the shader. Values must be decimal (32.0, not 32).
*/

// Color correction
#define DC_GAMMA 1.33     // 0.3 low ~ 2.0 high
#define DC_EXPOSURE 1.3   // [toggle] 0.5 dark ~ 3.0 bright

// Sky colors, zenith and horizon only (linear, before color correction)
#define DC_DAY_ZENITH_COL     vec3(0.3, 0.9, 2.0)
#define DC_DAY_HORIZON_COL    vec3(1.0, 1.6, 1.8)
// Dawn has no colored sky of its own: a muted lavender sky, and the pale warm
// light of the sun that only shows on the horizon around it
#define DC_DAWN_ZENITH_COL    vec3(1.77, 1.77, 2.86)
#define DC_DAWN_HORIZON_COL   vec3(1.38, 1.30, 1.79)
#define DC_DAWN_SUNLIGHT_COL  vec3(5.2, 4.2, 1.5)
#define DC_NIGHT_ZENITH_COL   vec3(0.008, 0.048, 0.08)
#define DC_NIGHT_HORIZON_COL  vec3(0.02, 0.06, 0.1)
#define DC_RAIN_ZENITH_COL    vec3(0.47, 0.51, 0.56)
#define DC_RAIN_HORIZON_COL   vec3(0.6, 0.6, 0.6)
#define DC_SKY_RAIN_MIX       0.9    // 0.0 no change ~ 1.0 fully overcast in rain
#define DC_UNDERWATER_TINT    vec3(0.9, 1.0, 0.9) // sky tint when underwater

// Sun and moon
#define DC_SUN_SIZE       1.0   // 0.3 tiny ~ 4.0 massive
#define DC_MOON_SIZE      1.0   // 0.3 tiny ~ 4.0 massive
#define DC_SUNMOON_GLOW   1.0   // 0.0 none ~ 2.0 strong, the soft glow around the sun and the moon

// Sun and moon path (degrees)
#define DC_SUN_PATH_YAW    15.0
#define DC_MOON_PATH_YAW   17.0
#define DC_SUN_PATH_TILT   31.0
#define DC_MOON_PATH_TILT -28.0

#endif
