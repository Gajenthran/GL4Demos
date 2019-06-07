#version 330
uniform int time;
uniform int basses;
uniform int state;
uniform int circle;
uniform int color;
in  vec2 vsoTexCoord;
out vec4 fragColor;

void main(void) {
  // Cercle coupée en couleur
  vec2 pos = abs(2.0 * (vsoTexCoord.xy) - vec2(1));
  float a = atan(pos.x, pos.y);
  float radius = length(vec2(pos.x * 1.3, pos.y));
  vec2 uv = vec2(a, radius);
  
  float part = (uv.x - (color / 3.0)) * 3.0;
  part = mod(part, 3.0);
  vec3 color = vec3(0.15, 0.15, 0.15);
 
  if(part >= 1.0) {
    color.r += 1.0;
    color.g += 1.0;
  }

  uv = (2.0 * uv) - 1.0;
  int b = basses;
  if(circle != 0) {
    b *= 10;
  }
  vec4 colorCircle = vec4(abs(1.0 / (1200.0/b * uv.y)) * color, 1.0);
  fragColor = colorCircle;

  // Mouvement du rayon du cercle
  if(circle != 0) {
    vec2 center = vec2(0);
    float ftime = time * b * 0.001;
    radius = length(vec2(0.01 + 0.44 * cos(ftime), 0.09 + 0.45 * sin(ftime)));
    float thickness = (0.09 + 0.05 * cos(ftime)) / 2.0;
    float dist = distance(pos, center);
    fragColor = vec4(smoothstep(thickness, 0.0, abs(radius-dist))) + colorCircle;
  }
}
