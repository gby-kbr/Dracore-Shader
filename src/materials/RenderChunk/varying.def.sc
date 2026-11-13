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
vec2          v_lightmapUV              : TEXCOORD1;
centroid vec2 v_texcoord0               : TEXCOORD0;
vec3          v_worldPos                : TEXCOORD3;
vec4          v_worldPosition           : COLOR3;
vec3          v_fogMul                  : TEXCOORD4; // fog: color*mul + add
vec3          v_fogAdd                  : TEXCOORD5;
vec4          v_refl                    : TEXCOORD6; // water: reflection*fresnel, water flag in alpha
