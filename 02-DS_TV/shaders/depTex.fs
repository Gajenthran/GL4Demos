/*!\file depTex.fs
 *
 * \brief rendu avec lumière directionnelle diffuse et texture.
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr 
 * \date May 13 2018
 */
#version 330
in  vec2 vsoTexCoord;
in  vec3 vsoNormal;
out vec4 fragColor;

void main(void) {
  // float timescale = 2.0;
	// vec2 uv = vsoTexCoord;
  // float soften = 5.0;
  // float t = 1 * timescale;
  // float d = length(uv);
  // float r = 0.25 + wave(uv, t) * 0.05 + wave(uv, t * 3.0) * 0.5;
  // vec2 col = vec2(smoothstep(r, r + soften, d));
  // fragColor = vec4(col,1.0, 1.0);

  // vec3 N = normalize(vsoNormal);
  // vec3 L = normalize(vec3(-1, -1, 0));
  // float diffuse = dot(N, -L);

  fragColor = vec4(vec3(dot(vec3(0, 0, 1), normalize(vsoNormal))), 1.0);
  // fragColor = vec4((texture(tex, vsoTexCoord).rgb ), 1);
}
