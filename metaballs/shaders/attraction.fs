#version 330
uniform int time;
uniform float basses;
uniform int state;
in  vec2 vsoTexCoord;
out vec4 fragColor;

void main(void) {
  // Cercle coupée en couleur
  vec2 p = abs(2.0 * (vsoTexCoord.xy) - vec2(1));
  float tau = 3.1415926535;
  float a = atan(p.x,p.y);
  float rad = length(p)*0.75;
  vec2 uv = vec2(a,rad);
  
  //get the color
  float part = (uv.x - (3 / 3.0)) * 3.0;
  part = mod(part, 3.0);
  vec3 color = vec3(0.15, 0.15, 0.15);
  
  if (part < 1.0) {
    color.g += 0.0;
  } else {
    color.g += 1.0;
  }

  uv = (2.0 * uv) - 1.0;
  vec4 f = vec4(abs(1.0 / (120.0 * uv.y)) * color, 1.0);

  // Mouvement du rayon du cercle
  uv = abs(2.0 * (vsoTexCoord.xy) - vec2(1));
  vec2 center = vec2(0);
  float speed = 0.01;
  float ftime = time * speed;
  float radius = 0.70 + 0.04 * sin(ftime);
  float thickness = 0.09 + 0.05 * cos(ftime);
  float dist = distance(uv, center);

  fragColor = vec4(smoothstep(thickness/2.0, 0.0, abs(dist-radius))) + f;
}
