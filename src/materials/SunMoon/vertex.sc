$input a_color0, a_position, a_texcoord0
$output v_texcoord0

#include <bgfx_shader.sh>

#ifndef INSTANCING
  #include <dracore/main.sh>

  uniform vec4 TimeOfDay;
#endif

// The sun and the moon are placed here, as in newb-x-mcbe (MIT), so they sit on the same tilted path as the sun and
// moon direction of calculateSunParams: the water reflection uses that direction.
void main() {
  v_texcoord0 = a_texcoord0;
  #ifndef INSTANCING
    vec3 pos = a_position;
    pos.x = -pos.x;

    // the sun and the moon are two draws of the same quad, told apart by where the game puts them
    vec2 dir = mul(u_model[0], vec4(0.0, 0.0, 0.0, 1.0)).xy;
    float st = 2.0*PI*TimeOfDay.x + 0.5*PI;
    bool isSun = dot(vec2(cos(st), sin(st)), dir) > 0.0;

    float dist = 300.0;
    float angle = 0.0;
    float tilt = 0.0;
    float yaw = 0.0;
    if (isSun) {
      dist = -dist;
      pos.x = -pos.x;
      pos.xz *= DC_SUN_SIZE;
      angle = degToRad(DC_SUN_TILT);
      tilt = degToRad(DC_SUN_PATH_TILT);
      yaw = degToRad(DC_SUN_PATH_YAW);
    } else {
      pos.xz *= DC_MOON_SIZE;
      angle = degToRad(DC_MOON_TILT);
      tilt = degToRad(DC_MOON_PATH_TILT);
      yaw = degToRad(DC_MOON_PATH_YAW);
    }
    pos.xz = mul(rmat2(angle + 0.5*st), pos.xz);

    vec4 wpos = vec4(dist, 70.0*pos.xz, 1.0);
    wpos.xy = mul(rmat2(st), -wpos.xy);
    wpos.yz = mul(rmat2(tilt), wpos.yz);
    wpos.xz = mul(rmat2(yaw), wpos.xz);

    gl_Position = mul(u_viewProj, wpos);
  #else
    gl_Position = vec4(0.0, 0.0, 0.0, 0.0);
  #endif
}
