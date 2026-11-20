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

// Clouds (cumulus, one layer in the sky)
#define DC_CLOUD_COVER          0.52  // 0.0 clear ~ 1.0 overcast
#define DC_CLOUD_SCALE          0.2   // 0.2 huge clouds ~ 2.0 small clouds
#define DC_CLOUD_PUFFINESS      0.5  // 0.0 broad smooth masses ~ 1.0 round cauliflower puffs
#define DC_CLOUD_SOFTNESS       0.4  // 0.05 hard edges ~ 0.4 fluffy edges
#define DC_CLOUD_ROUGHNESS      0.26  // 0.0 smooth ~ 0.8 ragged, torn edges
#define DC_CLOUD_DETAIL_SCALE   5.0   // 1.0 coarse ~ 5.0 fine detail
#define DC_CLOUD_WIND           0.05 // 0.0 still ~ 0.05 fast
#define DC_CLOUD_DEPTH          0.01 // 0.01 flat ~ 0.1 deep, how far the self shadow reaches
#define DC_CLOUD_DEEP_SHADE     0.4  // 0.4 dark undersides ~ 1.0 same as the shade color
#define DC_CLOUD_LIT_COL        vec3(3.5, 3.5, 3.5)    // side facing the sun
#define DC_CLOUD_SHADE_COL      vec3(3.5, 3.5, 3.5) // soft grey-blue underside
#define DC_CLOUD_HORIZON_TINT   0.6   // 0.0 own colors ~ 1.0 fully the color of the horizon
#define DC_CLOUD_NIGHT_LIT_COL  vec3(0.05, 0.07, 0.14)
#define DC_CLOUD_NIGHT_SHADE_COL vec3(0.022, 0.034, 0.08)
#define DC_CLOUD_RAIN_DARK      0.1   // 0.3 stormy dark ~ 1.0 bright grey

// Water (pure clear water with thin waves)
#define DC_WATER_TRANSPARENCY   1.0   // 0.0 invisible ~ 1.0 vanilla opacity
#define DC_WATER_COLOR          vec3(0.0, 0.0, 0.0) // tint of the water, white = no tint
#define DC_WATER_REFLECTION     1.5   // 0.0 none ~ 1.5 strong mirror
#define DC_WATER_FRESNEL        0.0   // 0.0 clear ~ 0.15 reflective when looked at straight
#define DC_WATER_BUMP           0.01  // 0.01 flat ~ 0.2 choppy
#define DC_WATER_WAVE_SCALE     0.3   // 0.3 long waves ~ 3.0 tiny ripples
#define DC_WATER_WAVE_SPEED     0.8   // 0.2 calm ~ 2.0 turbulent
#define DC_WATER_TEX_OPACITY    0.0   // 0.0 plain water ~ 1.0 vanilla water texture

// Sun and moon in the water, like Bedroalism: the reflected sky shows the sun and the moon discs,
// the waves break them up
#define DC_WATER_SUN_GLITTER    1.0   // 0.0 no sun and moon in the water ~ 3.0 blinding

// Under the water
#define DC_UNDERWATER_BRIGHTNESS     0.8 // 0.0 dark ~ 3.0 bright
#define DC_CAUSTIC_INTENSITY         2.0 // 0.0 none ~ 5.0 bright, light patterns on the ground
#define DC_UNDERWATER_SKY_VISIBILITY 1.0 // 0.0 no sky ~ 1.0 sun and clouds seen from under the water
#define DC_UNDERWATER_SKY_WOBBLE     0.04 // 0.0 still ~ 0.15 wavy view of the sky

// Fog, overworld only
#define DC_FOG_BASE_HEIGHT   63.0  // height in blocks where the fog is thickest (sea level)
#define DC_FOG_DENSITY       0.003 // 0.0015 thin ~ 0.012 thick, light lost per block at the base height
#define DC_FOG_FALLOFF       0.02  // 0.008 deep layer ~ 0.05 thin ground fog, thinning with height
#define DC_FOG_RAIN          6.0   // 0.0 none ~ 12.0 very dense, extra fog in rain
#define DC_FOG_RAIN_DARKEN   0.25  // 0.0 sky colored ~ 0.6 dark grey, how dark the fog gets in rain
#define DC_MIST_DENSITY      0.003 // 0.0 none ~ 0.008 thick, low mist banks over water and ground
#define DC_MIST_FALLOFF      0.08  // 0.05 deep layer ~ 0.25 thin layer hugging the ground
#define DC_MIST_COVER        0.45  // 0.1 few patches ~ 0.9 almost everywhere
#define DC_MIST_SCALE        0.03  // 0.01 huge banks ~ 0.1 small patches
#define DC_MIST_WIND         2.5   // 0.0 still ~ 10.0 fast, how fast the banks drift
#define DC_MIST_RAIN         1.0   // 0.0 none ~ 4.0 dense, extra mist in rain
#define DC_MIST_BRIGHTNESS   0.6   // 0.0 same as the sky ~ 1.5 whiter, brightness of the mist by day
#define DC_MIST_SUNLIGHT     0.6   // 0.0 none ~ 2.0 strong, the glow of the sun in the mist

// Wind, plants and lanterns
#define DC_PLANT_WAVE    0.05  // 0.02 gentle ~ 0.2 strong, how far grass, leaves and vines lean
#define DC_LANTERN_WAVE  0.16  // 0.05 subtle ~ 0.4 large, how much hanging lanterns swing
#define DC_WAVE_SPEED    2.0   // 0.5 slow ~ 5.0 fast, speed of the wind
#define DC_WAVE_RAIN     0.6   // 0.0 same ~ 2.0 stormy, how much stronger the wind is in rain
#define DC_WAVE_RANGE    16.0  // 8.0 near ~ 48.0 far, blocks around the camera that move
#define DC_WIND_DIR      vec2(0.912, 0.410) // direction of the wind, the same as the drift of the mist

// The End
#define DC_END_FOG_COL     vec3(0.518, 0.518, 0.667) // must match fog_color #8484AA in mcpack/fogs/the_end_fog_setting.json, it also detects the End (keep red equal to green)
#define DC_END_SKY_BRIGHTNESS 1.0 // 0.5 dark ~ 2.0 bright, the End sky panorama
#define DC_END_AMBIENT     1.0   // 0.8 dark ~ 2.2 bright, the light on the islands of the End
#define DC_END_AMBIENT_COL vec3(0.88, 0.90, 1.10) // soft blue, the color of the End sky and fog

// Sun and moon path (degrees)
#define DC_SUN_PATH_YAW    15.0
#define DC_MOON_PATH_YAW   17.0
#define DC_SUN_PATH_TILT   31.0
#define DC_MOON_PATH_TILT -28.0

#endif
