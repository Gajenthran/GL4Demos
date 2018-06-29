#ifndef _LINE_H
#define _LINE_H

#include <stdio.h>
#include <stdlib.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_mixer.h>
#include <fftw3.h>
#include <assert.h>
#include <math.h>

typedef struct mobile_t mobile_t;
struct mobile_t {
  int x, y;
  GLfloat r;
  GLuint c;
};

typedef struct line line;
struct line {
  float x, y, z, pz;
  GLuint c;
};

extern void spaceInit(int w, int h);
extern void spaceDelete(void);
extern void spaceMove(float basses);
extern void spaceDraw(void);

#endif
