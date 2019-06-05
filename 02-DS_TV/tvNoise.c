#include <assert.h>
#include <stdio.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4df.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include <GL4D/gl4dh.h>
#include <GL4D/gl4dp.h>
#include "audioHelper.h"


static void quit(void);
static void init(int w, int h);
static void draw(void);

static int _w = 1, _h = 1;
static GLuint _screen = 0;

static void quit(void) {
  if(_screen) {
    gl4dpSetScreen(_screen);
    gl4dpDeleteScreen();
    _screen = 0;
  }
}

void init(int w, int h) {
  _w = w; _h = h;
  glDisable(GL_DEPTH_TEST);
  gl4dpClearScreen();
}

static void draw(void) {
  int x, y;
  Uint32 c;
  for(x = 0; x < _w; ++x) {
    for(y = 0; y < _h; ++y) {
      c = gl4dmURand() * 255;
      c = c | (c << 8) | (c << 16) | (c << 24);
      gl4dpSetColor(c);
      gl4dpPutPixel(x, y);
    }
  }
}


void tvNoise(int state) {
  Sint16 * s;
  switch(state) {
    case GL4DH_INIT:
      _screen = gl4dpInitScreen();
      init(gl4dpGetWidth(), gl4dpGetHeight());
      return;
    case GL4DH_FREE:
      quit();
      gl4dpSetScreen(_screen);
      gl4dpDeleteScreen();
      return;
    case GL4DH_UPDATE_WITH_AUDIO:
      s = (Sint16 *)ahGetAudioStream();
      // _basses = ahGetAudioStreamFreq();
      return;
    default:
      draw();
      gl4dpUpdateScreen(NULL);
      return;
  }
}

