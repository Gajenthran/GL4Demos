#version 330
uniform int time;
uniform float formCoord[32];
uniform float onde;
in  vec2 vsoTexCoord;
out vec4 fragColor;

float wave(vec2 v, float t) {
  return pow(sin(atan(v.y, v.x) * 8.0 + t), 2.0);
}

float random (vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

void main(void) {
  /* float timescale = .002;
  // vec2 uv = vsoTexCoord * 2.5;
  vec2 uv = abs(2.0 * (vsoTexCoord.xy) - vec2(1));
  float soften = 5.0;

  float t = time * timescale;
  float d = length(uv);
  float r = 0.25 + wave(uv, t) * 0.005 + wave(uv, t * 3.0) * 0.05;
  
  vec3 color = vec3(smoothstep(r, r, d));
  
  fragColor = vec4(color, 1.0); //  +  vec4(sin(length(uv * 0.08) * 25. + time * 0.02));
  // fragColor += color2; */
  vec2 uv = vsoTexCoord * 2.5;
    vec2 u = fract(uv*10.0)-0.5;
    float s = random(floor(uv*10.0)*10.0) * 0.3;
    float a = random(floor(uv*10.0)) + time * 0.01;
    u += vec2(sin(a),cos(a)) * 0.3;
    float d = length(u);
    float k = smoothstep(d, d + 0.5, s);
    fragColor = vec4(k);
}