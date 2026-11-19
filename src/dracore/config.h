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
// Sunrise and sunset have no colors of their own, the day sky blends straight
// into the night sky. Night: a deep navy zenith under a lighter, soft blue horizon
#define DC_NIGHT_ZENITH_COL   vec3(0.0148, 0.0261, 0.0884)
#define DC_NIGHT_HORIZON_COL  vec3(0.0307, 0.0460, 0.1066)
#define DC_RAIN_ZENITH_COL    vec3(0.47, 0.51, 0.56)
#define DC_RAIN_HORIZON_COL   vec3(0.6, 0.6, 0.6)
#define DC_SKY_RAIN_MIX       0.9    // 0.0 no change ~ 1.0 fully overcast in rain
#define DC_UNDERWATER_TINT    vec3(0.9, 1.0, 0.9) // sky tint when underwater

// Sun and moon
#define DC_SUN_SIZE       1.0   // 0.3 tiny ~ 4.0 massive
#define DC_MOON_SIZE      1.0   // 0.3 tiny ~ 4.0 massive
#define DC_SUNMOON_GLOW   1.0   // 0.0 none ~ 2.0 strong, the soft glow around the sun and the moon
#define DC_SUNGLOW_COL    vec3(1.0, 0.90, 0.45) // pale cream light of the sun, the same all day

// Sun and moon path (degrees)
#define DC_SUN_PATH_YAW    15.0
#define DC_MOON_PATH_YAW   17.0
#define DC_SUN_PATH_TILT   31.0
#define DC_MOON_PATH_TILT -28.0

#endif
