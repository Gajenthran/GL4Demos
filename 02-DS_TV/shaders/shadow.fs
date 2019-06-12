#version 330 core
uniform int id;           // identifiant
uniform int state;        // états de la démo
uniform int time;         // temps
uniform vec4 couleur;     // couleur
uniform vec4 lumpos;      // lumière
uniform sampler2D smTex;
in  vec4 vsoNormal;
in  vec4 vsoMVPos;
in  vec4 vsoSMCoord;
in  vec2 vsoTexCoord;
layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 fragId;

/* création d'un anneau */
float ring(vec2 p) {
    float r = log(sqrt(length(p)));
    return abs(mod(9.0 * r, 3.0) - 2.0);
}

void main(void) {
  /* dessine le cercle en jaune pour le soleil (id = 2) */
  if(id == 2) {
    fragColor = vec4(1, 1, 0.5, 1);
  /* dessine le cercle en gris pour le nuage (id = 4) */
  } else if(id == 4) {
    fragColor = vec4(0.5, 0.5, 0.5, 1);
  } else {
    vec3 N = normalize(vsoNormal.xyz);
    vec3 L = normalize(vsoMVPos.xyz - lumpos.xyz);
    vec3 projCoords = vsoSMCoord.xyz / vsoSMCoord.w;
    float diffuse = dot(N, -L);
    if(id != 1) {
      if(diffuse < 0.3)
        diffuse = 0.1;
      else if(diffuse < 0.6)
        diffuse = 0.5;
      else if(diffuse < 0.9)
        diffuse = 0.75;
      else
        diffuse = 1.0;
    }
    if(texture(smTex, projCoords.xy).r  <  projCoords.z)
      diffuse *= 0.0;
      
    /* dessine la texture uniquement */
    if(state < 3)
      fragColor = vec4((couleur.rgb), couleur.a);
    /* dessine la texture avec la diffuse */
    else
      fragColor = vec4((couleur.rgb * diffuse), couleur.a);
    /* dessine des anneaux progressives sur le cercle (id = 3) */
    if(id == 3) {
      vec2 uv = (vsoTexCoord - 0.5);
      vec3 color = vec3(0.0);
      float rz = 1.0;
      uv /= exp(mod(time * 3.15, 1.0));
      rz *= abs(ring(uv));
      color = vec3(rz)  + vec3(1.0, 1.0, 1.0);
      if(state < 1) 
        fragColor = vec4((couleur.rgb), couleur.a);
      else if(state >= 1 && state <= 3)
        fragColor = vec4((couleur.rgb * diffuse), couleur.a);
      else {
        fragColor = mix(vec4((couleur.rgb * diffuse), couleur.a), vec4(color, 1.0), smoothstep(.1, .009, rz));
      }
    }
  }
}
