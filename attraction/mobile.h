#ifndef MOBILE_H
#define MOBILE_H

#include <assert.h>
#include <GL4D/gl4duw_SDL2.h>

typedef struct mobile_t mobile_t;
struct mobile_t {
  double x, y, vx, vy;
  float color[3];
  float r;
  int alive;
};

extern void mobileIntersect(void);
extern void mobile2texture(float * f);
extern void mobileInit(int n, int w, int h);
extern void mobileMove(void);
extern void mobileClean(void);

#endif
