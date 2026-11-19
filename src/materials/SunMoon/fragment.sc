$input v_color0, v_texcoord0, v_worldPos

#include <bgfx_shader.sh>
#include <dracore/main.sh>

SAMPLER2D_AUTOREG(s_SunMoonTexture);
uniform vec4 SunMoonColor;

void main() {
    vec4 color = texture2D(s_SunMoonTexture, v_texcoord0);
    color.rgb *= SunMoonColor.rgb;

    // the sun and the moon are brighter than the sky around them
    color.rgb *= 4.4*color.rgb;

    // SunMoonColor.a is how faded the disc is (for example in rain)
    float tr = 1.0 - SunMoonColor.a;
    color.a *= 1.0 - tr*tr;

    gl_FragColor = vec4(dcColorCorrection(color.rgb), color.a);
}
