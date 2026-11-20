$input v_fog, v_occlusionHeight, v_occlusionUV, v_texcoord0, v_local

#include <bgfx_shader.sh>
#include <dracore/main.sh>

uniform vec4 OcclusionHeightOffset;

SAMPLER2D_AUTOREG(s_WeatherTexture);
SAMPLER2D_AUTOREG(s_OcclusionTexture);
SAMPLER2D_AUTOREG(s_LightingTexture);

float getOcclusionHeight(const vec4 occlusionTextureSample) {
    return occlusionTextureSample.g + (occlusionTextureSample.b * 255.0) - (OcclusionHeightOffset.x / 255.0);
}

bool isOccluded(const vec2 occlusionUV, const float occlusionHeight, const float occlusionHeightThreshold) {
#if defined(NO_OCCLUSION)
    return false;
#else
    bool withinBounds = (occlusionUV.x >= 0.0 && occlusionUV.x <= 1.0 && occlusionUV.y >= 0.0 && occlusionUV.y <= 1.0);
    #if defined(FLIP_OCCLUSION)
        return withinBounds && (occlusionHeight > occlusionHeightThreshold);
    #else
        return withinBounds && (occlusionHeight < occlusionHeightThreshold);
    #endif
#endif
}

void main() {
    vec4 diffuse = texture2D(s_WeatherTexture, v_texcoord0);
    vec4 occlusionSample = texture2D(s_OcclusionTexture, v_occlusionUV);

    float occlusionLuminance = occlusionSample.r;
    float occlusionHeightThreshold = getOcclusionHeight(occlusionSample);

    if (isOccluded(v_occlusionUV, v_occlusionHeight, occlusionHeightThreshold)) {
        diffuse.a = 0.0;
    } else {
        float mixAmount = (v_occlusionHeight - occlusionHeightThreshold) * 25.0;
        vec2 lightingUV = vec2(occlusionLuminance - (mixAmount * occlusionLuminance), 1.0);
        diffuse.rgb *= texture2D(s_LightingTexture, lightingUV).rgb;
        if (v_local.z > 0.5) {
            diffuse.a *= DC_RAIN_OPACITY; // rain is mostly see-through
        } else {
            vec2 c = v_local.xy * 2.0 - 1.0;
            diffuse.a *= 1.0 - smoothstep(0.35, 1.0, dot(c, c)); // soft round snowflakes, never squares
        }
        diffuse.rgb = diffuse.rgb*v_fog.a + v_fog.rgb;
        diffuse.a *= v_fog.a; // far drops fade into the fog
    }
    diffuse.rgb = dcColorCorrection(diffuse.rgb);
    gl_FragColor = diffuse;
}
