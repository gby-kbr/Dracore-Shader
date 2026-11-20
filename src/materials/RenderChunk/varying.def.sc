vec4 a_color0    : COLOR0;
vec2 a_texcoord1 : TEXCOORD1;
vec3 a_position  : POSITION;
vec2 a_texcoord0 : TEXCOORD0;

vec4 i_data1 : TEXCOORD7;
vec4 i_data2 : TEXCOORD6;
vec4 i_data3 : TEXCOORD5;

vec4          v_clipPosition            : COLOR1;
vec4          v_color0                  : COLOR0;
vec2          v_ditheringAndMaskTinting : TEXCOORD2;
vec4          v_fog                     : COLOR2;
vec2          v_lightmapUV              : TEXCOORD1;
centroid vec2 v_texcoord0               : TEXCOORD0;
vec3          v_worldPos                : TEXCOORD3;
vec4          v_worldPosition           : COLOR3;
vec4          v_waterInfo               : TEXCOORD4; // x: night, y: is water, z: under water, w: the place (0.0 overworld, 1.0 End, 2.0 Nether)
