$input a_color0, a_position, a_texcoord0, a_texcoord1
#ifdef INSTANCING
    $input i_data1, i_data2, i_data3
#endif

$output v_clipPosition, v_color0, v_ditheringAndMaskTinting, v_fog, v_glint, v_lightCol, v_lightDir, v_lightmapUV, v_refl, v_texcoord0, v_worldPos, v_worldPosition

#include <bgfx_shader.sh>
#include <MinecraftRenderer.Materials/TAAUtil.dragonh>
#include <dracore/main.sh>

uniform vec4 RenderChunkFogAlpha;
uniform vec4 FogAndDistanceControl;
uniform vec4 ViewPositionAndTime;
uniform vec4 FogColor;
uniform vec4 MeshContext;
uniform vec4 TimeOfDay;
uniform vec4 CameraPosition;

void main() {
#ifdef INSTANCING
    mat4 model = mtxFromCols(i_data1, i_data2, i_data3, vec4(0.0, 0.0, 0.0, 1.0));
    vec3 worldPos = instMul(model, vec4(a_position, 1.0)).xyz;
#else
    vec3 worldPos = mul(u_model[0], vec4(a_position, 1.0)).xyz;
#endif

    vec4 color;
#ifdef RENDER_AS_BILLBOARDS
    worldPos += vec3_splat(0.5);
    vec3 viewDir = normalize(worldPos - ViewPositionAndTime.xyz);
    vec3 boardPlane = normalize(cross(vec3(0.0, 1.0, 0.0), viewDir));
    worldPos -= (cross(viewDir, boardPlane) * (a_color0.z - 0.5)) + (boardPlane * (a_color0.x - 0.5));
    color = vec4_splat(1.0);
#else
    color = a_color0;
#endif

    vec4 fogAndDist = FogAndDistanceControl;
    if (MeshContext.x > 0.5) {
        fogAndDist = vec4(0.99, 1.0, 100000.0, 100000.0);
    }

    uvec2 uv0_16 = uvec2(round(a_texcoord0 * 65535.0));
    vec2 uv0 = vec2(float((uv0_16.x & 32767u) << uint(1)), float((uv0_16.y & 32767u) << uint(1))) * 1.525902189314365386962890625e-05;
    uv0.x += 3.0517578125e-05 * ((2.0 * float((uv0_16.x & 32768u) >> uint(15))) - 1.0);
    uv0.y += 3.0517578125e-05 * ((2.0 * float((uv0_16.y & 32768u) >> uint(15))) - 1.0);

    uvec2 uv1_16 = uvec2(round(a_texcoord1 * 65535.0));
    float lightmapX = float((uv1_16.y >> 4u) & 15u) * 0.06666667;
    float lightmapY = float(uv1_16.y & 15u) * 0.06666667;
    vec2 lightmapUV = vec2(lightmapX, lightmapY);

    float ditherX = float((uv1_16.x & 256u) != 0u ? 1.0 : 0.0);
    float ditherY = float((uv1_16.y & 256u) != 0u ? 1.0 : 0.0);
    vec2 ditheringAndMaskTinting = vec2(ditherX, ditherY);

    // the environment is needed by the fog and by the water
    dc_environment env;
    dc_skycolor skycol;
    vec4 refl = vec4_splat(0.0); // water: reflection*fresnel, water flag in alpha
    vec4 glint = vec4_splat(0.0); // water: reflected direction, fresnel
    vec3 lightDir = vec3(0.0, 1.0, 0.0);
    vec3 lightCol = vec3_splat(0.0);
    if (MeshContext.x < 0.5) {
        env = dcDetectEnvironment(TimeOfDay.x, FogColor.rgb, FogAndDistanceControl.xyz);
        skycol = dcSkyColors(env);
#if defined(TRANSPARENT) && !(defined(RENDER_AS_BILLBOARDS) || defined(SEASONS))
        // the vanilla water is replaced, it has a bluish color and is see-through
        if (a_color0.b > 0.3 && a_color0.a < 0.95) {
            vec3 relPos = worldPos - ViewPositionAndTime.xyz;
            float dist = length(relPos);
            refl = dcWater(color, worldPos, skycol, env, relPos/max(dist, 0.0001), CameraPosition.xyz, lightmapUV.y, dist, ViewPositionAndTime.w, glint, lightDir, lightCol);
        }
#endif
    }

    vec4 position = jitterVertexPosition(worldPos);

    float camDist = length(ViewPositionAndTime.xyz - worldPos);
    float fogIntensity = clamp((((camDist / fogAndDist.z) + RenderChunkFogAlpha.x) - fogAndDist.x) / (fogAndDist.y - fogAndDist.x), 0.0, 1.0);

#if defined(TRANSPARENT)
    if (a_color0.a < 0.95) {
        if (refl.a < 0.5) { // water has its own opacity, it is not faded: the reflection is divided by it
            color.a = mix(a_color0.a, 1.0, clamp(camDist / FogAndDistanceControl.w, 0.0, 1.0));
        }
    }
#endif

    // fog is worked out here, once per vertex and not per pixel
    vec3 fogMul = vec3_splat(1.0 - fogIntensity);
    vec3 fogAdd = FogColor.rgb*fogIntensity;
    if (MeshContext.x < 0.5) {
        dcFogTerms(worldPos - ViewPositionAndTime.xyz, CameraPosition.xyz, lightmapUV.y, ViewPositionAndTime.w, env, skycol, FogColor.rgb, fogIntensity, fogMul, fogAdd);
    }
    v_fog = vec4(fogAdd, fogMul.x);
    v_refl = refl;
    v_glint = glint;
    v_lightDir = lightDir;
    v_lightCol = lightCol;

    v_clipPosition = position;
    v_worldPosition = vec4(worldPos, 1.0);
    v_color0 = color;
    v_ditheringAndMaskTinting = ditheringAndMaskTinting;
    v_lightmapUV = lightmapUV;
    v_texcoord0 = uv0;
    v_worldPos = worldPos;
    gl_Position = position;
}
