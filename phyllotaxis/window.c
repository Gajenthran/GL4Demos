#include <GL4D/gl4dp.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
#include <assert.h>

static void quit(void) {
  gl4duClean(GL4DU_ALL);
}

static void phyllotaxis(void) {
  static float n = 0.0;
  static int color_state = 0;
  float c = 10.0, form = 136.5;
  GLubyte r, g, b;

  float a = n * form;
  float rad = c * sqrt(n);
  float x = rad * cos(a) + gl4dpGetWidth() / 2;
  float y = rad * sin(a) + gl4dpGetHeight() /2;

  if(color_state) { r = rand()&0xFF; g = rand()&0xFF; b = rand()&0xFF; }
  else { r = g = b = 255; }

  color_state = (color_state + 1) % 2;
  gl4dpSetColor(RGB(r, g, b));
  gl4dpFilledCircle(x, y, 2);
  n++;
}

static void draw(void) {
  phyllotaxis();
  gl4dpUpdateScreen(NULL);
}

int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv,
			 "phyllotaxis",
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
