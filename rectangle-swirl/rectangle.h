#ifndef _RECTANGLE_H
#define _RECTANGLE_H

#include <GL4D/gl4dp.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include <assert.h>

typedef struct point point;
typedef struct rectangle rectangle;

struct point {
	int x, y;
};

struct rectangle {
	point a, b, c, d;
	GLuint color;
};

extern void rectangleInit(int n);
extern void rectangleMove(void);
extern void rectangleDraw(void);
extern void rectangleClean(void);


#endif
