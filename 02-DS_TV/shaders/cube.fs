#version 330
uniform int basses;
uniform int time;
uniform int id;
uniform vec4 lumPos;
uniform sampler2D cube;
// in  vec2 vsoTexCoord;
in  vec3 vsoNormal;
in  vec3 vsoModPos;
out vec4 fragColor;

void main(void) {
  /* Création d'une grille de 7*7 avec chaque rangée de la grille qui 
   * sera remplie en fonction de la musique.
   * TODO: faire en fonction des colonnes de la grille avec echantillons 
   */
  if(id == 2) {
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
    Idiffuse = clamp(dot(N, -L), 0, 1);
    vec3 V = vec3(0, 0, -1);
    vec3 R = reflect(L, N);
    fragColor = lum_diffus * color * Idiffuse + lum_amb * Iamb * color + lum_spec * Ispec;
  } else {
    fragColor = vec4(1.0);
  }
}
