$input v_color0, v_texcoord0, v_worldPos, v_time

#include <bgfx_shader.sh>
#include <dracore/main.sh>

SAMPLER2D_AUTOREG(s_SunMoonTexture);
SAMPLER2D_AUTOREG(s_DcCloudNoise);
uniform vec4 SunMoonColor;
uniform vec4 FogColor;
uniform vec4 FogAndDistanceControl;
uniform vec4 TimeOfDay;
uniform vec4 CameraPosition;

void main() {
    vec4 color = texture2D(s_SunMoonTexture, v_texcoord0);
    color.rgb *= SunMoonColor.rgb;

    // the sun and the moon are brighter than the sky around them
    color.rgb *= 4.4*color.rgb;

    // SunMoonColor.a is how faded the disc is (for example in rain)
    float tr = 1.0 - SunMoonColor.a;
    color.a *= 1.0 - tr*tr;

    // the sun and the moon are far behind the clouds, so the clouds hide them
    dc_environment env = dcDetectEnvironment(TimeOfDay.x, FogColor.rgb, FogAndDistanceControl.xyz);
    dc_skycolor skyColors = dcSkyColors(env);
    vec4 clouds = dcClouds(s_DcCloudNoise, normalize(v_worldPos), env, skyColors, CameraPosition.xyz, vec3_splat(0.0), v_time.x);
    color.a *= 1.0 - clouds.a;

    gl_FragColor = vec4(dcColorCorrection(color.rgb), color.a);
}
