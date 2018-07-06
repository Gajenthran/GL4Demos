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
  double x, y, vx, vy;
  GLuint c;
  float r;
};

enum {
  FREQ_MOY = 5
};

static void quit(void);
static void init(int w, int h);
static void grow(void);
static void move(void);
static void mobileDraw(void);
static void draw(void);

static int _w = 1, _h = 1;
static GLuint _screen = 0;
static int _hasInit = 0;
static GLuint _screenColor = RGB(255, 255, 255);

static mobile_t * _mobile = NULL;
static int _nb_mobiles = 30;
static int _state = 0;
static int _rad = 20.0, _r = 20.0;
static GLfloat _basses = 0;

static void quit(void) {
  if(_mobile) {
    free(_mobile);
    _mobile = NULL;
  }

  if(_screen) {
    gl4dpSetScreen(_screen);
    gl4dpDeleteScreen();
    _screen = 0;
  }
}

void init(int w, int h) {
  int i;
  _w = w; _h = h;
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

  for(i = 0; i < _nb_mobiles; i++) {
    _mobile[i].x = _w/2;
    _mobile[i].y = _h/2;
    if(!(i%2))
      _mobile[i].c = RGB(255, 255, 255);
    else
      _mobile[i].c = RGB(0, 0, 0);
    _mobile[i].r = 0.0;
  }
}

static void grow(void) {
  _mobile[_state++].r = _rad;
  _mobile[_state++].r = _rad + 2;
  _rad = _rad + _r;
}

static void move(void) {
  if(_state+1 >= _nb_mobiles) {
    return;
  }

  static GLfloat lf = FREQ_MOY;
  static float r = 1.0;
  int i;

  if(_state < 2) {
    grow();
    lf = _basses;
    return;
  }

  if(_basses > FREQ_MOY && lf < FREQ_MOY) {
    if(_state >= 2 && _state < 4) {
      grow();
      lf = _basses;
      return;
    }

    if(_state >= 4 && _state < 6) {
      grow();
      lf = _basses;
      return;
    }

    if(_state >= 6 && _state < 8) {
      grow();
      return;
    }

    if(_state >= 8 && _state < 10) {
      _mobile[1].r = _rad - _r * 2;
      _state += 2;
      lf = _basses;
      return;
    }

    if(_state >= 10 && _state < 12) {
      _screenColor = RGB(0, 0, 0);
      _state += 2;
      lf = _basses;
      return;
    }

    if(_state >= 12 && _state < 14) {
      grow();
      lf = _basses;
      return;
    }

    if(_state >= 14 && _state < 16) {
      grow();
      lf = _basses;
      return;
    }
  }

  if(_state >= 16 && _state < 18) {
    if(_mobile[_nb_mobiles-1].r + _mobile[_nb_mobiles-1].x > _w) {
      _mobile[_nb_mobiles-1].r = 0;
    }

    _r += 0.1;
    _mobile[_nb_mobiles-1].r += _r;
    _mobile[_nb_mobiles-1].r += (r + 1);
    _mobile[_nb_mobiles-1].c = RGB(255, 255, 255);
    for(i = 0; i < _state; i++) {
      _mobile[i].r += 1;
    }

    return;
  }

  lf = _basses;
}

static void mobileDraw(void) {
  int i;
  for(i = _nb_mobiles-2; i > 0; i--) {
    gl4dpSetColor(_mobile[i].c);
    if((i%2))
      gl4dpFilledCircle(_mobile[i].x, _mobile[i].y, _mobile[i].r);
    else
      gl4dpFilledCircle(_mobile[i].x, _mobile[i].y, _mobile[i].r);
  }

  gl4dpSetColor(_mobile[i].c);
  gl4dpCircle(_mobile[_nb_mobiles-1].x, _mobile[_nb_mobiles-1].y, _mobile[_nb_mobiles-1].r);

}

static void draw(void) {
  gl4dpClearScreenWith(_screenColor);
  move();
  mobileDraw();
}


void growCircle(int state) {
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

