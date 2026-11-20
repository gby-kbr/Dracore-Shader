$input a_position
#ifdef INSTANCING
    $input i_data0, i_data1, i_data2, i_data3
#endif

$output v_worldPos, v_envParams

#include <bgfx_shader.sh>
#include <dracore/main.sh>

uniform vec4 FogColor;
uniform vec4 FogAndDistanceControl;
uniform vec4 ViewPositionAndTime;
uniform vec4 CameraPosition;

void main() {
#ifdef INSTANCING
    mat4 model = mtxFromCols(i_data0, i_data1, i_data2, i_data3);
#else
    mat4 model = u_model[0];
#endif

    // The game's cloud mesh is only used as a flat sheet at DC_CLOUD_HEIGHT in the world: the clouds are
    // drawn per pixel in the fragment shader, so they can be flown up to and over
    vec3 worldPosition = mul(model, vec4(a_position, 1.0)).xyz;
    worldPosition.y = DC_CLOUD_HEIGHT - CameraPosition.y;

    bool underwater = dcDetectUnderwater(FogColor.rgb, FogAndDistanceControl.xy);
    v_envParams = vec3(float(underwater), dcDetectRain(FogAndDistanceControl.xyz), ViewPositionAndTime.w);
    v_worldPos = worldPosition;

    // under the water the clouds are drawn in the sky, not on the sheet
    gl_Position = underwater ? vec4(0.0, 0.0, 0.0, 0.0) : mul(u_viewProj, vec4(worldPosition, 1.0));
}
