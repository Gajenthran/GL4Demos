#include <GL4D/gl4dp.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
#include <assert.h>
#include <math.h>

static void quit(void) {
  gl4duClean(GL4DU_ALL);
}

static void circleExtremities(void) {
  srand(time(NULL));
  static GLubyte red = 255, green = 255, blue = 255;
  static float a = 0.0, r = 10;

  if(a >= 2 * M_PI) {
    a = 0.0;
    r += 10.0;
    red = rand()&0xFF; green = rand()&0xFF; blue = rand()&0xFF;
  }

  int x = r * cos(a) + gl4dpGetWidth()/2;
  int y = r * sin(a) + gl4dpGetHeight()/2;
  a += 0.1;

  if(x > gl4dpGetWidth() || y > gl4dpGetHeight()) {
    return;
  }

  gl4dpSetColor(RGB(red, green, blue));
  gl4dpCircle(x, y, 2);
}

static void draw(void) {
  circleExtremities();
  gl4dpUpdateScreen(NULL);
}

int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv,
			 "circleExtremities", 
			 10, 10, 800, 600,
			 GL4DW_SHOWN)) {
    return 1;
  }

  atexit(quit);
  gl4dpInitScreen();
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();

  return 0;
}
