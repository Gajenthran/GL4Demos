#include "mobile.h"

static void quit(void) {
  gl4duClean(GL4DU_ALL);
  mobileDelete();
}

static void draw(void) {
  gl4dpClearScreen();
  mobileDraw();
  gl4dpUpdateScreen(NULL);
}

int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv,
			 "2_snowBall",
			 10, 10, 800, 600,
			 GL4DW_SHOWN)) {
    return 1;
  }

  atexit(quit);
  gl4dpInitScreen();
  gl4duwIdleFunc(mobileMove);
  gl4duwDisplayFunc(draw);
  mobileInit();
  gl4duwMainLoop();
  return 0;
}
