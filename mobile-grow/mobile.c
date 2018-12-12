#include <time.h>
#include <assert.h>
#include "mobile.h"

static mobile_t * _mobile = NULL;
static int _nbMobiles = 0;
static int _curMobile = 0;

void mobileInit(int n) {
  srand(time(NULL));
  int i;
  _nbMobiles = n;

  if(_mobile) {
    free(_mobile);
    _mobile = NULL;
  }

  _mobile = malloc(_nbMobiles * sizeof *_mobile);
  assert(_mobile);
  for(i = 0; i < _nbMobiles; i++) {
    _mobile[i].r = 1;
    do {
      _mobile[i].x = gl4dmURand() * gl4dpGetWidth();
      _mobile[i].y = gl4dmURand() * gl4dpGetHeight();
    } while (gl4dpGetPixel(_mobile[i].x, _mobile[i].y) != 0);
    _mobile[i].c = RGB(255, 255, 255);
    _mobile[i].alive = 1;
  }
}

static void mobileIntersect(double dt) {
  int i, j;
  float dx, dy, d, de;
  if(_curMobile < _nbMobiles)
    _curMobile++;
  for(i = 0; i < _curMobile; i++) {
    for(j = i + 1; j < _curMobile; j++) {
      dx = _mobile[i].x - _mobile[j].x;
      dy = _mobile[i].y - _mobile[j].y;
      d = _mobile[i].r + _mobile[j].r + 1;
      if((de = dx * dx + dy * dy) < d * d) {
        _mobile[i].alive = 0;
        _mobile[j].alive = 0;
      }
    }
  }
}

void mobileGrow(void) {
  int i;
  static Uint32 t0 = 0;
  Uint32 t = SDL_GetTicks();
  double dt = (t - t0) / 1000.0;
  t0 = t;

  mobileIntersect(dt);
  for(i = 0; i < _curMobile; i++) {
    if((!(_mobile[i].x + _mobile[i].r > gl4dpGetWidth() ||
       _mobile[i].x - _mobile[i].r < 0 ||
       _mobile[i].y + _mobile[i].r > gl4dpGetHeight() ||
       _mobile[i].y - _mobile[i].r < 0)) &&
       _mobile[i].alive)
      _mobile[i].r += 0.5;
  }
}

void mobileDraw(void) {
  int i;
  for(i = 0; i < _curMobile; i++) {
    gl4dpSetColor(_mobile[i].c);
    gl4dpFilledCircle(_mobile[i].x, _mobile[i].y, _mobile[i].r);
  }
}

void mobileDelete(void) {
  if(_mobile) {
    free(_mobile);
    _mobile = NULL;
  }
  _nbMobiles = 0;
}
