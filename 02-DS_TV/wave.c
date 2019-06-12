#include <assert.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <fftw3.h>
#include <GL4D/gl4dh.h>
#include "audioHelper.h"

static void init(int w, int h);
static void draw(void);
static void quit(void);

/* !\brief écran de la démo */
static GLuint _screen = 0;
/* !\brief dimensions de la démo */
static int _w, _h;
/*!\brief identifiant du programme GLSL */
static GLuint _pId = 0;
/*!\brief identifiant du cube de GL4Dummies */
static GLuint _quad = 0;
/* !\brief basses de la démo */
static GLuint _basses = 0;
/*!\brief mode _wave pour la représentation graphique de la musique*/
static GLuint _wave = 0;
/*!\brief état de la démo */
static GLuint _state = 0;

/*!\brief initialise les paramètres OpenGL et les données */
static void init(int w, int h) {
  _w = w;
  _h = h;
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  _pId = gl4duCreateProgram("<vs>shaders/wave.vs", "<fs>shaders/wave.fs", NULL);
  _quad = gl4dgGenQuadf();
  glBindTexture(GL_TEXTURE_1D, 0);
}

/*!\brief dessine dans le contexte OpenGL actif. */
static void draw(void) {
  static int prev_basses = 0;
  static float line = 10.0;
  int time = SDL_GetTicks();
  glDisable(GL_DEPTH_TEST);
  glUseProgram(_pId);

  glClear(GL_COLOR_BUFFER_BIT);  
  glUniform1i(glGetUniformLocation(_pId, "time"), time);
  glUniform1i(glGetUniformLocation(_pId, "basses"), _basses);
  glUniform1i(glGetUniformLocation(_pId, "wave"), _wave);
  glUniform1f(glGetUniformLocation(_pId, "line"), line);
  gl4dgDraw(_quad);
  
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_1D, 0);
  glUseProgram(0);

  /* nombre de lignes pour la représentation du cercle */
  if (prev_basses >= 15 && _basses >= 15) { 
    line += 15.0;
    _state++;
  }

  /* activation du mode _wave */
  if(_state == 3)
    _wave = 1;
  prev_basses = _basses;
}

/* !\brief libère les éléments OpenGL utilisés */
static void quit(void) {
  if(_screen) {
    gl4dpSetScreen(_screen);
    gl4dpDeleteScreen();
    _screen = 0;
  }
}

void wave(int state) {
  Sint16 * s;
  switch(state) {
  case GL4DH_INIT:
    _screen = gl4dpInitScreen();
    init(gl4dpGetWidth(), gl4dpGetHeight());
    return;
  case GL4DH_FREE:
    quit();
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    s = (Sint16 *)ahGetAudioStream();
    _basses = ahGetAudioStreamFreq();
    return;
  default:
    draw();
    return;
  }
}