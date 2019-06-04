#version 330
in  vec3 vsoModPos;
in  vec2 vsoTexCoord;
out vec4 fragColor;


vec3 illusion(void) {

  vec2 st = vsoTexCoord.xy;
  vec3 color = vec3(1.0);

  float px = 0.75;
  float py = 0.5;
  //vec2 position = vec2(py)-st;
  vec2 position = vec2(px, py)-st;
  float rad = length(position) * 2.0;
  float ang = atan(position.y, position.x);

  float f;
  f = cos(ang * 1.0);
  //f = abs(cos(ang * 3.0));
  //f = abs(cos(ang * 2.5)) * 0.5 + 0.3;
  //f = abs(cos(ang * 7.5)) * 0.5 + 0.3;
  //f = abs(cos(ang * 12.0) * sin(ang * 3.0)) * 0.8 + 0.1;
  color = vec3(1.0 - smoothstep(f, f + 0.0, rad));

  return color;
}

void main(void) {
  vec4 color = vec4(1, 1, 1, 1) 
  fragColor = color;
}
