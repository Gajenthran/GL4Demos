#include <assert.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <GL4D/gl4dh.h>
#include "audioHelper.h"

static void init(int w, int h);
static void draw(void);
static void quit(void);

static int _wW, _wH;
static int _screen = 0;

static GLuint _pId = 0;
static int _basses = 0;
static GLuint _quad = 0;

static int _state = 0;
static int _circle = 0;
static int _side = 0;
static GLfloat _color[4] = {1.0, 1.0, 0.0, 1.0};


static void init(int w, int h) {
  _wH = h;
  _wW = w;

  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  _pId = gl4duCreateProgram("<vs>shaders/color.vs", "<fs>shaders/color.fs", NULL);
  _quad = gl4dgGenQuadf();
  glBindTexture(GL_TEXTURE_1D, 0);
}

static void quit(void) {
  if(_screen) {
    gl4dpSetScreen(_screen);
    gl4dpDeleteScreen();
    _screen = 0;
  }
}

static void draw(void) {
  static int t0 = 0, t, dt;
  t = SDL_GetTicks();
  dt = (t - t0) / 1000.0;

  if(_state == 0 && dt >= 3) {
    _state = 1;
    _side = 2;
    t0 = t;
  }
  else if(_state == 1 && dt >= 3) {
    _state++;
    _side = 3;
    t0 = t;
  }
  else if(_state == 2 && dt >= 3) {
    _state++;
    _side = 1;
    t0 = t;
  }
  else if(_state == 3 && dt >= 3) {
    _state++;
    _side = 2;
    t0 = t;
  }
  else if(_state == 4 && dt >= 3) {
    _state++;
    _side = 3;
    t0 = t;
  }
  else if(_state == 5 && dt >= 3) {
    _state++;
    _side = 1;
    t0 = t;
  }
  else if(_state == 6 && dt >= 3) {
    _state++;
    _side = 2;
    t0 = t;
  }
  else if(_state == 7 && dt >= 3) {
    _state++;
    _side = 2;
    t0 = t;
  }
  else if(_state == 8) {
    _side = 1;
    _circle = 1;
    _state++;
    t0 = t;
  }
  else if(_state == 9 && dt >= 3) {
    _side = 1;
    _state++;
    t0 = t;
  }
  else if(_state == 10 && dt >= 3) {
    _side = 3;
    _state++;
    t0 = t;
  }

  if(_state >= 8 && dt >= 3) {
    _color[0] = fabs(cos(gl4dmURand() * t));
    _color[1] = fabs(cos(gl4dmURand() * dt));
    _color[2] = fabs(cos(gl4dmURand() * (t - dt)));
  }

  if(_state >= 8 && _basses == 0) {
    printf("%d\n", _basses);
    _state = -1;
  }

  glDisable(GL_DEPTH_TEST);
  glUseProgram(_pId);

  glClear(GL_COLOR_BUFFER_BIT);  
  glUniform1i(glGetUniformLocation(_pId, "time"), t);
  glUniform1i(glGetUniformLocation(_pId, "basses"), _basses);
  glUniform1i(glGetUniformLocation(_pId, "side"), _side);
  glUniform1i(glGetUniformLocation(_pId, "state"), _state);
  glUniform1i(glGetUniformLocation(_pId, "circle"), _circle);
  glUniform1fv(glGetUniformLocation(_pId, "color"), 4, _color);

  gl4dgDraw(_quad);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_1D, 0);
  glUseProgram(0);
}

void color(int state) {
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