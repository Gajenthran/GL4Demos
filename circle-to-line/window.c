#include "mobile.h"

static void quit(void) {
  gl4duClean(GL4DU_ALL);
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
  gl4duwIdleFunc(mobileToLine);
  gl4duwDisplayFunc(draw);
  mobileInit(100.0);
  gl4duwMainLoop();
  return 0;
}
