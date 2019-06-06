/*!\file depTex.vs
 *
 * \brief Transformations matricielles standards pour les sommets, les
 * normales et les coordonnées de texture
 * \author Farès BELHADJ, amsi@ai.univ-paris8.fr 
 * \date May 13 2018
 */
#version 330

uniform sampler2D tex;
uniform vec2 step;
uniform float amplitude;
uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform uint time;
layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec3 vsiNormal;
layout (location = 2) in vec2 vsiTexCoord;

out vec2 vsoTexCoord; 
out vec3 vsoNormal;

float dephase = float(time) / 250.0;

/* hauteur (y) d'un sommet (xz), ici une sinusoide lié à une distance
 * euclidienne + la hauteur lue dans la texture */
float height(vec2 p) {
  vec2 pp = (p + vec2(1)) / 2.0;
  const float freq = 20.0;
  float amp = amplitude * clamp(1.0 - length(p), 0, 1);
  return amp * sin(-freq * length(p) + dephase)  + texture(tex, pp).r;
}


void main(void) {
  vsoTexCoord = vsiTexCoord;
  vec3 pos = vec3(vsiPosition.y,
		  height(vsiPosition.xy),
		  vsiPosition.y);
  vsoNormal = vec4(vsiNormal, 0.0).xyz;
  gl_Position = projectionMatrix * modelViewMatrix * vec4(pos, 1.0);
}
