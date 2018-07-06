#include <assert.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <fftw3.h>
#include <GL4D/gl4dh.h>
#include "audioHelper.h"

typedef struct mobile_t mobile_t;
struct mobile_t {
  double x, y;
  float color[3];
  float r;
  int alive;
};

static void         init(int w, int h);
static void         draw(void);
static void         quit(void);
static void         mobileIntersect(void);
static void         mobileInit(void);
static void         mobile2texture(float * f);
static int          distance(int x0, int y0, int x1, int y1);
static int          bestMove(mobile_t mobile);
static void         mobileMove(void);

enum { NOTHING, LEFT, LEFT_UP, UP, RIGHT_UP, RIGHT, RIGHT_DOWN, DOWN, LEFT_DOWN, NDIR };
enum { EYES10, EYES11, EYES20, EYES21, NEYES };


static int _hasInit = 0;
static int _w = 1, _h = 1;
static GLuint _pId = 0;
static GLuint _tId = 0;
static GLuint _screen = 0;

static mobile_t * _mobile = NULL;
static const int _nb_mobiles = 400;
static float _pos_mobile[8][2];

static float _basses = 0;
static float _aigus = 0;
static int _state = 0;
static GLuint _quad = 0;
static int _voronoi = 0;

static int _dirx[] = { 0, -1, -1, 0, 1, 1,  1,  0, -1 };
static int _diry[] = { 0,  0,  1, 1, 1, 0, -1, -1, -1 };

static void mobileInit(void) {
  int i;
  srand(time(NULL));
  if(_mobile) {
    free(_mobile);
    _mobile = NULL;
  }
  if(!_hasInit) {
    atexit(quit);
    _hasInit = 1;
  }
  _mobile = malloc(_nb_mobiles * sizeof *_mobile);
  assert(_mobile);

  _pos_mobile[0][0] = _w/2; _pos_mobile[0][1] = _h;
  _pos_mobile[1][0] = _w/1.7; _pos_mobile[1][1] = _h-100;
  _pos_mobile[2][0] = 0;      _pos_mobile[2][1] = _h-100;
  _pos_mobile[3][0] = _w/1.4; _pos_mobile[3][1] = 0;
  _pos_mobile[4][0] = 0; _pos_mobile[4][1] = 0;
  _pos_mobile[5][0] = _w/2; _pos_mobile[5][1] = 0;
  _pos_mobile[6][0] = _w; _pos_mobile[6][1] = _h;
  _pos_mobile[7][0] = _w/2;   _pos_mobile[7][1] = _h/3;


  // YEUX
  _mobile[EYES10].x = _mobile[EYES11].x = _w/2 - _h/4;
  _mobile[EYES20].x = _mobile[EYES21].x = _w/2 + _h/4;
  _mobile[EYES10].y = _mobile[EYES11].y = _mobile[EYES21].y = _mobile[EYES20].y = _h/2;
  _mobile[EYES11].r = _mobile[EYES21].r = 130; _mobile[EYES10].r = _mobile[EYES20].r = 50;
  _mobile[EYES11].color[0] = _mobile[EYES21].color[0] = 0.0;
  _mobile[EYES11].color[1] = _mobile[EYES21].color[1] = 0.0;
  _mobile[EYES11].color[2] = _mobile[EYES21].color[2] = 0.0;
  _mobile[EYES10].color[0] = _mobile[EYES20].color[0] = 1.0;
  _mobile[EYES10].color[1] = _mobile[EYES20].color[1] = 1.0;
  _mobile[EYES10].color[2] = _mobile[EYES20].color[2] = 1.0;
  _mobile[EYES10].alive = _mobile[EYES11].alive = _mobile[EYES21].alive = _mobile[EYES20].alive = 1;
  _mobile[NEYES].x = _pos_mobile[0][0];
  _mobile[NEYES].y = _pos_mobile[0][1];
  _mobile[NEYES].color[0] = gl4dmURand();
  _mobile[NEYES].color[1] = gl4dmURand();
  _mobile[NEYES].color[2] = gl4dmURand();
  _mobile[NEYES].r = 15;

  for(i = NEYES+1; i < _nb_mobiles; i++) {
    _mobile[i].r = 0;
    _mobile[i].x = gl4dmURand() * _w;
    _mobile[i].y = gl4dmURand() * _h;
    _mobile[i].color[0] = 3.0;
    _mobile[i].color[1] = 8.0;
    _mobile[i].color[2] = 100.0;
    _mobile[i].alive = 0;
  }

  mobileIntersect();
}

