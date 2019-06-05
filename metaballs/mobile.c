#include "mobile.h"

enum { NOTHING, LEFT, LEFT_UP, UP, RIGHT_UP, RIGHT, RIGHT_DOWN, DOWN, LEFT_DOWN, NDIR };

static int _dirx[] = { 0, -1, -1, 0, 1, 1,  1,  0, -1 };
static int _diry[] = { 0,  0,  1, 1, 1, 0, -1, -1, -1 };
static mobile_t * _mobile = NULL;
static int _nbMobiles = 0;
static int _w = 1, _h = 1;

void mobileClean(void) {
  _nbMobiles = 0;
  if(_mobile) {
    free(_mobile);
    _mobile = NULL;
  }
}

void mobileInit(int n, int w, int h) {
  srand(time(NULL));
  int i;
  _w = w; _h = h;
  _nbMobiles = n;

  if(_mobile) {
    free(_mobile);
    _mobile = NULL;
  }

  _mobile = malloc(_nbMobiles * sizeof *_mobile);
  assert(_mobile);

  for(i = 0; i < _nbMobiles; i++) {
    _mobile[i].r = (!i ? 50.0 : 10.0);
    _mobile[i].x = (!i ? _w/2 : gl4dmURand() * _w);
    _mobile[i].y = (!i ? _h/2 : gl4dmURand() * _h);
    _mobile[i].color[0] = gl4dmURand();
    _mobile[i].color[1] = gl4dmURand();
    _mobile[i].color[2] = gl4dmURand();
    _mobile[i].alive = 1;
  }
}

void mobile2texture(float * f) {
  int i;
  for(i = 0; i < _nbMobiles; i++) {
    f[8 * i + 0] = _mobile[i].color[0];
    f[8 * i + 1] = _mobile[i].color[1];
    f[8 * i + 2] = _mobile[i].color[2];
    f[8 * i + 3] = 1;
    f[8 * i + 4] = _mobile[i].x / _w;
    f[8 * i + 5] = _mobile[i].y / _h;
    f[8 * i + 6] = _mobile[i].r / ((GLfloat)MIN(_w, _h));
    f[8 * i + 7] = 1;
  }
}

int distance(int x0, int y0, int x1, int y1) {
  return sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));
}

int bestMove(mobile_t mobile) {
  int i, cur, move = (int)(_w * _h), dir = NOTHING;
  for(i = 0; i < NDIR; i++) {
    cur = distance(mobile.x + _dirx[i], mobile.y + _diry[i], _mobile[0].x, _mobile[0].y);
    if(cur < move) {
      move = cur;
      dir = i;
    }
  }
  return dir;
}

int noMobiles(void) {
  int i;
  for(i = 1; i < _nbMobiles; i++) {
    if(_mobile[i].alive)
      return 0;
  }
  return 1;
}

void mobileMove(void) {
  int i, m;
  float dx, dy, d, de;
  for(i = 1; i < _nbMobiles; i++) {
    m = bestMove(_mobile[i]);
    _mobile[i].x += _dirx[m] * 2;
    _mobile[i].y += _diry[m] * 2;

    dx = _mobile[i].x - _mobile[0].x;
    dy = _mobile[i].y - _mobile[0].y;
    d = _mobile[i].r + _mobile[0].r+1;
    if( (de = dx * dx + dy * dy) < d * d && _mobile[i].alive) {
      _mobile[0].r += _mobile[i].r * 0.04;
    }

    if(noMobiles())
      _mobile[0].r += 0.005;
  }
}
