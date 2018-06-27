#ifndef _MOBILE_H
#define _MOBILE_H

#include <GL4D/gl4dp.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include <assert.h>
#include <math.h>

typedef struct mobile_t mobile_t;
struct mobile_t {
  float x, y, r;
  GLuint c;
};

extern void mobileInit(void);
extern void mobileMove(void);
extern void mobileDraw(void);

#endif
