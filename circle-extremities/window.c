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
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  static GLubyte red = 255, green = 255, blue = 255;
  static float a = 0.0, r = 100;

  float radius = 100;
  int nbLine = 100;
  gl4dpCircle(gl4dpGetWidth()/2,gl4dpGetHeight()/2, radius);
  for(double i = 0; i < 2 * M_PI; i += 2 * M_PI / nbLine) {
    int x0 = radius * cos(i);
    int y0 = radius * sin(i);
    int x1 = (radius + 20) * cos(i);
    int y1 = (radius + 20) * sin(i);
    // printf("%d - %d\n", x, y);
    gl4dpSetColor(RGB(red, green, blue));
    gl4dpLine(x0 + gl4dpGetWidth()/2, 
      y0 + gl4dpGetHeight()/2, 
      x1 + gl4dpGetWidth()/2, 
      y1 + gl4dpGetHeight()/2);
    // gl4dpCircle(x + gl4dpGetWidth()/2, y + gl4dpGetHeight()/2, 10);
  }
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