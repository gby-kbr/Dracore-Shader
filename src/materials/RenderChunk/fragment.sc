$input v_clipPosition, v_worldPosition, v_color0, v_ditheringAndMaskTinting, v_fog, v_glint, v_lightCol, v_lightDir, v_lightmapUV, v_refl, v_texcoord0, v_worldPos

#include <bgfx_shader.sh>
#include <MinecraftRenderer.Materials/DitherUtil.dragonh>
#include <dracore/main.sh>

uniform vec4 FogColor;
uniform vec4 FogAndDistanceControl;
uniform vec4 DitherParams;
uniform vec4 DitherParams2[3];
uniform vec4 ViewPositionAndTime;

SAMPLER2D_AUTOREG(s_MatTexture);
SAMPLER2D_AUTOREG(s_LightMapTexture);
#if defined(SEASONS)
SAMPLER2D_AUTOREG(s_SeasonsTexture);
#endif

void main() {
#if defined(DEPTH_ONLY) || defined(DEPTH_ONLY_OPAQUE) || defined(DEPTH_ONLY_ALPHA_TEST)
    #if defined(ALPHA_TEST)
    vec4 diffuse = texture2D(s_MatTexture, v_texcoord0);
    if (diffuse.a < 0.5) {
        discard;
    }
    #endif
    gl_FragColor = vec4_splat(0.0);
    return;
#else

    vec4 diffuse = texture2D(s_MatTexture, v_texcoord0);

#if defined(ALPHA_TEST)
    bool ditherDiscard = false;
    #if defined(DITHERING)
    if (v_ditheringAndMaskTinting.x > 0.5) {
        ditherDiscard = evaluateDitherDiscard(v_clipPosition, v_worldPosition.xyz, ViewPositionAndTime.xyz, u_view, DitherParams, DitherParams2[2]);
    }
    #endif
    if (ditherDiscard || diffuse.a < 0.5) {
        discard;
    }
#endif

#if defined(TRANSPARENT) && !defined(SEASONS)
    bool isWater = v_refl.a > 0.5;
#else
    const bool isWater = false;
#endif

#if defined(SEASONS)
    diffuse.rgb *= mix(vec3_splat(1.0), texture2D(s_SeasonsTexture, v_color0.xy).rgb * 2.0, v_color0.b);
    diffuse.rgb *= v_color0.a;
    diffuse.a = 1.0;
#else
    if (isWater) {
        // the vanilla water texture is replaced by a plain color, color and opacity come from the vertex shader
        diffuse.rgb = vec3_splat(1.0 - DC_WATER_TEX_OPACITY*(1.0 - diffuse.b*1.8)) * v_color0.rgb;
        diffuse.a = v_color0.a;
    } else if (v_ditheringAndMaskTinting.y > 0.5) {
        diffuse.rgb = mix(diffuse.rgb, diffuse.rgb * v_color0.rgb, diffuse.a) * v_color0.a;
        diffuse.a = 1.0;
    } else {
        diffuse.rgb *= v_color0.rgb;
        #if !defined(TRANSPARENT)
            diffuse.a = 1.0;
        #else
            diffuse.a *= v_color0.a;
        #endif
    }
#endif

#if defined(TRANSPARENT) && defined(DITHERING)
    if (v_ditheringAndMaskTinting.x > 0.5) {
        if (evaluateDitherDiscard(v_clipPosition, v_worldPosition.xyz, ViewPositionAndTime.xyz, u_view, DitherParams, DitherParams2[2])) {
            diffuse.a = 0.0;
        }
    }
#endif

    // ambient light has no block light, the block light gets the newb torch color
    vec3 ambientLight = texture2D(s_LightMapTexture, vec2(0.0, v_lightmapUV.y)).rgb;
    float blockLight = luminance(texture2D(s_LightMapTexture, vec2(v_lightmapUV.x, 0.0)).rgb);
    vec3 torchColor = dcTorchColor(
        detectUnderwater(FogColor.rgb, FogAndDistanceControl.xy),
        detectEnd(FogColor.rgb),
        detectNether(FogColor.rgb, FogAndDistanceControl.xy)
    );
    diffuse.rgb *= dcAddTorchLight(ambientLight, torchColor, blockLight);
#if defined(TRANSPARENT)
    if (isWater) {
        // the reflection and the glint of the sun or moon disc are added to the water, then blended with its alpha,
        // so they are divided by it to reach the eye in full. The distance between the two unit vectors is their angle,
        // and unlike a dot product close to 1.0 it stays precise at low float precision
        vec3 shine = v_refl.rgb;
        float angle = length(normalize(v_glint.xyz) - v_lightDir);
        float disc = 1.0 - smoothstep(0.767*DC_WATER_SUN_SIZE, DC_WATER_SUN_SIZE, angle);
        if (disc > 0.0) {
            shine += colorCorrection(v_lightCol*(DC_WATER_SUN_GLITTER*disc*v_glint.w));
        }
        diffuse.rgb += shine/max(diffuse.a, 0.05);
    }
#endif
    diffuse.rgb = diffuse.rgb*v_fog.a + v_fog.rgb; // fog, worked out in the vertex shader

    gl_FragColor = diffuse;
#endif
}
