#version 330
uniform sampler1D mobiles;
uniform float step;
uniform int time;
uniform float onde;
in  vec2 vsoTexCoord;
out vec4 fragColor;

vec4 balle(void) {
  float i;
  for(i = step / 2.0; i < 1.0; i += 2.0 * step) {
    vec3 po = texture(mobiles, i + step).xyz;
    float d = length(vsoTexCoord - po.xy) - po.z;
    if(d < 0)
      return texture(mobiles, i);
  }
  return vec4(1);
}

void main(void) {
  vec2 pos = abs(2.0 * (vsoTexCoord.xy) - vec2(1));
  float r = dot(pos/3, pos/3) * onde;
  vec3 c = vec3(sin(3.14 * 10 * mod(0.4 * time/1000 + r * 1.5 / 0.3, 0.3)));

  vec4 t = mix(vec4(c, 1.0), balle(), smoothstep(.1, .009, r));
  fragColor = t;
}
