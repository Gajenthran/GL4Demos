#include "mobile.h"

#define MAX_COLOR 255

static mobile_t _mobile;

void mobileInit(void) {
  GLubyte r, g, b;
  _mobile.x = 0;
  _mobile.y = gl4dpGetHeight()/2;
  _mobile.r = (gl4dpGetWidth() / 40.0f) * (0.2f + 0.8f * rand() / (RAND_MAX + 1.0));
  r = rand()&0xFF; g = rand()&0xFF; b = rand()&0xFF;
  _mobile.c = RGB(r, g, b);
}

void mobileMove(void) {
  if(_mobile.x > gl4dpGetWidth())
    _mobile.x = 0;
  _mobile.x += 2.0;
}

void mobileDraw(void) {
  int c = _mobile.x * MAX_COLOR / gl4dpGetWidth();
  gl4dpClearScreenWith(RGB(c, c, c));
  gl4dpSetColor(_mobile.c);
  gl4dpFilledCircle(_mobile.x, _mobile.y, _mobile.r);
}
