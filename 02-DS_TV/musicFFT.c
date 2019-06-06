#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_mixer.h>
#include <fftw3.h>
#include <GL4D/gl4dh.h>
#include "audioHelper.h"

static void init(int w, int h);
static void audio(void);
static void draw(void);
static void drawPixelWithThickness(int x, int y, int t);
static void quit(void);

#define ECHANTILLONS 1024

typedef struct mobile_t mobile_t;
struct mobile_t {
  float x, y, r;
  GLuint c;
};

static Sint16 _hauteurs[ECHANTILLONS];
static GLuint _screen = 0;
static int _basses = 0;
static int _gap = 0, _state = 0, _radius;
static int _w, _h;
static mobile_t _mobile;
static Uint32 white = RGB(255, 255, 255);


static fftw_complex * _in4fftw = NULL, * _out4fftw = NULL;
static fftw_plan _plan4fftw = NULL;

static int _color[6][3] = {
  {128, 214, 243},
  {19, 74, 207},
  {235, 88, 203},
  {31, 87, 211},
  {97, 28, 62},
  {142, 247, 251}
};

static void init(int w, int h) {
  _w = w; _h = h;
  /* préparation des conteneurs de données pour la lib FFTW */
  _in4fftw   = fftw_malloc(ECHANTILLONS *  sizeof *_in4fftw);
  memset(_in4fftw, 0, ECHANTILLONS *  sizeof *_in4fftw);
  assert(_in4fftw);
  _out4fftw  = fftw_malloc(ECHANTILLONS * sizeof *_out4fftw);
  assert(_out4fftw);
  _plan4fftw = fftw_plan_dft_1d(ECHANTILLONS, _in4fftw, _out4fftw, FFTW_FORWARD, FFTW_ESTIMATE);
  assert(_plan4fftw);
  _mobile.x = _w/2;
  _mobile.y = _h/2;
  _mobile.r = _radius = 100;
  _mobile.c = RGB(255, 255, 255);
}

static void drawPixelWithThickness(int x, int y, int t) {
  int i;
  for(i = 0; i < t; i++) {
    gl4dpPutPixel(x+i, y+i);
    gl4dpPutPixel(x, y);
    gl4dpPutPixel(x-i, y-i);
  }
}

static void circleToLine(void) {
  _mobile.r = _radius - _gap;
  if(_mobile.r > 0.0)
    _gap += 1.5;
  int j;
  gl4dpFilledCircle(_mobile.x, _mobile.y, _mobile.r);

  for(j = 0; j < _gap; j++) {
    gl4dpLine(_mobile.x + _mobile.r, _mobile.y, _mobile.x + _radius + j, _mobile.y);
    gl4dpLine(_mobile.x - _mobile.r, _mobile.y,  _mobile.x - _radius - j, _mobile.y);
  }
}

static void circleExtremities(void) {
  srand(time(NULL));
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  gl4dpCircle(_w/2, _h/2, _mobile.r);
  int it = 0;
  for(double i = 0; i < 2 * M_PI; i += 2 * M_PI / ECHANTILLONS) {
    if(it & 16) { it++; continue; }
    int x0 = _mobile.r * cos(i);
    int y0 = _mobile.r * sin(i);
    int x1 = (_mobile.r + _basses * 8) * cos(i);
    int y1 = (_mobile.r + _basses * 8) * sin(i);
    gl4dpLine(x0 + gl4dpGetWidth()/2, 
              y0 + gl4dpGetHeight()/2, 
              x1 + gl4dpGetWidth()/2, 
              y1 + gl4dpGetHeight()/2);
    it++;
  }
}


static void draw(void) {
  int i;
  static int t0 = 0, t, dt;
  t = SDL_GetTicks();
  dt = (t - t0) / 1000.0;

  gl4dpSetScreen(_screen);
  gl4dpClearScreenWith(RGB(0, 0, 0));
  gl4dpSetColor(white);

  if(_state == 0 && dt >= 5) {
    _state++;
    t0 = t;
  } else if(_state == 1 && _mobile.r <= 0) {
    _state++;
  }

  if(_state == 0) {
    circleExtremities();
  } else if(_state == 1) {
    circleToLine();
  } else {
    if(_mobile.r <= 0.0) {
      for(i = 0; i < ECHANTILLONS; ++i) {
        int x0, y0;
        x0 = (i * (_w - 1)) / (ECHANTILLONS - 1);
        y0 = _hauteurs[i];
        drawPixelWithThickness(x0 + _w/2, y0 + _h/2, 3);
      }
    }
  }
  gl4dpUpdateScreen(NULL);
}

static void audio(void) {
  if(_plan4fftw) {
    int i, j, l = MIN(ahGetAudioStreamLength() >> 1, ECHANTILLONS);
    Sint16 *d = (Sint16 *)ahGetAudioStream();
    _basses = ahGetAudioStreamFreq();
    for(i = 0; i < l; i++)
      _in4fftw[i][0] = d[i] / ((1 << 15) - 1.0);
    fftw_execute(_plan4fftw);
    for(i = 0; i < l >> 2; i++) {
      _hauteurs[4 * i] = (int)(sqrt(_out4fftw[i][0] * _out4fftw[i][0] + _out4fftw[i][1] * _out4fftw[i][1]) * exp(2.0 * i / (double)(l / 4.0)));
      for(j = 1; j < 4; j++)
        _hauteurs[4 * i + j] = MIN(_hauteurs[4 * i], 255);
    }
  }
}

static void quit(void) {
  if(_screen) {
    gl4dpSetScreen(_screen);
    gl4dpDeleteScreen();
    _screen = 0;
  }
}

void musicFFT(int state) {
  switch(state) {
  case GL4DH_INIT:
    _screen = gl4dpInitScreen();
    init(gl4dpGetWidth(), gl4dpGetHeight());
    return;
  case GL4DH_FREE:
    quit();
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    audio();
    return;
  default:
    draw();
    return;
  }
}
