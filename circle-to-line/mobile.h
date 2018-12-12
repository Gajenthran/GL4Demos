#ifndef _MOBILE_H
#define _MOBILE_H

#include <GL4D/gl4dp.h>
#include <GL4D/gl4duw_SDL2.h>
#include <assert.h>
#include <math.h>

typedef struct mobile_t mobile_t;
struct mobile_t {
  float x, y, r;
  GLuint c;
};

extern void mobileInit(float rad);
extern void mobileToLine(void);
extern void mobileDraw(void);


#endif
