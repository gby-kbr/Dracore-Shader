$input a_color0, a_position, a_texcoord0
#ifdef INSTANCING
    $input i_data1, i_data2, i_data3
#endif

$output v_color0, v_texcoord0, v_worldPos

#include <bgfx_shader.sh>

void main() {
    // the game's own clouds are hidden, Dracore draws its clouds in the Sky material
    v_color0 = vec4(0.0, 0.0, 0.0, 0.0);
    v_texcoord0 = vec2(0.0, 0.0);
    v_worldPos = vec3(0.0, 0.0, 0.0);
    gl_Position = vec4(0.0, 0.0, 0.0, 0.0);
}
