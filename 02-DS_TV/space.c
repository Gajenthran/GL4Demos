#include <assert.h>
#include <stdio.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4df.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include <GL4D/gl4dh.h>
#include <GL4D/gl4dp.h>
#include "audioHelper.h"


typedef struct mobile_t mobile_t;
struct mobile_t {
  int x, y;
  GLfloat r;
  GLuint c;
};

typedef struct point point;
typedef struct triangle triangle;

struct point {
  int x, y;
};

struct triangle {
  point a, b, c;
};

typedef struct line line;

struct line {
  float x, y, z, pz;
  GLuint c;
};

static float  myRand(float max);
static void   quit(void);
static void   init(int w, int h);
static void   lineMove(void);
static void   drawLineWithThickness(int x0, int y0, int x1, int y1, int t);
static void   lineDraw(void);
static void   triangleMove(void);
static void   triangleDraw(void);
static void   draw(void);

enum {
  FREQ_MOY = 5, FREQ_H = 12
};

// Variables globales concernant la fenêtre
static int _w = 1, _h = 1;
static GLuint _screen = 0;
static int _hasInit = 0;
static GLuint _screenColor = RGB(0, 0, 0);
static float _basses = 0;

// Variables globales concernant la première démo : space
static line * _line = NULL;
static int _nb_lines = 300;
static float _speed = 0.0;
static float _lspeed = .2;
static float _angle = 0.0;
static float _langle = .008;
static float _maxRad = 200.0;
static int _active = 0;

// Variables globales concernant la deuxième démo : triangle + circle
static triangle _triangle;
static mobile_t _mobile[2];
static int _nb_mobiles = 2;

static float myRand(float max) {
  float rand = gl4dmURand() * max;
  float sgn = gl4dmURand() * 2.0;
  sgn = sgn > 1.0 ? 1.0 : -1.0;
  rand *= sgn;
  return rand;
}

static void init(int w, int h) {
  int i;
  _w = w; _h = h;

  if(_line) {
    free(_line);
    _line = NULL;
  }

  if(!_hasInit) {
    atexit(quit);
    _hasInit = 1;
  }

  _line = malloc(_nb_lines * sizeof * _line);
  assert(_line);
  for(i = 0; i < _nb_lines; i++) {
    GLubyte r, g, b;
    _line[i].x = myRand(_w);
    _line[i].y = myRand(_h);
    _line[i].z = gl4dmURand() * _w;
    _line[i].pz = _line[i].z;
    r = rand()&0xFF; g = rand()&0xFF; b = rand()&0xFF;
    _line[i].c = RGB(r, g, b);
  }

  _triangle.a.x = _w/2 - 1; _triangle.a.y = _h/2;
  _triangle.b.x = _w/2 + 1; _triangle.b.y = _h/2;
  _triangle.c.x = _w/2; _triangle.c.y = _h/2 + 1;

  for(i = 0; i < _nb_mobiles; i++) {
    _mobile[i].x = (i ? _w/2 - _h/5 : _w/2 + _h/5);
    _mobile[i].y = _h/2;
    _mobile[i].r = 0;
    _mobile[i].c = RGB(255, 255, 255);
  }
}

static void drawLineWithThickness(int x0, int y0, int x1, int y1, int t) {
  int i;
  for(i = 0; i < t; i++) {
    gl4dpLine(x0 - i, y0 - i, x1 - i, y1 - i);
    gl4dpLine(x0, y0, x1, y1);
    gl4dpLine(x0 + i, y0 + i, x1 + i, y1 + i);
  }
}

static void triangleMove(void) {
  _triangle.a.x -= 15; _triangle.a.y -= 15;
  _triangle.b.x += 15; _triangle.b.y -= 15;
  _triangle.c.y += 15;

  if((_triangle.a.x < 0 || _triangle.a.y < 0 ||
    _triangle.b.x > _w || _triangle.b.y < 0 ||
    _triangle.c.y > _h) && _active == 1) {
    _active = 2;
  }
}

static void triangleDraw(void) {
  gl4dpSetColor(RGB(255,255,255));
  drawLineWithThickness(_triangle.a.x, _triangle.a.y, _triangle.b.x, _triangle.b.y, 5);
  drawLineWithThickness(_triangle.b.x, _triangle.b.y, _triangle.c.x, _triangle.c.y, 5);
  drawLineWithThickness(_triangle.c.x, _triangle.c.y, _triangle.a.x, _triangle.a.y, 5);
}

static void lineMove(void) {
  static 
  int i;
  for(i = 0; i < _nb_lines; i++) {
    _line[i].z -= _speed;
    if(_line[i].z < 1) {
      _line[i].z = _w;
      _line[i].x = myRand(_w);
      _line[i].y = myRand(_h);
      _line[i].pz = _line[i].z; 
    }
  }

  if(_speed > 5) {
    _speed -= 0.5;
  }
  _speed += _lspeed;
}

static void lineDraw(void) {
  static float lf = 0.0;
  int i;
  float sx, sy, px, py;
  gl4dpSetColor(RGB(255, 255, 255)); 
  for(i = 0; i < _nb_lines; i++) {
    sx = (_line[i].x / _line[i].z) * _w + _w/2;
    sy = (_line[i].y / _line[i].z) * _h + _h/2;
    px = (_line[i].x / _line[i].pz) * _w + _w/2;
    py = (_line[i].y / _line[i].pz) * _h + _h/2;
    gl4dpLine(px, py, sx, sy);
  }

  int r = (int)_basses * 10;
  gl4dpFilledCircle(_w/2, _h/2, r);
  if(lf < FREQ_MOY && _basses > FREQ_H && _active == 0)
    _active = 1;

  if(lf < FREQ_MOY && _basses > FREQ_H && _active == 2)
    _active = 3;

  _angle += _langle;
  float ip;
  float x, lx = x = _w/2;
  float y, ly = y = _h/2;
  float theta, radius;
  gl4dpSetColor(RGB(255, 255, 255));

  for(ip = 0.0; ip < _nb_lines; ip += 1.0) {
    theta = _angle * ip;
    radius = _maxRad * sqrt(ip/_nb_lines);
    x = _w/2 + radius * cos(theta);
    y = _h/2 + radius * sin(theta);
    gl4dpSetColor(RGB(2, 2, 2));
    gl4dpLine(lx, ly, x, y);
    lx = x;
    ly = y;

  }

  lf = _basses;
}

static void quit(void) {
  if(_line) {
    free(_line);
    _line = NULL;
  }

  if(_screen) {
    gl4dpSetScreen(_screen);
    gl4dpDeleteScreen();
    _screen = 0;
  }
}

static void draw(void) {
  gl4dpClearScreenWith(_screenColor);
  lineMove();
  lineDraw();
  if(_active) {
    triangleMove();
    triangleDraw();
  }
}


void space(int state) {
  Sint16 * s;
  switch(state) {
    case GL4DH_INIT:
        _screen = gl4dpInitScreen();
        init(gl4dpGetWidth(), gl4dpGetHeight());
      return;
    case GL4DH_FREE:
        quit();
        gl4dpSetScreen(_screen);
        gl4dpDeleteScreen();
      return;
    case GL4DH_UPDATE_WITH_AUDIO:
      s = (Sint16 *)ahGetAudioStream();
      _basses = ahGetAudioStreamFreq();
      return;
    default:
      draw();
      gl4dpUpdateScreen(NULL);
      return;
  }
}

