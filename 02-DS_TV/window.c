#include <stdlib.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4dh.h>
#include <GL4D/gl4duw_SDL2.h>
#include "animations.h"
#include "audioHelper.h"

static void init(void);
static void quit(void);
static void resize(int w, int h);
static void keydown(int keycode);

static GL4DHanime _animations[] = {
  { 14000, shadow,  NULL,  NULL },
  { 100,   shadow,  pmsphere,  transition_vide },
  { 24000, pmsphere,  NULL,  NULL },
  { 2000,  pmsphere,  color,  fondu },
  { 33500, color,  NULL,  NULL },
  { 1500,  color,  cube,  fondud },
  { 38500, cube,  NULL,  NULL },
  { 100,   cube,  wave,  fondud },
  { 42000, wave,  NULL,  NULL },
  { 2000,  wave,  musicFFT,  fondu },
  { 20000, musicFFT,  NULL,  NULL },
  { 100,   musicFFT,  credits,  transition_vide },
  { 5000,  credits,  NULL,  NULL },
  // { 340000, cube,  NULL,  NULL },
  { 0,     NULL,  NULL,  NULL }
};

static GLfloat _dim[] = {1024, 768};

int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "PG2D1819S2 - TV", 
			 0, 0, 
			 _dim[0], _dim[1],
			 GL4DW_RESIZABLE | GL4DW_SHOWN))
    return 1;
  init();
  atexit(quit);
  gl4duwResizeFunc(resize);
  gl4duwKeyDownFunc(keydown);
  gl4duwDisplayFunc(gl4dhDraw);

  ahInitAudio("audio/mixedsong.mp3");
  gl4duwMainLoop();
  return 0;
}

static void init(void) {
  glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
  gl4dhInit(_animations, _dim[0], _dim[1], animationsInit);
  resize(_dim[0], _dim[1]);
}

static void resize(int w, int h) {
  _dim[0] = w; _dim[1] = h;
  glViewport(0, 0, _dim[0], _dim[1]);
}

static void keydown(int keycode) {
  switch(keycode) {
  case SDLK_ESCAPE:
  case 'q':
    exit(0);
  default: break;
  }
}

static void quit(void) {
  ahClean();
  gl4duClean(GL4DU_ALL);
}
