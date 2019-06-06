#version 330
uniform int time;
uniform float formCoord[32];
uniform float onde;
in  vec2 vsoTexCoord;
out vec4 fragColor;

float rand(float n){
  return fract(sin(n) * 43758.5453123);
}

void main(void) {
  float pi = 3.14;
  float duration = 5.0;

  vec2 uv = vsoTexCoord * 5.0;
  vec2 pos = vec2(2.5, 2.5);
  vec2 n = normalize(uv - pos);
  
  float t = mod(time * 0.001, duration);
  float deg  = mix(270.0, 70.0, cos(pi/duration*t - pi)/2.0+0.5);
  float ydeg = sin(radians(deg));
    
  float dist = pow(pos.x - uv.x, 2.0) + pow(pos.y - uv.y, 2.0);
  if(dist > 0.1) {
    fragColor = vec4(0.0, 0.0, 0.0, 0.0);
  } else if(n.y > ydeg) {
    fragColor = vec4(0.0, 0.0, 0.0, 0.0);
  } else {
    fragColor = vec4(1.0, 1.0, 0.0, 1.0);
  }

  int i, nbForm = 32, it = 0;
  float st, end;
  for(i = 0; i < nbForm; i+=2) {
    uv = vsoTexCoord * 5.0;
    pos = vec2(formCoord[i], formCoord[i + 1]);
    n = normalize(uv - pos);
    
    if(it % 2 == 0) {
      st = 270.0;
      end = 70 - it;
    } else {
      st = 90.0;
      end = 270.0 - it;
    }

    t = mod(time * it * 0.0001, duration);
    deg  = mix(st, end, cos(pi/duration*t - pi)/ 2.0 + 0.5);
    ydeg = cos(radians(deg));
      
    dist = pow(pos.x - uv.x, 2.0) + pow(pos.y - uv.y, 2.0);
    if(dist > 0.1) {
      fragColor += vec4(0.0, 0.0, 0.0, 0.0);
    } else if(n.y > ydeg) {
      fragColor += vec4(0.0, 0.0, 0.0, 0.0);
    } else {
      fragColor += vec4(1.0, 1.0, 0.0, 1.0);
    }
    it++;
  }
}
