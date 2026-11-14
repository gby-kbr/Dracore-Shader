$input v_texcoord0, v_worldPos, v_time

#include <bgfx_shader.sh>

#ifndef INSTANCING
  #include <dracore/main.sh>

  uniform vec4 SunMoonColor;
  uniform vec4 TimeOfDay;
  uniform vec4 FogColor;
  uniform vec4 FogAndDistanceControl;

  SAMPLER2D_AUTOREG(s_SunMoonTexture);
  SAMPLER2D_AUTOREG(s_DcCloudNoise);
#endif

void main() {
  #ifndef INSTANCING
    vec4 color = texture2D(s_SunMoonTexture, v_texcoord0);
    color.rgb *= SunMoonColor.rgb;
    color.rgb *= 4.4*color.rgb;
    float tr = 1.0 - SunMoonColor.a;
    color.a *= 1.0 - tr*tr;

    // the sun and the moon are far behind the clouds, so the clouds in that direction hide them
    dc_environment env = dcDetectEnvironment(TimeOfDay.x, FogColor.rgb, FogAndDistanceControl.xyz);
    color.a *= 1.0 - dcCloudCover(s_DcCloudNoise, normalize(v_worldPos), env, v_time.x);

    color.rgb = colorCorrection(color.rgb);
    gl_FragColor = color;
  #else
    gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
  #endif
}
