#ifndef INSTANCING
  $input v_worldPos, v_underwaterRainTimeDay
#endif

#include <bgfx_shader.sh>

#ifndef INSTANCING
  #include <dracore/main.sh>

  uniform vec4 TimeOfDay;
  uniform vec4 FogColor;
  uniform vec4 FogAndDistanceControl;

  SAMPLER2D_AUTOREG(s_DcCloudNoise);
#endif

void main() {
  #ifndef INSTANCING
    vec3 viewDir = normalize(v_worldPos);

    dc_environment env;
    env.end = false;
    env.nether = false;
    env.underwater = v_underwaterRainTimeDay.x > 0.5;
    env.rainFactor = v_underwaterRainTimeDay.y;
    env.dayFactor = v_underwaterRainTimeDay.w;
    env.fogCol = FogColor.rgb;
    env = calculateSunParams(env, TimeOfDay.x);

    dc_skycolor skycol = dcOverworldSkyColors(env);

    vec3 skyColor = dcRenderSky(skycol, viewDir);

    // the clouds, also seen from under the water: the sky is the same there
    vec4 clouds = dcClouds(s_DcCloudNoise, viewDir, env, skycol, v_underwaterRainTimeDay.z);
    skyColor = mix(skyColor, clouds.rgb, clouds.a);

    skyColor = colorCorrection(skyColor);

    gl_FragColor = vec4(skyColor, 1.0);
  #else
    gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
  #endif
}
