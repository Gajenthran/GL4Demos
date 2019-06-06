#include <assert.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4df.h>
#include <GL4D/gl4dfBlurWeights.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <GL4D/gl4dh.h>
#include "audioHelper.h"

#define ECHANTILLONS 1024

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void         init(int w, int h);
static void         draw(void);
static void         quit(void);
/*!\brief dimensions de la fenêtre */
static int _screen = 0;
static int _wW, _wH;
/*!\brief identifiant des GLSL program */
static GLuint _pId = 0, _pId2 = 0;
/*!\brief identifiant de la sphere */
static GLuint _cube = 0;
static GLuint _grid = 0;
static int _moyenne = 0;
static int _gridWidth = 255;
/*!\brief grid height */
static int _gridHeight = 255;
/*!\brief position de la lumière relativement à la sphère éclairée */
static GLfloat _lumPos0[4] = {-1.1, 5.0, 5.7, 1.0};

/*!\brief initialise les paramètres OpenGL */
static void init(int w, int h) {
  _wW = w;
  _wH = h;
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  _pId  = gl4duCreateProgram("<vs>shaders/cube.vs", "<fs>shaders/cube.fs", NULL);
  _pId2 = gl4duCreateProgram("<vs>shaders/sol.vs", "<fs>shaders/sol.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  _cube = gl4dgGenCubef();
  _grid = gl4dgGenGrid2df(_gridWidth, _gridHeight);
}

/*!\brief Cette fonction dessine dans le contexte OpenGL actif.
 */
static void draw(void) {
  static Uint32 t0 = 0;
  GLfloat dt = 0.0, steps[2] = {1.0f / _wW, 1.0f / _wH};
  GLfloat steps2[2] = { 2.0 / _gridWidth, 2.0 / _gridHeight};
  GLfloat lumPos[4], *mat;
  Uint32 t;
  dt = ((t = SDL_GetTicks()) - t0) / 1000.0;
  t0 = t;
  GLint vp[4];
  glGetIntegerv(GL_VIEWPORT, vp);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * vp[3] / vp[2], 0.5 * vp[3] / vp[2], 1.0, 1000.0);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  gl4duTranslatef(0, 0, -5);
  gl4duScalef(0.01, 0.01, 0.01);
  mat = gl4duGetMatrixData();
  MMAT4XVEC4(lumPos, mat, _lumPos0);
  glUseProgram(_pId);
  gl4duPushMatrix();
  gl4duRotatef(0, 0, 0, 0);
  // gl4duScalef(0.5, 0.5, 0.5);
  glUniform1i(glGetUniformLocation(_pId, "basses"), _moyenne/1000);
  /* envoi de toutes les matrices stockées par GL4D */
  gl4duSendMatrices();
  gl4dgDraw(_cube);

  // glEnable(GL_BLEND);
  // glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glActiveTexture(GL_TEXTURE0);
  glUseProgram(_pId2);
  gl4duLoadIdentityf();
  gl4duPushMatrix(); {
    gl4duTranslatef(0, -1, -3);
    gl4duScalef(1, 1, 0.5);
    gl4duRotatef(0, 90, 20, 0);
    glUniform1i(glGetUniformLocation(_pId2, "tex"), 0);
    glUniform1ui(glGetUniformLocation(_pId2, "frame"), SDL_GetTicks());
    glUniform2fv(glGetUniformLocation(_pId2, "step"), 1, steps2);
    glUniform1f(glGetUniformLocation(_pId2, "amplitude"), _moyenne / ((1<<15) + 1.0));
    gl4duSendMatrices();
  } gl4duPopMatrix();
  gl4dgDraw(_grid);
  gl4duPopMatrix();
}

/*!\brief appelée au moment de sortir du programme (atexit), libère les éléments utilisés */
static void quit(void) {
  if(_screen) {
    gl4dpSetScreen(_screen);
    gl4dpDeleteScreen();
    _screen = 0;
  }
}

void cube(int state) {
  Sint16 * s;
  int i;
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
    for(i = 0, _moyenne = 0; i < ECHANTILLONS; i++)
      _moyenne += abs(s[i]);
    _moyenne /= ECHANTILLONS;
    return;
  default:
    draw();
    return;
  }
}