#include "mobile.h"

static mobile_t _mobile;
static float _gap = 0;
static float _radius = 0;

void mobileInit(float rad) {
  srand(time(NULL));
  GLubyte r, g, b;
  _mobile.x = gl4dpGetWidth()/2;
  _mobile.y = gl4dpGetHeight()/2;
  _mobile.r = rad;
  r = rand()&0xFF; g = rand()&0xFF; b = rand()&0xFF;
  _mobile.c = RGB(r, g, b);
  _radius = _mobile.r;
}

void mobileToLine(void) {
  _mobile.r = _radius - _gap;
  if(_mobile.r > 0.0)
    _gap += 1.5;
}

void mobileDraw(void) {
  int j;
  gl4dpSetColor(_mobile.c);
  gl4dpFilledCircle(_mobile.x, _mobile.y, _mobile.r);

  for(j = 0; j < _gap; j++) {
    gl4dpLine(_mobile.x + _mobile.r, _mobile.y, _mobile.x + _radius + j, _mobile.y);
    gl4dpLine(_mobile.x - _mobile.r, _mobile.y,  _mobile.x - _radius - j, _mobile.y);
  }
}