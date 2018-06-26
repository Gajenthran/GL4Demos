#ifndef _TRIANGLE_H
#define _TRIANGLE_H

#include <GL4D/gl4dp.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include <assert.h>

typedef struct point point;
typedef struct triangle triangle;

struct point {
	int x, y;
};

struct triangle {
	point a, b, c;
};

extern void triangleInit(int n);
extern void triangleMove(void);
extern void triangleDraw(void);
extern void triangleClean(void);


#endif
