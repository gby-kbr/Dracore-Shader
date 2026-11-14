#ifndef DC_CONFIG_H
#define DC_CONFIG_H

/* Sun and moon path, used by calculateSunParams() in functions/detection.h */
#define DC_SUN_PATH_YAW    15.0
#define DC_MOON_PATH_YAW   17.0
#define DC_SUN_PATH_TILT   31.0
#define DC_MOON_PATH_TILT -28.0
#define DC_SUN_SIZE        1.0  // 0.3 tiny ~ 4.0 massive
#define DC_MOON_SIZE       1.0  // 0.3 tiny ~ 4.0 massive
#define DC_SUN_TILT        45.0 // 0.0 no tilt ~ 90.0, how much the sun picture is turned
#define DC_MOON_TILT       45.0

/* Color correction: exposure, Reinhard-Jodie tonemap, gamma */
#define DC_EXPOSURE 1.3                // 0.5 dark ~ 3.0 bright
#define DC_GAMMA 1.33                  // 0.3 low ~ 2.0 high

/* Sky (Overworld only, no End sky) */
#define DC_SKY_RAIN_MIX_FACTOR 1.0

/* Sky colors - zenith=top, horizon=bottom */
#define DC_DAWN_ZENITH_COL   vec3(0.1,0.4,0.7)
#define DC_DAWN_HORIZON_COL  vec3(3.0,0.4,0.4)
#define DC_DAY_ZENITH_COL    vec3(0.3,0.9,2.0)
#define DC_DAY_HORIZON_COL   vec3(1.0,1.6,1.8)
#define DC_NIGHT_ZENITH_COL  vec3(0.008,0.048,0.08)
#define DC_NIGHT_HORIZON_COL vec3(0.02,0.06,0.1)
#define DC_RAIN_ZENITH_COL   vec3(0.47,0.51,0.56)
#define DC_RAIN_HORIZON_COL  vec3(0.6,0.6,0.6)

/* Underwater */
#define DC_UNDERWATER_DENSITY 0.035 // 0.02 very clear ~ 0.12 hazy, light lost per block under water

/* Torch (block light) colors */
#define DC_TORCHLIGHT_INTENSITY 1.0  // 0.5 weak ~ 3.0 bright
#define DC_OVERWORLD_TORCH_COL  vec3(1.0,0.52,0.18)
#define DC_UNDERWATER_TORCH_COL vec3(1.0,0.52,0.18)
#define DC_NETHER_TORCH_COL     vec3(1.0,0.52,0.18)
#define DC_END_TORCH_COL        vec3(1.0,0.52,0.18)

/* Fog, overworld only */
#define DC_FOG_DENSITY 0.003  // 0.0015 thin ~ 0.012 thick, light lost per block at sea level
#define DC_FOG_FALLOFF 0.02   // 0.008 deep layer ~ 0.05 thin ground mist, thinning with height
#define DC_FOG_RAIN    2.0    // 0.0 none ~ 5.0 dense, extra fog in rain
#define DC_FOG_DAWN    1.5    // 0.0 none ~ 4.0 dense, extra mist at sunrise and sunset
#define DC_GROUND_MIST 0.002  // 0.0 none ~ 0.006 thick, low patchy mist over water and ground
#define DC_GROUND_MIST_FALLOFF 0.08 // 0.05 deep layer ~ 0.25 thin layer hugging the ground

/* Water */
#define DC_WATER_COLOR        vec3(0.10,0.37,0.41) // color of the water body in daylight, teal green
#define DC_WATER_OPACITY      0.22  // 0.0 clear ~ 1.0 solid, looking straight down. Same from above and from below
#define DC_WATER_REFLECTIVITY 0.02  // 0.02 realistic ~ 0.1 mirror like, reflection at a steep angle
#define DC_WATER_TEX_OPACITY  0.0   // 0.0 plain water ~ 1.0 vanilla water texture
#define DC_WATER_WAVE_SPEED   0.7   // 0.2 calm ~ 2.0 turbulent
#define DC_WATER_BUMP         0.10  // 0.0 flat ~ 0.2 bumpy, how much the waves tilt the reflection
#define DC_WATER_WAVE         0.04  // 0.0 flat ~ 0.15, how far the waves lower the surface, in blocks
#define DC_WATER_SUN_GLITTER  1.0   // 0.0 none ~ 2.0 strong, reflection of the sun and the moon
#define DC_WATER_SUN_SIZE     0.0583 // radius of the sun reflection, in radians. Set it to the visible sun disc

/* Clouds, see functions/clouds.h. Soft streaks read from textures/environment/cloud_noise.png */
#define DC_CLOUD_COVER    0.48 // 0.0 clear ~ 1.0 overcast, how much of the sky the clouds cover
#define DC_CLOUD_STRETCH  2.0  // 1.0 round ~ 4.0 long streaks, how much the wind pulls the clouds
#define DC_CLOUD_WARP     0.5  // 0.0 straight ~ 2.0 curly, how much the streaks bend
#define DC_CLOUD_SOFTNESS 0.26 // 0.1 sharp ~ 0.7 very soft, how wide the soft edge is
#define DC_CLOUD_FIBER    0.32 // 0.0 smooth ~ 0.4 fine, thin fibers at the edge
#define DC_CLOUD_OPACITY  0.7 // 0.2 thin ~ 1.0 solid, how much the clouds hide the sky
#define DC_CLOUD_WIND     0.5  // 0.0 still ~ 2.0 fast, how fast the clouds drift

#endif
