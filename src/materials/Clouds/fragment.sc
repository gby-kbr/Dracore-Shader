$input v_color0, v_texcoord0, v_worldPos

#include <bgfx_shader.sh>

void main() {
    // nothing is drawn, see vertex.sc
    gl_FragColor = vec4(0.0, 0.0, 0.0, 0.0);
}
