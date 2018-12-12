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
  { 23500,  growCircle,   NULL,       NULL },
  { 1000,   growCircle,   space,      fondu },
  { 26000,  space,        NULL,       NULL },
  { 500,    space,        voronoi,    fondud },
  { 26000,  voronoi,      NULL,       NULL },
  { 500,    voronoi,      stars,      fondui },
  { 18500,  stars,        NULL,       NULL },
  { 2000,   stars,        musicBox,   fondu },
  { 35750,  musicBox,     NULL,       NULL },
  { 3000,   musicBox,     attraction, fondui },
  { 28000,  attraction,   NULL,       NULL },
  { 500,    attraction,   musicFFT,   transition_vide },
  { 25000,  musicFFT,     NULL,       NULL },
  { 100,    musicFFT,     credits,    transition_vide },
  { 10000,  credits,      NULL,       NULL},
  {    0,   NULL,         NULL,       NULL }
};

static GLfloat _dim[] = {1024, 768};

int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "PG2D1718S2 - Circles", 
			 0, 0, 
			 _dim[0], _dim[1],
			 GL4DW_RESIZABLE | GL4DW_SHOWN))
    return 1;
  init();
  atexit(quit);
  gl4duwResizeFunc(resize);
  gl4duwKeyDownFunc(keydown);
  gl4duwDisplayFunc(gl4dhDraw);

  ahInitAudio("audio/JPB - High.mp3");
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
