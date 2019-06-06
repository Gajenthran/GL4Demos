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

static GLuint _screen = 0;
/*!\brief window's width and height */
static int _wW, _wH;
/*!\brief GLSL program Id */
static GLuint _pId = 0;
/*!\brief A generated Quad Id */
static GLuint _quad = 0;
static GLfloat _formCoord[32] = {
  1.5, 2.5, 3.5, 2.5, 1.5, 4.3,
  4.5, 1.5, 3.5, 0.5, 3.0, 3.5,
  1.5, 2.5, 2.5, 0.5, 4.5, 2.5,
  4.5, 0.5, 3.5, 1.5, 1.5, 1.5,
  0.5, 1.5, 0.5, 0.5, 4.5, 4.5,
  0.5, 0.5
};

/*!\brief initialise OpenGL parameters and data. */
static void init(int w, int h) {
  _wW = w;
  _wH = h;
  /* setting OpenGL clear color (for next glClear) */
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  /* loading and compiling a GLSL program composed with a vertex shader and a fragment shader */
  _pId = gl4duCreateProgram("<vs>shaders/pmform.vs", "<fs>shaders/pmform.fs", NULL);
  /* generating a Quad (Vertex coords + Vertex Normal + Vertex TexCoord */
  _quad = gl4dgGenQuadf();
  /* creating a 1D texture to store mobile coords */
  glBindTexture(GL_TEXTURE_1D, 0);
  /* calling mobileInit */
}


/*!\brief mobile simulation and draw
 */
static void draw(void) {
  int time = SDL_GetTicks();
  glDisable(GL_DEPTH_TEST);
  glUseProgram(_pId);

  glClear(GL_COLOR_BUFFER_BIT);  
  glUniform1i(glGetUniformLocation(_pId, "time"), time);
  glUniform1fv(glGetUniformLocation(_pId, "formCoord"), 32, _formCoord);

  gl4dgDraw(_quad);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_1D, 0);
  glUseProgram(0);
}

/*!\brief called at exit and delete and clean used data. */
static void quit(void) {
  if(_screen) {
    gl4dpSetScreen(_screen);
    gl4dpDeleteScreen();
    _screen = 0;
  }
}

void pmform(int state) {
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
    return;
  default:
    draw();
    return;
  }
}