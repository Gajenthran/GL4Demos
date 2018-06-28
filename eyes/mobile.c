#include "mobile.h"

enum { NOTHING, LEFT, LEFT_UP, UP, RIGHT_UP, RIGHT, RIGHT_DOWN, DOWN, LEFT_DOWN, NDIR };
enum { EYES10, EYES11, EYES20, EYES21, NEYES };

static mobile_t * _mobiles = NULL;
static int _nbMobiles = 5;
static int _dirx[] = { 0, -1, -1, 0, 1, 1,  1,  0, -1 };
static int _diry[] = { 0,  0,  1, 1, 1, 0, -1, -1, -1 };
static GLuint _white = RGB(255, 255, 255);
static GLuint _black = RGB(0, 0, 0);

void mobileInit(void) {
  srand(time(NULL));
  GLubyte r, g, b;
  r = rand()&0xFF; g = rand()&0xFF; b = rand()&0xFF;

  if(_mobiles) {
    free(_mobiles);
    _mobiles = NULL;
  }
  _mobiles = malloc(_nbMobiles * sizeof *_mobiles);
  assert(_mobiles);

  _mobiles[EYES10].x = _mobiles[EYES11].x = gl4dpGetWidth()/2 - gl4dpGetWidth()/5;
  _mobiles[EYES20].x = _mobiles[EYES21].x = gl4dpGetWidth()/2 + gl4dpGetWidth()/5;
  _mobiles[EYES10].y = _mobiles[EYES11].y = _mobiles[EYES21].y = _mobiles[EYES20].y = gl4dpGetHeight()/2;
  _mobiles[EYES10].r = _mobiles[EYES20].r = 100; _mobiles[EYES11].r = _mobiles[EYES21].r = 50;
  _mobiles[EYES10].c = _mobiles[EYES20].c = _white; 
  _mobiles[EYES11].c = _mobiles[EYES21].c = _black;
  _mobiles[EYES10].alive = _mobiles[EYES11].alive = _mobiles[EYES21].alive = _mobiles[EYES20].alive = 1;

  _mobiles[NEYES].x = gl4dmURand() * gl4dpGetWidth();
  _mobiles[NEYES].y = gl4dmURand() * gl4dpGetHeight();
  _mobiles[NEYES].r = (gl4dpGetWidth() / 40.0f) * (0.2f + 0.8f * rand() / (RAND_MAX + 1.0)) * 0.5;
  _mobiles[NEYES].c = RGB(r, g, b);
  _mobiles[NEYES].alive = 0;
}

static int distance(int x0, int y0, int x1, int y1) {
  return sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));
}

static int bestMove(mobile_t mobile) {
  int i, cur, move = (int)(gl4dpGetWidth() * gl4dpGetHeight()), dir = NOTHING;
  for(i = 0; i < NDIR; i++) {
    cur = distance(mobile.x + _dirx[i], mobile.y + _diry[i], _mobiles[NEYES].x, _mobiles[NEYES].y);
    if(cur < move) {
      move = cur;
      dir = i;
    }
  }
  return dir;
}

void mobileMove(void) {
  static int t0 = 0;
  int i, m, t;
  t = SDL_GetTicks();
  for(i = 0; i < NEYES; i++) {
    if((i == EYES21 && _mobiles[i].alive) || 
       (i == EYES11 && _mobiles[i].alive)) {
        m = bestMove(_mobiles[i]);
        _mobiles[i].x += _dirx[m] * 20;
        _mobiles[i].y += _diry[m] * 20;
    }
  }

  _mobiles[EYES21].alive = _mobiles[EYES11].alive = 0;
  if(t/1000 != t0/1000) {
    _mobiles[NEYES].x = gl4dmURand() * gl4dpGetWidth();
    _mobiles[NEYES].y = gl4dmURand() * gl4dpGetHeight();
    _mobiles[EYES21].alive = _mobiles[EYES11].alive = 1;
    _mobiles[EYES11].x = gl4dpGetWidth()/2 - gl4dpGetWidth()/5;
    _mobiles[EYES21].x = gl4dpGetWidth()/2 + gl4dpGetWidth()/5;
    _mobiles[EYES11].y = _mobiles[EYES21].y = gl4dpGetHeight()/2;
    t0 = t;
  }
}

void mobileDraw(void) {
  int i;
  for(i = 0; i < _nbMobiles; i++) {
    gl4dpSetColor(_mobiles[i].c);
    gl4dpFilledCircle(_mobiles[i].x, _mobiles[i].y, _mobiles[i].r);
  }
}

void mobileDelete(void) {
  if(_mobiles) {
    free(_mobiles);
    _mobiles = NULL;
    _nbMobiles = 0;
  }
}