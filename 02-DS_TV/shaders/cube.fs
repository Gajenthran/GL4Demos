#version 330
uniform int basses;
uniform int time;
in  vec2 vsoTexCoord;
out vec4 fragColor;

void main(void) {
  /* Création d'une grille de 7*7 avec chaque rangée de la grille qui 
   * sera remplie en fonction de la musique.
   * TODO: faire en fonction des colonnes de la grille avec echantillons 
   */
  float nbLines = 7.0;
  vec2 pos = (fract(vsoTexCoord * nbLines) - 1.0) * 2.0;
  vec2 lpos = vsoTexCoord * nbLines;
  float c = clamp(step(0.8, abs(pos.x)) + step(0.9, abs(pos.y)), 0.0, 1.0); 
  c = 1.0 - c;
  float y = step(2.0, lpos.y * basses * 0.1);
  vec3 bg = vec3(1.0) * c * clamp(y, 0.0, 1.0);
  fragColor = vec4(bg, 1);
}
