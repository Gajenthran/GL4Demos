#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4dp.h>
#include <SDL_image.h>
#include "sphere.h"

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

static float myRand(float max) {
  float rand = gl4dmURand() * max;
  float sgn = gl4dmURand() * 2.0;
  sgn = sgn > 1.0 ? 1.0 : -1.0;
  rand *= sgn;
  return rand;
}

void mobileInit(int n, int id, int w, int h) {
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
    _mobile[i].r = (!i ? 200.0 : 20.0);
    _mobile[i].x = (!i ? _w/2 : myRand(_w/2));
    _mobile[i].y = (!i ? _h/2 : myRand(_h/2));
    _mobile[i].id = gl4dmURand() * id + 1.0;
  }
}

void mobile2texture(float * f) {
  int i;
  for(i = 0; i < _nbMobiles; i++) {
    f[8 * i + 0] = _mobile[i].x; 
    f[8 * i + 1] = _mobile[i].y;
    f[8 * i + 2] = _mobile[i].r / ((GLfloat)MIN(_w, _h));
    f[8 * i + 3] = _mobile[i].id;
  }
}

void mobileMove(void) {
  int i;
  for(i = 1; i < _nbMobiles; i++) {
    _mobile[i].y -= 0.01;
  }
}
