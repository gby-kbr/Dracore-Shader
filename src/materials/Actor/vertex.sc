$input a_position, a_color0, a_texcoord0, a_indices, a_normal
#ifdef INSTANCING
    $input i_data1, i_data2, i_data3
#endif

$output v_clipPosition, v_color0, v_fog, v_light, v_texcoord0, v_worldPos

#include <bgfx_shader.sh>
#include <MinecraftRenderer.Materials/DynamicUtil.dragonh>
#include <MinecraftRenderer.Materials/FogUtil.dragonh>
#include <MinecraftRenderer.Materials/TAAUtil.dragonh>
#include <dracore/main.sh>

uniform vec4 OverlayColor;
uniform vec4 TileLightColor;
uniform vec4 FogColor;
uniform vec4 FogControl;
uniform vec4 ViewPositionAndTime;
uniform vec4 TimeOfDay;
uniform vec4 CameraPosition;
uniform vec4 UVAnimation;
uniform mat4 Bones[8];

void main() {
    mat4 World = u_model[0];

    World = mul(World, Bones[int(a_indices)]);

    vec2 texcoord0 = a_texcoord0;
    texcoord0 = applyUvAnimation(texcoord0, UVAnimation);

    float lightIntensity = calculateLightIntensity(World, vec4(a_normal.xyz, 0.0), TileLightColor);
    lightIntensity += OverlayColor.a * 0.35;
    vec4 light = vec4(lightIntensity * TileLightColor.rgb, 1.0);

    vec3 worldPosition;
#ifdef INSTANCING
    mat4 model = mtxFromCols(i_data1, i_data2, i_data3, vec4(0.0, 0.0, 0.0, 1.0));
    worldPosition = instMul(model, vec4(a_position, 1.0)).xyz;
#else
    worldPosition = mul(World, vec4(a_position, 1.0)).xyz;
#endif

    vec4 position = jitterVertexPosition(worldPosition);
    float cameraDepth = position.z;
    float fogIntensity = calculateFogIntensity(cameraDepth, FogControl.z, FogControl.x, FogControl.y);

    // the fog is handed to the pixels as  color*fog.a + fog.rgb. Mobs get the same light fog as the
    // blocks, so they stand inside it; under water, in the Nether and in the End it is the vanilla fog
    vec4 fog = vec4(FogColor.rgb*fogIntensity, 1.0 - fogIntensity);
#if !(defined(DEPTH_ONLY) || defined(DEPTH_ONLY_OPAQUE) || defined(DEPTH_ONLY_ALPHA_TEST))
    dc_environment fogEnv = dcDetectEnvironment(TimeOfDay.x, FogColor.rgb, FogControl.xyz);
    if (!(fogEnv.underwater || fogEnv.nether || fogEnv.end)) {
        // mobs have no sky light, they are treated as standing in the open
        fog = dcFog(worldPosition - ViewPositionAndTime.xyz, CameraPosition.xz, CameraPosition.y, 1.0, FogControl.z, ViewPositionAndTime.w, fogEnv, dcSkyColors(fogEnv));
    }
#endif

#if defined(DEPTH_ONLY) || defined(DEPTH_ONLY_OPAQUE) || defined(DEPTH_ONLY_ALPHA_TEST)
    v_texcoord0 = vec2_splat(0.0);
    v_color0 = vec4_splat(0.0);
#else
    v_texcoord0 = texcoord0;
    v_color0 = a_color0;
#endif

    v_clipPosition = position;
    v_fog = fog;
    v_light = light;
    v_worldPos = worldPosition;
    gl_Position = position;
}
