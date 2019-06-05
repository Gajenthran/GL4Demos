#version 330 core
uniform vec4 couleur, lumpos;
uniform int id;
uniform sampler2D smTex;
uniform int time;
in  vec4 vsoNormal;
in  vec4 vsoMVPos;
in  vec4 vsoSMCoord;
in  vec2 vsoTexCoord;
layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 fragId;

float ring(vec2 p) {
  float r = length(p);
    r = log(sqrt(r));
    return abs(mod(4.0 * r, 6.0) - 3.0);
}

void main(void) {
  if(id == 2) {
    fragColor = vec4(1, 1, 0.5, 1);
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

    vec2 uv = (vsoTexCoord - 0.5);
    vec3 color = vec3(0.0);
    float rz = 1.0;
    uv /= exp(mod(1 * 3.15, 1.0)); // Remplacer par le tic de la musique
    rz *= abs(ring(uv));
    color = vec3(1.0, 1.0, 1.0) / rz;

    fragColor = vec4((couleur.rgb * diffuse), couleur.a);
    if(id == 3) {
      fragColor = mix(vec4((couleur.rgb * diffuse), couleur.a), vec4(color, 1.0), smoothstep(.1, .009, rz));
    }
  }
}
