#include "rectangle.h"
#include <assert.h>

static rectangle * _rectangles = NULL;
static int _nbRectangles = 0;

void rectangleInit(int n) {
  int i;
  GLubyte r, g, b;
  _nbRectangles = n;
  if(_rectangles) {
    free(_rectangles);
    _rectangles = NULL;
  }

  _rectangles = malloc(_nbRectangles * sizeof *_rectangles);
  assert(_rectangles);

  for(i = 0; i < _nbRectangles; i++){
    _rectangles[i].a.x = 1; _rectangles[i].a.y = gl4dpGetHeight() - 1;
    _rectangles[i].b.x = gl4dpGetWidth() - 1; _rectangles[i].b.y = gl4dpGetHeight() - 1;
    _rectangles[i].c.x = gl4dpGetWidth() - 1; _rectangles[i].c.y = 1;
    _rectangles[i].d.x = 1; _rectangles[i].d.y = 1;
    r = rand()&0xFF; g = rand()&0xFF; b = rand()&0xFF;
    _rectangles[i].color = RGB(r, g, b);
  }
}

void rectangleRestart(void) {
  int i;
  GLubyte r, g, b;
  for(i = 0; i < _nbRectangles; i++){
    _rectangles[i].a.x = 1; _rectangles[i].a.y = gl4dpGetHeight() - 1;
    _rectangles[i].b.x = gl4dpGetWidth() - 1; _rectangles[i].b.y = gl4dpGetHeight() - 1;
    _rectangles[i].c.x = gl4dpGetWidth() - 1; _rectangles[i].c.y = 1;
    _rectangles[i].d.x = 1; _rectangles[i].d.y = 1;
    r = rand()&0xFF; g = rand()&0xFF; b = rand()&0xFF;
    _rectangles[i].color = RGB(r, g, b);
  }
}

void rectangleMove(void) {
  static int i = 1;
  if(i > _nbRectangles) {
    rectangleRestart();
    i = 1;
    return;
  }
  _rectangles[i].a.x = (((_rectangles[i-1].a.x + _rectangles[i-1].b.x)/2) + _rectangles[i-1].a.x) / 2;
  _rectangles[i].a.y = (((_rectangles[i-1].a.y + _rectangles[i-1].b.y)/2) + _rectangles[i-1].a.y) / 2;

  _rectangles[i].b.x = (((_rectangles[i-1].b.x + _rectangles[i-1].c.x)/2) + _rectangles[i-1].b.x) / 2;
  _rectangles[i].b.y = (((_rectangles[i-1].b.y + _rectangles[i-1].c.y)/2) + _rectangles[i-1].b.y) / 2;

  _rectangles[i].c.x = (((_rectangles[i-1].c.x + _rectangles[i-1].d.x)/2) + _rectangles[i-1].c.x) / 2;
  _rectangles[i].c.y = (((_rectangles[i-1].c.y + _rectangles[i-1].d.y)/2) + _rectangles[i-1].c.y) / 2;

  _rectangles[i].d.x = (((_rectangles[i-1].d.x + _rectangles[i-1].a.x)/2) + _rectangles[i-1].d.x) / 2;
  _rectangles[i].d.y = (((_rectangles[i-1].d.y + _rectangles[i-1].a.y)/2) + _rectangles[i-1].d.y) / 2;

  i++;
}

void rectangleDraw(void) {
  int i;
  for(i = 0; i < _nbRectangles; i++) {
    gl4dpSetColor(_rectangles[i].color);
    gl4dpLine(_rectangles[i].a.x, _rectangles[i].a.y, _rectangles[i].b.x, _rectangles[i].b.y);
    gl4dpLine(_rectangles[i].b.x, _rectangles[i].b.y, _rectangles[i].c.x, _rectangles[i].c.y);
    gl4dpLine(_rectangles[i].c.x, _rectangles[i].c.y, _rectangles[i].d.x, _rectangles[i].d.y);
    gl4dpLine(_rectangles[i].d.x, _rectangles[i].d.y, _rectangles[i].a.x, _rectangles[i].a.y);
  }
}

void rectangleClean(void) {
  if(_rectangles) {
    free(_rectangles);
    _rectangles = NULL;
  }
}