$input v_worldPos, v_envParams

#include <bgfx_shader.sh>
#include <dracore/main.sh>

SAMPLER2D_AUTOREG(s_DcCloudNoise);

uniform vec4 TimeOfDay;
uniform vec4 FogColor;
uniform vec4 CameraPosition;

void main() {
    dc_environment env = dcEmptyEnvironment();
    env.rainFactor = v_envParams.y;
    env.fogCol = FogColor.rgb;
    env = dcCalculateSunParams(env, TimeOfDay.x);

    vec3 dir = normalize(v_worldPos);
    dc_skycolor skyColors = dcSkyColors(env);
    vec3 glow = dcSunMoonGlow(dir, env);

    vec4 clouds = dcCloudsAt(s_DcCloudNoise, v_worldPos.xz + CameraPosition.xz, env, skyColors, glow, v_envParams.z);

    // the far edge of the sheet fades out, the sky draws the clouds from there on
    float edge = 1.0 - smoothstep(250.0, 500.0, length(v_worldPos.xz));
    gl_FragColor = vec4(dcColorCorrection(clouds.rgb), clouds.a*edge);
}