static void init(int w, int h) {
  _w = w; _h = h;
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  _pId = gl4duCreateProgram("<vs>shaders/voronoi.vs", "<fs>shaders/voronoi.fs", NULL);
  _quad = gl4dgGenQuadf();
  glGenTextures(1, &_tId);
  glBindTexture(GL_TEXTURE_1D, _tId);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, 1, 0, GL_RGBA, GL_FLOAT, NULL);
  glBindTexture(GL_TEXTURE_1D, 0);
  mobileInit();
}

static void mobile2texture(float * f) {
  int i;
  for(i = 0; i < _nb_mobiles; i++) {
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

static int distance(int x0, int y0, int x1, int y1) {
  return sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));
}

static int bestMove(mobile_t mobile) {
  int i, cur, move = (int)(_w * _h), dir = NOTHING;
  for(i = 0; i < NDIR; i++) {
    cur = distance(mobile.x + _dirx[i], mobile.y + _diry[i], _mobile[NEYES].x, _mobile[NEYES].y);
    if(cur < move) {
      move = cur;
      dir = i;
    }
  }
  return dir;
}

static void mobileMove(void) {
  int i, m;
  int dt = SDL_GetTicks();
  static int lf = 0.0;
  static int rtime = 0.0;
  static int curPos = 1;
  for(i = 0; i < NEYES; i++) {
    if((i == EYES20 && _mobile[i].alive) || 
       (i == EYES10 && _mobile[i].alive)) {
        m = bestMove(_mobile[i]);
        _mobile[EYES10].x += _dirx[m] * 20;
        _mobile[EYES20].x += _dirx[m] * 20;
        _mobile[EYES10].y += _diry[m] * 20;
        _mobile[EYES20].y += _diry[m] * 20;
    }
  }

  _mobile[EYES20].alive = _mobile[EYES10].alive = 0;

  if(dt/2000 != _state) {
    _state = dt/2000;
    _mobile[NEYES].x = _pos_mobile[curPos][0];
    _mobile[NEYES].y = _pos_mobile[curPos][1];
    _mobile[NEYES].color[0] = gl4dmURand();
    _mobile[NEYES].color[1] = gl4dmURand();
    _mobile[NEYES].color[2] = gl4dmURand();
    _mobile[EYES20].alive = _mobile[EYES10].alive = 1;
    _mobile[EYES10].x = _w/2 - _w/5;
    _mobile[EYES20].x = _w/2 + _w/5;
    _mobile[EYES10].y = _mobile[EYES20].y = _h/2;
    if(curPos >= 8) curPos = -1;
    rtime++;
    curPos++;
  }

  if(rtime >= 8) {
    _voronoi = 1;
  }
  lf = _basses;
}

static void draw(void) {
  GLfloat * f = malloc(_nb_mobiles * 8 * sizeof *f), step = 1.0 / (_nb_mobiles * 2);
  assert(f);
  glDisable(GL_DEPTH_TEST);
  glUseProgram(_pId);

  glClear(GL_COLOR_BUFFER_BIT);  

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_1D, _tId);
  mobileMove();
  mobile2texture(f);
  glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA32F, 2 * _nb_mobiles, 0, GL_RGBA, GL_FLOAT, f);
  free(f);
  glUniform1i(glGetUniformLocation(_pId, "voronoi"), _voronoi);
  glUniform1i(glGetUniformLocation(_pId, "mobiles"), 0);
  glUniform1f(glGetUniformLocation(_pId, "step"), step);

  gl4dgDraw(_quad);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_1D, 0);
  glUseProgram(0);
}


static void mobileIntersect(void) {
  int i, j;
  float dx, dy, d, de;
  for(i = NEYES; i < _nb_mobiles; i++) {
    for(j = 0; j < NEYES; j++) {
      dx = _mobile[i].x - _mobile[j].x;
      dy = _mobile[i].y - _mobile[j].y;
      d = _mobile[i].r + _mobile[j].r + 1;
      if((de = dx * dx + dy * dy) < d * d) {
        _mobile[i].color[0] = 0.0;
        _mobile[i].color[1] = 0.0;
        _mobile[i].color[2] = 0.0;
      }
    }
  }
}

static void quit(void) {
  if(_mobile) {
    free(_mobile);
    _mobile = NULL;
  }

  if(_tId) {
    glDeleteTextures(1, &_tId);
    _tId = 0;
  }

  if(_screen) {
    gl4dpSetScreen(_screen);
    gl4dpDeleteScreen();
    _screen = 0;
  }
}


void voronoi(int state) {
  Sint16 * s;
  switch(state) {
  case GL4DH_INIT:
    _screen = gl4dpInitScreen();
    init(gl4dpGetWidth(), gl4dpGetHeight());
    return;
  case GL4DH_FREE:
    quit();
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    s = (Sint16 *)ahGetAudioStream();
    _basses = ahGetAudioStreamFreq();
    _aigus = ahGetAudioStreamAigus();
    return;
  default:
    draw();
    return;
  }
}
