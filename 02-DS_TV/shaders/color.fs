#version 330
uniform int time;
uniform int basses, state;
uniform int circle, side;
uniform float color[4];
in  vec2 vsoTexCoord;
out vec4 fragColor;

void main(void) {
  /* Cercle coupée en plusieurs parties avec une partie 
   * colorée et l'autre non. */
  vec2 pos = abs(2.0 * (vsoTexCoord.xy) - vec2(1));
  float a = atan(pos.x, pos.y);
  float radius = length(vec2(pos.x * 1.3, pos.y));
  vec2 uv = vec2(a, radius);
  
  float part = (uv.x - (side / 3.0)) * 3.0;
  part = mod(part, 3.0);
  vec3 c = vec3(0.15, 0.15, 0.15);
 
  if(part >= 1.0) {
    c.r += 1.0;
    c.g += 1.0;
    c.r += color[0];
    c.g += color[1];
    c.b += color[2];
  }

  uv = (2.0 * uv) - 1.0;
  int b = basses * 5;
  if(circle != 0) {
    b *= 5;
  }
  vec4 colorCircle = vec4(abs(1.0 / (1200.0/b * uv.y)) * c, 1.0);
  fragColor = colorCircle;

  /* Dessin du rayon du cercle qui s'agrandira en fonction 
   * de la musique */
  if(circle != 0) {
    if(state == -1) {
      fragColor = vec4(0.0);
    } else {
      vec2 center = vec2(0);
      float ftime = time * b * 0.001;
      radius = length(vec2(0.01 + 0.44 * cos(ftime), 0.09 + 0.45 * sin(ftime)));
      float thickness = (0.09 + 0.05 * cos(ftime)) / 2.0;
      float dist = distance(pos, center);
      fragColor = vec4(smoothstep(thickness, 0.0, abs(radius-dist))) + colorCircle;
    }
  }
}
