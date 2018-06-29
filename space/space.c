#include "space.h"

static int _w = 0, _h = 0;
static line * _line = NULL;
static int _nbLines = 300;

static GLuint _white = RGB(255, 255, 255);
static GLuint _black = RGB(0, 0, 0);

static mobile_t _mobile;


static float myRand(float max) {
  float rand = gl4dmURand() * max;
  float sgn = gl4dmURand() * 2.0;
  sgn = sgn > 1.0 ? 1.0 : -1.0;
  rand *= sgn;
  return rand;
}

void spaceInit(int w, int h) {
  int i;
  _w = w; _h = h;

  if(_line) {
    free(_line);
    _line = NULL;
  }

  _line = malloc(_nbLines * sizeof * _line);
  assert(_line);

  for(i = 0; i < _nbLines; i++) {
    _line[i].x = myRand(_w);
    _line[i].y = myRand(_h);
    _line[i].z = gl4dmURand() * _w;
    _line[i].pz = _line[i].z;
  }

  _mobile.x = _w/2;
  _mobile.y = _h/2;
  _mobile.r = 50;
  _mobile.c = _white;
}

static void lineMove(void) {
  int i;
  float speed = 3;

  for(i = 0; i < _nbLines; i++) {
    _line[i].z -= speed;
    if(_line[i].z < 1) {
      _line[i].z = _w;
      _line[i].x = myRand(_w);
      _line[i].y = myRand(_h);
      _line[i].pz = _line[i].z; 
    }
  }
}

static void mobileMove(float basses) {
  _mobile.r = (int)basses * 10;
}

void spaceMove(float basses) {
  mobileMove(basses);
  lineMove();
}

static void mobileDraw(void) {
  gl4dpSetColor(_mobile.c);
  gl4dpFilledCircle(_mobile.x, _mobile.y, _mobile.r);
}

static void spiralDraw(void) {
  static float angle = 0.008;
  const float maxRadius = 200.0;

  float i;
  float x, lx = x = _w/2;
  float y, ly = y = _h/2;
  float theta, radius;

  for(i = 0.0; i < _nbLines; i += 1.0) {
    theta = angle * i;
    radius = maxRadius * sqrt(i/_nbLines);
    x = _w/2 + radius * cos(theta);
    y = _h/2 + radius * sin(theta);
    gl4dpSetColor(_black);
    gl4dpLine(lx, ly, x, y);
    lx = x;
    ly = y;
  }

  angle += .008;
}

static void lineDraw(void) {
  int i;
  float sx, sy, px, py;
  gl4dpSetColor(_white); 
  for(i = 0; i < _nbLines; i++) {
    sx = (_line[i].x / _line[i].z) * _w + _w/2;
    sy = (_line[i].y / _line[i].z) * _h + _h/2;
    px = (_line[i].x / _line[i].pz) * _w + _w/2;
    py = (_line[i].y / _line[i].pz) * _h + _h/2;
    gl4dpLine(px, py, sx, sy);
  }
}

void spaceDraw(void) {
  mobileDraw();
  spiralDraw();
  lineDraw();
}

void spaceDelete(void) {
  if(_line) {
    free(_line);
    _line = NULL;
  }
}
