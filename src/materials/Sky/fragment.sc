#ifndef INSTANCING
    $input v_worldPos, v_envParams
#endif

#include <bgfx_shader.sh>

#ifndef INSTANCING
    #include <dracore/main.sh>

    uniform vec4 TimeOfDay;
    uniform vec4 FogColor;
#endif

void main() {
#ifndef INSTANCING
    vec3 viewDir = normalize(v_worldPos);

    dc_environment env;
    env.end = false;
    env.nether = false;
    env.underwater = v_envParams.x > 0.5;
    env.rainFactor = v_envParams.y;
    env.fogCol = FogColor.rgb;
    env = dcCalculateSunParams(env, TimeOfDay.x);

    dc_skycolor skyColors = dcSkyColors(env);
    vec3 sky = dcRenderSky(skyColors, viewDir);
    sky += dcSunMoonGlow(viewDir, env);

    gl_FragColor = vec4(dcColorCorrection(sky), 1.0);
#else
    gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
#endif
}
