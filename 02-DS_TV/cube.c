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

static void init(int w, int h);
static void draw(void);
static void quit(void);

typedef struct cube_t cube_t;
struct cube_t {
  float x, y, z;
  float s;
  GLfloat c[4];
};

static int _screen = 0;
static int _wW, _wH;

static GLuint _pId = 0, _pId2 = 0;
static GLuint _cube = 0;
static int _nbCubes = 0;
static cube_t * _cubes = NULL;
static GLuint _grid = 0;
static GLuint _move = 0;
static int _moyenne = 0;
static int _gridWidth = 255;
static int _gridHeight = 255;

static GLfloat _lumPos0[4] = {0.0, 0.0, -4.0, 1.0};
static int _state = 0;

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

  double i;
  int it = 0;
  for(i = 0; i < 2 * M_PI; i += 2 * M_PI / ECHANTILLONS) {
    _nbCubes++;
  }

  _cubes = malloc(_nbCubes * sizeof *_cubes);
  for(i = 0; i < 2 * M_PI; i += 2 * M_PI / ECHANTILLONS) {
    _cubes[it].x = cos(i);
    _cubes[it].y = sin(i);
    _cubes[it].z = (gl4dmURand() * 8.0 + 8.0) * -1.0 + 3.0;
    GLfloat c = gl4dmURand();
    _cubes[it].c[0] = c > 0.5 ? 1.0 : 0.0;
    _cubes[it].c[1] = c > 0.5 ? 0.0 : 0.0;
    _cubes[it].c[2] = c > 0.5 ? 1.0 : 1.0;
    _cubes[it].c[3] = 1.0;
    _cubes[it].s = (gl4dmURand() * 1.0) * 0.05;
    it++;
  }
}

static void draw(void) {
  static Uint32 t0 = 0;
  static double a = 0.0;
  static int prev_moy = 0;
  GLfloat dt = 0.0;
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
  mat = gl4duGetMatrixData();
  MMAT4XVEC4(lumPos, mat, _lumPos0);
  glUseProgram(_pId);
  double i; 
  int it = 0;
  if((_moyenne >= 2000 && _moyenne <= 7000 && 
     prev_moy >= 2000 && prev_moy <= 7000 &&
     _lumPos0[2] < -10.5) || 
     _lumPos0[2] < -12.0) {
    _lumPos0[2] = -4.0;
    _state++;
  } else if(!_state) {
    _lumPos0[2] -= 0.009;
  }

  prev_moy = _moyenne;
  for(i = 0; i < 2 * M_PI; i += 2 * M_PI / ECHANTILLONS) {
    gl4duPushMatrix(); {
      GLfloat x = _cubes[it].x;
      GLfloat y = _cubes[it].y;
      if(_state) {
        x *= _moyenne * 0.0001;
        y *= _moyenne * 0.0001;
      }
      gl4duTranslatef(x, y, _cubes[it].z);
      gl4duRotatef(0, 0, 0, 0);
      gl4duScalef(_cubes[it].s,_cubes[it].s, _cubes[it].s);
      glUniform1i(glGetUniformLocation(_pId, "id"), 2);
      glUniform1i(glGetUniformLocation(_pId, "move"), _move);
      glUniform1i(glGetUniformLocation(_pId, "basses"), _moyenne/1000);
      glUniform1i(glGetUniformLocation(_pId, "time"), SDL_GetTicks());
      glUniform4fv(glGetUniformLocation(_pId, "lumPos"), 1, lumPos);
      gl4duSendMatrices();
    } gl4duPopMatrix();
    it++;
    gl4dgDraw(_cube);
  }

  glActiveTexture(GL_TEXTURE0);
  glUseProgram(_pId2);
  gl4duLoadIdentityf();
  for(i = 0; i < 2; i++) {
    gl4duPushMatrix(); {
      gl4duTranslatef(0, i ? -1 : 1, -3);
      gl4duScalef(1, 1, 0.5);
      if(!i) gl4duRotatef(180, 270, 0, 0);
      glUniform1i(glGetUniformLocation(_pId2, "tex"), 0);
      glUniform1ui(glGetUniformLocation(_pId2, "frame"), SDL_GetTicks());
      glUniform2fv(glGetUniformLocation(_pId2, "step"), 1, steps2);
      glUniform1f(glGetUniformLocation(_pId2, "amplitude"), _moyenne / ((1 << 15) + 1.0));
      gl4duSendMatrices();
    } gl4duPopMatrix();
    gl4dgDraw(_grid);
  }

  if(_state) {
    _lumPos0[0] = cos(a) * 50;
    _lumPos0[0] = sin(a) * 50;
    a += 2 * M_PI / ECHANTILLONS;
    if(a >= 2 * M_PI) a = 0.0;
    _move = 1;
  }
}

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