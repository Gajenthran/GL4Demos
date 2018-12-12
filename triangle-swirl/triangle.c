#include "triangle.h"
#include <assert.h>

static triangle * _triangles = NULL;
static int _nbTriangles = 0;

void triangleInit(int n) {
  int i;
  _nbTriangles = n;
  if(_triangles) {
    free(_triangles);
    _triangles = NULL;
  }

  _triangles = malloc(_nbTriangles * sizeof *_triangles);
  assert(_triangles);

  for(i = 0; i < _nbTriangles; i++){
    _triangles[i].a.x = gl4dpGetWidth() - 10; _triangles[i].a.y = 10;
    _triangles[i].b.x = 10; _triangles[i].b.y = 10;
    _triangles[i].c.x = gl4dpGetWidth()/2; _triangles[i].c.y = gl4dpGetHeight();
  }
}

void triangleMove(void) {
  static int i = 1;
  if(i > _nbTriangles)
    return;

  _triangles[i].a.x = (((_triangles[i-1].a.x + _triangles[i-1].b.x)/2) + _triangles[i-1].a.x) / 2;
  _triangles[i].a.y = (((_triangles[i-1].a.y + _triangles[i-1].b.y)/2) + _triangles[i-1].a.y) / 2;
  _triangles[i].b.x = (((_triangles[i-1].b.x + _triangles[i-1].c.x)/2) + _triangles[i-1].b.x) / 2;
  _triangles[i].b.y = (((_triangles[i-1].b.y + _triangles[i-1].c.y)/2) + _triangles[i-1].b.y) / 2;
  _triangles[i].c.x = (((_triangles[i-1].c.x + _triangles[i-1].a.x)/2) + _triangles[i-1].c.x) / 2;
  _triangles[i].c.y = (((_triangles[i-1].c.y + _triangles[i-1].a.y)/2) + _triangles[i-1].c.y) / 2;
  i++;
}

void triangleDraw(void) {
  int i;
  gl4dpSetColor(RGB(255,255,255));
  for(i = 0; i < _nbTriangles; i++) {
    gl4dpLine(_triangles[i].a.x, _triangles[i].a.y, _triangles[i].b.x, _triangles[i].b.y);
    gl4dpLine(_triangles[i].b.x, _triangles[i].b.y, _triangles[i].c.x, _triangles[i].c.y);
    gl4dpLine(_triangles[i].c.x, _triangles[i].c.y, _triangles[i].a.x, _triangles[i].a.y);
  }
}

void triangleClean(void) {
  if(_triangles) {
    free(_triangles);
    _triangles = NULL;
  }
}