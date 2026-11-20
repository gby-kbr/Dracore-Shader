#ifndef INSTANCING
    $input v_worldPos, v_envParams
#endif

#include <bgfx_shader.sh>

#ifndef INSTANCING
    #include <dracore/main.sh>

    SAMPLER2D_AUTOREG(s_DcCloudNoise);

    uniform vec4 TimeOfDay;
    uniform vec4 FogColor;
    uniform vec4 CameraPosition;
#endif

void main() {
#ifndef INSTANCING
    vec3 viewDir = normalize(v_worldPos);

    dc_environment env = dcEmptyEnvironment();
    env.end = false;
    env.nether = false;
    env.underwater = v_envParams.x > 0.5;
    env.rainFactor = v_envParams.y;
    env.fogCol = FogColor.rgb;
    env = dcCalculateSunParams(env, TimeOfDay.x);

    vec3 sky;
    if (env.underwater) {
        // under the water the sun and the clouds are still seen through the surface
        sky = dcUnderwaterSky(s_DcCloudNoise, viewDir, env, CameraPosition.xyz, v_envParams.z);
    } else {
        dc_skycolor skyColors = dcSkyColors(env);
        vec3 glow = dcSunMoonGlow(viewDir, env);
        sky = dcRenderSky(skyColors, viewDir) + glow;

        vec4 clouds = dcClouds(s_DcCloudNoise, viewDir, env, skyColors, CameraPosition.xyz, glow, v_envParams.z);
        sky = mix(sky, clouds.rgb, clouds.a);
    }

    gl_FragColor = vec4(dcColorCorrection(sky), 1.0);
#else
    gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
#endif
}
