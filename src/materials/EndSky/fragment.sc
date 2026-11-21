$input v_color0, v_texcoord0, v_worldPos

#include <bgfx_shader.sh>
#include <dracore/main.sh>

SAMPLER2D_AUTOREG(s_SkyTexture);

void main() {
    vec3 dir = normalize(v_worldPos);
    // level 0 only: mip selection from the atan2 derivatives would draw a line at the seam
    vec3 tex = texture2DLod(s_SkyTexture, dcEndSkyUV(dir), 0.0).rgb;
    gl_FragColor = vec4(dcEndSkyColor(tex), 1.0);
}
