#version 330
uniform mat4 modelViewMatrix;
uniform int swirl;
uniform int temps;
uniform int direction;
uniform sampler2D tex0;
in  vec3 vsoNormal;
in  vec2 vsoTexCoord;
in  vec3 vsoModPos;
out vec4 fragColor;

void main(void) {
  vec4 color = texture(tex0, vsoTexCoord);
	vec2 vecteur = vsoTexCoord - vec2(0.5) * direction;
	float distance = length(vecteur);
 	float angle = atan(vecteur.y, vecteur.x);
	angle +=  0.0005 * temps / (1.0 + distance);
	vec2 tc = vec2(0.5) + vec2(distance * cos(angle), distance * sin(angle));
	fragColor = texture(tex0, tc);
	if(swirl == 0)
		fragColor = texture(tex0, vsoTexCoord);
}
