$input v_color0, v_texcoord0, v_worldPos

#include <bgfx_shader.sh>
#include <dracore/main.sh>

void main() {
    vec4 color = v_color0;
    color.rgb = dcColorCorrection(color.rgb);
    gl_FragColor = color;
}
