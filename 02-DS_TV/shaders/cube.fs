#version 330
uniform int basses;
in  vec2 vsoTexCoord;
out vec4 fragColor;

void main(void) {
  float n = 7.0;
  vec2 st = (fract(vsoTexCoord * n) - 1.0) * 2.0;
  vec2 ist = vsoTexCoord * n;
  float c = clamp(step(0.8, abs(st.x)) + step(0.9, abs(st.y)), 0.0, 1.0); 
  c = 1.0 - c;
  float y = step(1.0, ist.y * basses * 0.1);
  vec3 bg = vec3(1.0) * c * clamp(y, 0.0, 1.0);
  fragColor = vec4(bg, 1);
}
