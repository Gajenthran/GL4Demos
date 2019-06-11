#version 330

uniform mat4 modelViewMatrix;
uniform mat4 projectionMatrix;
uniform sampler2D ebump;
uniform int basses;
uniform int pixel;
uniform int time;
uniform int state;
layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec3 vsiNormal;
layout (location = 2) in vec2 vsiTexCoord;
 
out vec3 vsoNormal;
out vec3 vsoModPos;
out vec2 vsoTexCoord;

void main(void) {
  vec3 vsiP = vsiPosition;
  float move;
  if(pixel == 1) {
    move = 0.0000004;
  } else {
    move = 0.04;
    if(state < 0)
      vsiP.xz += vsiP.xz * sin(basses * 10.0 * vsiP.y - 3.0 * time) * 0.05 * 1.0;
  }

  vsoTexCoord = vec2(vsiTexCoord.x, 1.0 - vsiTexCoord.y);
  vec3 bpos = vsiP + basses * move * texture(ebump, vsoTexCoord).r * vsiNormal;
  vec4 mp = modelViewMatrix * vec4(bpos, 1.0);
  vsoNormal = (transpose(inverse(modelViewMatrix))  * vec4(vsiNormal, 0.0)).xyz;
  vsoModPos = mp.xyz;
  gl_Position = projectionMatrix * mp;
}
