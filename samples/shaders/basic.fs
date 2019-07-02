#version 330

uniform vec4 color;
in  vec3 vsoNormal;
out vec4 fragColor;

void main(void) {
  // vec3 N = normalize(vsoNormal);
  // vec3 L = normalize(vec3(-1, -1, 0));
  // float diffuse = dot(N, -L);
  fragColor = vec4((color.rgb), color.a);
}
