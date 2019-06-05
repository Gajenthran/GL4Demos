#version 330
uniform int time;
uniform float onde;
in  vec2 vsoTexCoord;
out vec4 fragColor;


void main(void) {
  vec2 pos = abs(2.0 * (vsoTexCoord.xy) - vec2(1));
  float r = dot(pos/3, pos/3) * onde;
  vec3 c = vec3(sin(3.14 * 10 * mod(0.4 * time/1000 + r * 1.5 / 0.3, 0.3)));

  // vec4 t = mix(vec4(c, 1.0), balle(), smoothstep(.1, .009, r));
  // fragColor = t;

  vec2 p = abs(2.0 * (vsoTexCoord.xy) - vec2(1));
  float tau = 3.1415926535*2.0;
  float a = atan(p.x,p.y);
  float rad = length(p)*0.75;
  vec2 uv = vec2(a,rad);
  
  //get the color
  float xCol = (uv.x - (1 / 3.0)) * 3.0;
  xCol = mod(xCol, 3.0);
  vec3 horColour = vec3(0.15, 0.15, 0.15);
  
  if (xCol < 1.0) {
    horColour.g += 0.0;
  }
  else if (xCol < 2.0) {
    horColour.g += 0.0;
  }
  else {
    xCol -= 1.0;
    horColour.g += 1.0;
  }

  // draw color beam
  uv = (2.0 * uv) - 1.0;
  float beamWidth = abs(1.0 / (30.0 * uv.y));
  vec3 horBeam = vec3(beamWidth);
  vec4 f = vec4((( horBeam) * horColour), 1.0);

  uv = abs(2.0 * (vsoTexCoord.xy) - vec2(1));
  vec2 center = vec2(0);
  float speed = .01,
        ftime = time * speed,
        radius = 0.70 + 0.04 * sin(ftime),
        thickness = 0.09 + 0.05 * cos(ftime),
        dist = distance(uv, center);
  fragColor = vec4(smoothstep(thickness/2., 0., abs(dist-radius))) + f;
}
