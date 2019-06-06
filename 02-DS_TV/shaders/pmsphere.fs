#version 330
uniform vec4 lumPos;
uniform vec2 steps;
uniform int time;
uniform int swirl;
uniform sampler2D eday, egloss, ebump;
uniform float basses, aigus;
in  vec3 vsoNormal;
in  vec3 vsoModPos;
in  vec2 vsoTexCoord;
out vec4 fragColor;

vec2 G[9] = vec2[9](vec2(-1, -1), vec2(0, -2), vec2(1, -1),
		    vec2(-2,  0), vec2(0,  0), vec2(2, 0),
		    vec2(-1,  1), vec2(0,  2), vec2(1,  1));

vec2 offset[9] = vec2[9](vec2(-steps.x,  steps.y), vec2(0,  steps.y), vec2(steps.x,  steps.y),
			 vec2(-steps.x,  0),       vec2(0,  0),       vec2(steps.x, 0),
			 vec2(-steps.x, -steps.y), vec2(0, -steps.y), vec2(steps.x, -steps.y));

vec2 sobel(sampler2D map) {
  vec2 g = vec2(0);
  for(int i = 0; i < 9; i++)
    g += G[i] * texture(map, vsoTexCoord + offset[i]).r;
  return g;
}

vec4 circle(vec2 pos, vec2 center, float radius, vec3 color, float antialias){   
  float d = length(pos - center) - radius;     
  float t = smoothstep (0.0, antialias, d);   
  return vec4(color, 1.0 - t);   
}

void main(void) {
  const vec4 lum_diffus = vec4(1, 1, 0.9, 1);
  const vec4 lum_amb = vec4(0.8, 0.8, 1, 1);
  const vec4 lum_spec = vec4(1, 1, 0.75, 1);
  const float Iamb = 0.15;

  vec3 L = normalize(vsoModPos - lumPos.xyz);
  float Idiffuse = 0, Ispec = 0;
  vec4 color = vec4(1);
  vec3 N = normalize(vsoNormal);
  vec3 B = cross(normalize(vec3(N.x, 0, N.z)), vec3(0, 1, 0));
  vec3 T = cross(N, B);
  vec2 v = 0.75 * sobel(ebump);
  N = normalize(N + v.x * B + v.y * T);
  Idiffuse = clamp(dot(N, -L), 0, 1);
  vec3 V = vec3(0, 0, -1);
  vec3 R = reflect(L, N);
  Ispec = aigus * (0.3 + 0.7 * texture(egloss, vsoTexCoord).r) * pow(clamp(dot(R, -V), 0, 1), 10);
  color = texture(eday, vsoTexCoord);

  vec2 vecteur = vsoTexCoord - vec2(0.5) * 50;
  float distance = length(vecteur);
  float angle = atan(vecteur.y, vecteur.x);
  angle +=  5 * time / (1.0 + distance);
  vec2 tc = vec2(0.5) + vec2(distance * cos(angle), distance * sin(angle));
  
  fragColor = lum_diffus * color * Idiffuse + lum_amb * Iamb * color + lum_spec * Ispec;
  if(swirl != 0) {
    fragColor = texture(eday, tc);
  }
}
