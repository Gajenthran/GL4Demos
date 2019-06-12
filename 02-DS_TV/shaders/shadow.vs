#version 330 core

uniform mat4 modelMatrix;
uniform mat4 cameraViewMatrix;
uniform mat4 cameraProjectionMatrix;
uniform mat4 lightViewMatrix;
uniform mat4 lightProjectionMatrix;
uniform int id;    // identifiant
uniform int time;  // temps
layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec3 vsiNormal;
layout (location = 2) in vec2 vsiTexCoord;
 
out vec4 vsoNormal;
out vec4 vsoMVPos;
out vec4 vsoSMCoord;
out vec2 vsoTexCoord;

void main(void) {
  vec3 vsiP = vsiPosition;
  /* modification de la forme de la sphère (nuages et soleil) pour donner une 
   * impression de dessin */
  if(id == 2 || id == 4) {
    vsiP *= 2;
    float theta = atan(vsiP.x, vsiP.y);
    float pi = 3.14159;
    vsiP.xy += vsiP.xy * cos(theta * pi - time * 0.01) * sin(3.0 * theta) / 4.0;
  } 
  const mat4 bias = mat4( 0.5, 0.0, 0.0, 0.0,
              0.0, 0.5, 0.0, 0.0,
              0.0, 0.0, 0.5, 0.0,
              0.5, 0.5, 0.5, 1.0 );
  vsoMVPos    = cameraViewMatrix * modelMatrix * vec4(vsiP, 1.0);
  vsoNormal   = transpose(inverse(cameraViewMatrix * modelMatrix))  * vec4(vsiNormal, 0.0);
  gl_Position = cameraProjectionMatrix * vsoMVPos;
  vsoSMCoord  = bias * lightProjectionMatrix * lightViewMatrix * modelMatrix * vec4(vsiP, 1.0);
  vsoTexCoord = vsiTexCoord;
}