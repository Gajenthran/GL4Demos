#include "mobile.h"

static void quit(void) {
  mobileDelete();
  gl4duClean(GL4DU_ALL);
}

static void draw(void) {
  gl4dpClearScreen();
  mobileDraw();
  gl4dpUpdateScreen(NULL);
}

int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv,
       "growCircle",
       10, 10, 800, 600,
       GL4DW_SHOWN)) {
    return 1;
  }

  atexit(quit);
  gl4dpInitScreen();
  gl4duwIdleFunc(mobileGrow);
  gl4duwDisplayFunc(draw);
  mobileInit(200);
  gl4duwMainLoop();

  return 0;
}
