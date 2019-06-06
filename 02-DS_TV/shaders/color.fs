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
  vec2 p = abs(2.0 * (vsoTexCoord.xy) - vec2(1));
  float a = atan(p.x,p.y);
  float rad = length(p) * 1.5;
  vec2 uv = vec2(a,rad);
  
  //get the color
  float part = (uv.x - (color / 3.0)) * 3.0;
  part = mod(part, 3.0);
  vec3 color = vec3(0.15, 0.15, 0.15);
  
  if (part < 1.0) {
    color.g += 0.0;
  } else {
    color.g += 1.0;
  }

  uv = (2.0 * uv) - 1.0;
  int b = basses;
  if(circle != 0) {
    b *= 10;
  }
  vec4 f = vec4(abs(1.0 / (1200.0/b * uv.y)) * color, 1.0);
  fragColor = f;

  // Mouvement du rayon du cercle
  if(circle != 0) {
    uv = abs(2.0 * (vsoTexCoord.xy) - vec2(1));
    vec2 center = vec2(0);
    float speed = b * 0.001;
    float ftime = time * speed;
    float radius = 0.35 + 0.04 * sin(ftime);
    float thickness = 0.09 + 0.05 * cos(ftime);
    float dist = distance(uv, center);
    fragColor = vec4(smoothstep(thickness/2.0, 0.0, abs(dist-radius))) + f;
  }
}
