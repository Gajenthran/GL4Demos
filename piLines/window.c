#include <GL4D/gl4dp.h>
#include "mobile.h"
#include <assert.h>

static void quit(void) {
  gl4duClean(GL4DU_ALL);
}

static void draw(void) {
  mobileDraw();
  gl4dpUpdateScreen(NULL);
}

int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv,
			 "piLines",
			 10, 10, 800, 600,
			 GL4DW_SHOWN)) {
    return 1;
  }

  atexit(quit);
  gl4dpInitScreen();
  gl4duwDisplayFunc(draw);
  mobileInit();
  gl4duwMainLoop();
  return 0;
}
