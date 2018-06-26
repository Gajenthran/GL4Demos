#include "triangle.h"

static void quit(void) {
  triangleClean();
  gl4duClean(GL4DU_ALL);
}

static void draw(void) {
  gl4dpClearScreen();
  triangleDraw();
  gl4dpUpdateScreen(NULL);
}

int main(int argc, char ** argv) {

  if(!gl4duwCreateWindow(argc, argv,
			 "1.3_spiral",
			 10, 10, 640, 480,
			 GL4DW_SHOWN)) {
    return 1;
  }

  atexit(quit);
  gl4dpInitScreen();
  gl4duwIdleFunc(triangleMove);
  gl4duwDisplayFunc(draw);
  triangleInit(10);
  gl4duwMainLoop();

  return 0;
}
