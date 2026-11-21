$input v_clipPosition, v_worldPosition, v_color0, v_ditheringAndMaskTinting, v_fog, v_lightmapUV, v_texcoord0, v_worldPos, v_waterInfo

#include <bgfx_shader.sh>
#include <MinecraftRenderer.Materials/DitherUtil.dragonh>
#include <dracore/main.sh>

uniform vec4 FogColor;
uniform vec4 DitherParams;
uniform vec4 DitherParams2[3];
uniform vec4 ViewPositionAndTime;
uniform vec4 TimeOfDay;
uniform vec4 FogAndDistanceControl;
uniform vec4 CameraPosition;

SAMPLER2D_AUTOREG(s_MatTexture);
SAMPLER2D_AUTOREG(s_LightMapTexture);
#if defined(TRANSPARENT) && !(defined(SEASONS) || defined(RENDER_AS_BILLBOARDS))
SAMPLER2D_AUTOREG(s_DcCloudNoise);
#endif
#if defined(SEASONS)
SAMPLER2D_AUTOREG(s_SeasonsTexture);
#endif
#if defined(OPAQUE) && !defined(SEASONS)
SAMPLER2D_AUTOREG(s_DcCausticNoise);
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

#if defined(SEASONS)
    diffuse.rgb *= mix(vec3_splat(1.0), texture2D(s_SeasonsTexture, v_color0.xy).rgb * 2.0, v_color0.b);
    diffuse.rgb *= v_color0.a;
    diffuse.a = 1.0;
#else
    if (v_ditheringAndMaskTinting.y > 0.5) {
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

    bool isWater = v_waterInfo.y > 0.5;
    bool underwater = v_waterInfo.z > 0.5;

    dc_environment env = dcEmptyEnvironment();
    dc_skycolor skyColors = dcEmptySkyColors();
    if (isWater || underwater) {
        env = dcDetectEnvironment(TimeOfDay.x, FogColor.rgb, FogAndDistanceControl.xyz);
        skyColors = dcSkyColors(env);
    }

    float time = ViewPositionAndTime.w;
    vec3 waterReflection = vec3(0.0, 0.0, 0.0);
    float waterReflectionAmount = 0.0;

#if defined(TRANSPARENT) && !(defined(SEASONS) || defined(RENDER_AS_BILLBOARDS))
    if (isWater) {
        // clear water: no biome tint, only the optional vanilla water texture
        diffuse.rgb = vec3_splat(1.0 - DC_WATER_TEX_OPACITY*(1.0 - diffuse.b*1.8));

        vec3 gPos = v_worldPos + CameraPosition.xyz;
        vec3 toWater = v_worldPos - ViewPositionAndTime.xyz;
        float camDist = length(toWater);
        vec3 viewDir = toWater/camDist; // from the camera to the water

        // the shape of the face tells the top of the water from its sides
        vec3 faceNormal = normalize(cross(dFdx(v_worldPos), dFdy(v_worldPos)));
        vec3 normal = abs(faceNormal.y) > 0.5 ? dcWaterNormal(gPos.xz, time, camDist) : faceNormal;
        if (dot(normal, viewDir) > 0.0) {
            normal = -normal;
        }
        vec3 reflDir = reflect(viewDir, normal);

        if (underwater) {
            // from under the water the surface mirrors the water below
            waterReflection = dcRenderSky(skyColors, reflDir);
        } else {
            // a reflected ray that would go below the horizon is held at the horizon
            vec3 dir = normalize(vec3(reflDir.x, max(reflDir.y, 0.0), reflDir.z));
            waterReflection = dcSkyRadiance(dir, env, skyColors, vec2(40.0, 6.0)*DC_WATER_SUN_GLITTER, s_DcCloudNoise, CameraPosition.xyz, time);
        }

        waterReflectionAmount = dcWaterShade(diffuse, v_color0.a);
        if (!underwater) {
            waterReflectionAmount *= DC_WATER_SURFACE_DIM;
        }

        // the waves as light and dark ripples, so they show looking straight down as well
        if (abs(faceNormal.y) > 0.5) {
            waterReflection *= max(1.0 + dcWaterRipple(gPos.xz, time, camDist), 0.0);
        }

        // the reflection is dimmer in the shade
        waterReflection *= 0.05 + v_lightmapUV.y*v_lightmapUV.y*1.14;
    }
#endif

    vec3 light = texture2D(s_LightMapTexture, v_lightmapUV).rgb;
    // the End has no sky light, the vanilla lightmap there is dark and green: a soft ambient takes its place
    if (abs(v_waterInfo.w - 1.0) < 0.5) {
        light = mix(DC_END_AMBIENT_COL*DC_END_AMBIENT, light*DC_END_AMBIENT_COL, smoothstep(0.1, 0.8, v_lightmapUV.x)*0.5);
    }
    // torches and glowing blocks: the warm version of the vanilla light
    light = dcTorchLight(light, v_lightmapUV.x, v_lightmapUV.y, 1.0 - v_waterInfo.x, v_waterInfo.w, underwater);
    if (abs(v_waterInfo.w) < 0.5 && !underwater) {
        light *= dcSunLightTint(cos(2.0*DC_PI*TimeOfDay.x), v_lightmapUV.y, dcDetectRain(FogAndDistanceControl.xyz));
    }
    if (underwater) {
        light = (light + DC_UNDERWATER_BRIGHTNESS)*dcUnderwaterTint(skyColors.horizon, v_lightmapUV.y*v_lightmapUV.y);
    }
    diffuse.rgb *= light;
    if (!isWater && v_waterInfo.x > 0.001) {
        diffuse.rgb = dcNightLight(diffuse.rgb, v_lightmapUV, v_waterInfo.x); // dark covered places, a dim moonlight
    }
    diffuse.rgb += waterReflection*waterReflectionAmount;

#if defined(OPAQUE) && !defined(SEASONS)
    {
        vec3 pixFrom = v_worldPos - ViewPositionAndTime.xyz;
        float pixDist = length(pixFrom);
        vec3 pixNormal = normalize(cross(dFdx(v_worldPos), dFdy(v_worldPos)));
        if (dot(pixNormal, pixFrom) > 0.0) {
            pixNormal = -pixNormal;
        }

        // rain wets every open surface, flat ground and slopes alike
        if (v_worldPosition.w > 0.01) {
            dc_environment wetEnv = dcDetectEnvironmentLite(TimeOfDay.x, FogColor.rgb, FogAndDistanceControl.xyz);
            diffuse.rgb = dcWetSurface(diffuse.rgb, v_worldPos + CameraPosition.xyz, pixNormal, pixFrom/pixDist, v_worldPosition.w, v_lightmapUV.y, pixDist, time, dcSkyColors(wetEnv));
        }

        // the moving light pattern on the bottom of the sea, from under and above the water
        diffuse.rgb = dcCaustics(s_DcCausticNoise, diffuse.rgb, v_worldPos.xz + CameraPosition.xz, v_worldPos.y + CameraPosition.y, pixDist, pixNormal, v_lightmapUV.y, TimeOfDay.x, FogColor.rgb, FogAndDistanceControl.xyz, time);
    }
#endif

    diffuse.rgb = diffuse.rgb*v_fog.a + v_fog.rgb; // fog, worked out in the vertex shader

    diffuse.rgb = dcColorCorrection(diffuse.rgb);

    gl_FragColor = diffuse;
#endif
}
