#ifndef INSTANCING
    $input a_position
    $output v_worldPos, v_envParams
#endif

#include <bgfx_shader.sh>

#ifndef INSTANCING
    #include <dracore/main.sh>

    uniform vec4 FogColor;
    uniform vec4 FogAndDistanceControl;
    uniform vec4 ViewPositionAndTime;
#endif

void main() {
#ifndef INSTANCING
    v_envParams = vec3(float(dcDetectUnderwater(FogColor.rgb, FogAndDistanceControl.xy)),
                       dcDetectRain(FogAndDistanceControl.xyz),
                       ViewPositionAndTime.w);

    // one quad over the whole screen, the sky is drawn per pixel from the view direction
    vec4 pos = vec4(a_position.xzy, 1.0);
    pos.xy = 2.0*clamp(pos.xy, -0.5, 0.5);
    v_worldPos = mul(u_invViewProj, pos).xyz;

    gl_Position = pos;
#else
    // the game's own sky dome is hidden
    gl_Position = vec4(0.0, 0.0, 0.0, 0.0);
#endif
}
