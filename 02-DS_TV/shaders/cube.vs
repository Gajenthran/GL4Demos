#version 330

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform int time;          // temps
uniform int move;          // mode "mouvement des cubes"
uniform sampler2D cube;
layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec3 vsiNormal;
layout (location = 2) in vec2 vsiTexCoord;
 
out vec3 vsoNormal;
out vec3 vsoModPos;

void main(void) {
  vec3 vsiP = vsiPosition;
  /* mouvement des cubes en modifiant les vertices des cubes */
  if (vsiP.y > 0 && move != 0) {
    vsiP.x += 1.5 * sin(time * 0.01);
  }
  vec2 uv = vec2(vsiTexCoord.x, 1.0 - vsiTexCoord.y);
  vec3 bpos = vsiP + 0.04 * texture(cube, uv).r * vsiP;
  vec4 mp = modelViewMatrix * vec4(bpos, 1.0);
  vsoNormal = (transpose(inverse(modelViewMatrix))  * vec4(vsiNormal, 0.0)).xyz;
  vsoModPos = mp.xyz;
  gl_Position = projectionMatrix * mp;
}