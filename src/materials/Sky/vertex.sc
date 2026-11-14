#ifndef INSTANCING
  $input a_color0, a_position
  $output v_worldPos, v_underwaterRainTimeDay
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
    bool underwater = detectUnderwater(FogColor.rgb, FogAndDistanceControl.xy);
    v_underwaterRainTimeDay.x = float(underwater);
    v_underwaterRainTimeDay.y = underwater ? 0.0 : detectRain(FogAndDistanceControl.xyz); // the fog control reads like rain under water
    v_underwaterRainTimeDay.z = ViewPositionAndTime.w;
    v_underwaterRainTimeDay.w = detectDayFactor(FogColor.rgb);

    // background quad
    vec4 pos = vec4(a_position.xzy, 1.0);
    pos.xy = 2.0*clamp(pos.xy, -0.5, 0.5);

    v_worldPos = mul(u_invViewProj, pos).xyz;

    gl_Position = pos;
  #else
    gl_Position = vec4(0.0, 0.0, 0.0, 0.0);
  #endif
}
