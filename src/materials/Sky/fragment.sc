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
    uniform vec4 SkyColor;
    uniform vec4 CloudColor;
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
    if (env.underwater) {
        // the fog gives no weather under the water, the game's sky color does: grey in rain, blue when clear
        float hi = max(max(SkyColor.r, SkyColor.g), SkyColor.b);
        float sat = (hi - min(min(SkyColor.r, SkyColor.g), SkyColor.b))/max(hi, 0.02);
        float satRain = 1.0 - smoothstep(0.10, 0.30, sat);
        // the clouds of the game turn dark grey in rain, by day: clear clouds are bright
        float cloudRain = clamp((0.85 - dot(CloudColor.rgb, vec3(0.2126, 0.7152, 0.0722)))/0.3, 0.0, 1.0)*smoothstep(0.15, 0.4, env.sunDir.y);
        env.rainFactor = max(satRain, cloudRain);
    }

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
