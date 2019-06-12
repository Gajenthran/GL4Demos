#version 330
uniform int basses;        // basses
uniform int id;            // identifiant
uniform int state;         // états de la démo
uniform int time;          // temps
uniform int swirl, pixel;  // modes
uniform float pixelPrec;   // précision des pixels
uniform vec2 steps;        // pas
uniform vec4 lumPos;       // lumière  
uniform sampler2D eday, egloss, ebump;
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

void main(void) {
  /* ajout de lumières sur la texture (lumière diffuse, lumière
   * ambiante et spéculaire) */ 
  if(id == 1) {
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
    Ispec = (0.3 + 0.7 * texture(egloss, vsoTexCoord).r) * pow(clamp(dot(R, -V), 0, 1), 10);
    color = texture(eday, vsoTexCoord);
    fragColor = lum_diffus * color * Idiffuse + lum_amb * Iamb * color + lum_spec * Ispec;

    if(swirl != 0) {
      /* mélange des éléments de la texture créant une sorte de tourbillon */
      vec2 vecteur = vsoTexCoord - vec2(0.5) * 50;
      float distance = length(vecteur);
      float angle = atan(vecteur.y, vecteur.x);
      angle +=  5 * time / (1.0 + distance);
      vec2 tc = vec2(0.5) + vec2(distance * cos(angle), distance * sin(angle));
      fragColor = texture(eday, tc);
    } else if(pixel != 0) {
        /* pixellisation de la texture */
        vec2 uv = vsoTexCoord;
        uv.x -= mod(uv.x, 1.0 / pixelPrec);
        uv.y -= mod(uv.y, 1.0 / pixelPrec);
        fragColor = texture(eday, uv);
    }
  } else {
    if(basses >= 13)
      fragColor = vec4(1.0, 1.0, 0.0, 1.0);
    else
      fragColor = vec4(1.0);
  }
}