#include "rectangle.h"

static void quit(void) {
  rectangleClean();
  gl4duClean(GL4DU_ALL);
}

static void draw(void) {
  gl4dpClearScreen();
  rectangleDraw();
  gl4dpUpdateScreen(NULL);
}

int main(int argc, char ** argv) {

  if(!gl4duwCreateWindow(argc, argv,
			 "rectangleSwirl",
			 10, 10, 600, 500,
			 GL4DW_SHOWN)) {
    return 1;
  }

  atexit(quit);
  gl4dpInitScreen();
  gl4duwIdleFunc(rectangleMove);
  gl4duwDisplayFunc(draw);
  rectangleInit(30);
  gl4duwMainLoop();

  return 0;
}
