#ifndef _CERCLE_H
#define _CERCLE_H

#include <GL4D/gl4dp.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include <assert.h>

typedef struct mobile_t mobile_t;
struct mobile_t {
  float x, y, r;
  float color[3];
  int alive;
  GLuint c;
};

extern void mobileInit(int n);
extern void mobileGrow(void);
extern void mobileDraw(void);
extern void mobileDelete(void);

#endif
