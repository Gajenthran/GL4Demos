#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <GL4D/gl4df.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4dh.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <GL4D/gl4duw_SDL2.h>
#include "audioHelper.h"


#define EPSILON 0.00001f

typedef struct mobile_t mobile_t;
struct mobile_t {
  GLfloat x, y, z, r;
  GLfloat vy;
  GLfloat color[4];
};

static void   init(int w, int h);
static void   mobileInit(GLfloat width, GLfloat depth);
static double get_dt(void);
static void   mobileMove(void);
static void   mobileDraw(GLuint obj);
static void   draw(void);
static void   quit(void);


static GLuint _screen = 0;
static int _wW, _wH;
static int _basses = 0;
static int _state = 0;

static GLuint _shPID = 0;
static GLuint _smPID = 0;
static GLuint _sphere = 0, _quad = 0;
static GLfloat _plan_s = 15.0f;
static GLuint _fbo = 0;
static GLuint _colorTex = 0;
static GLuint _depthTex = 0;
static GLuint _idTex = 0;
static GLuint _smTex = 0;

static GLfloat _lumpos[] = { -9, 9, 0, 1 };
static mobile_t _mobile;
static GLfloat _width = 1, _depth = 1;
static GLfloat _gravity = -9.8 * 3.0;

#define SHADOW_MAP_SIDE 512


static void init(int w, int h) {
  _wW = w;
  _wH = h;
  glEnable(GL_DEPTH_TEST);
  _shPID  = gl4duCreateProgram("<vs>shaders/shadow.vs", "<fs>shaders/shadow.fs", NULL);
  _smPID  = gl4duCreateProgram("<vs>shaders/shadowMap.vs", "<fs>shaders/shadowMap.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelMatrix");
  gl4duGenMatrix(GL_FLOAT, "lightViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "lightProjectionMatrix");
  gl4duGenMatrix(GL_FLOAT, "cameraViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "cameraProjectionMatrix");
  gl4duGenMatrix(GL_FLOAT, "cameraPVMMatrix");

  // glViewport(0, 0, _wW, _wH);
  gl4duBindMatrix("lightProjectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-1, 1, -1, 1, 1.5, 50.0);
  gl4duBindMatrix("cameraProjectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * _wH / _wW, 0.5 * _wH / _wW, 1.0, 50.0);
  gl4duBindMatrix("modelMatrix");

  _sphere = gl4dgGenSpheref(30, 30);
  _quad = gl4dgGenQuadf();
  mobileInit(_plan_s, _plan_s);

  glGenTextures(1, &_smTex);
  glBindTexture(GL_TEXTURE_2D, _smTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_SIDE, SHADOW_MAP_SIDE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

  glGenTextures(1, &_colorTex);
  glBindTexture(GL_TEXTURE_2D, _colorTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _wW, _wH, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

  glGenTextures(1, &_depthTex);
  glBindTexture(GL_TEXTURE_2D, _depthTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _wW, _wH, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);

  glGenTextures(1, &_idTex);
  glBindTexture(GL_TEXTURE_2D, _idTex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, _wW, _wH, 0, GL_RED, GL_UNSIGNED_INT, NULL);

  glGenFramebuffers(1, &_fbo);
}

static void mobileInit(GLfloat width, GLfloat depth) {
  _width = width; _depth = depth;
  _mobile.r = 0.8f;
  _mobile.x = 0.2;
  _mobile.z = 0.2;
  _mobile.y = 3.0f;
  _mobile.vy = 1.0;
  _mobile.color[0] = 1.0f;
  _mobile.color[1] = 1.0f;
  _mobile.color[2] = 1.0f;
  _mobile.color[3] = 1.0f;
}

static void scene(GLboolean sm) {
  glEnable(GL_CULL_FACE);
  int time = SDL_GetTicks();
  if(sm) {
    glCullFace(GL_FRONT);
    glUseProgram(_smPID);
    gl4duBindMatrix("lightViewMatrix");
    gl4duLoadIdentityf();
    gl4duLookAtf(_lumpos[0], _lumpos[1], _lumpos[2], 0, 2, 0, 0, 1, 0);
    gl4duBindMatrix("modelMatrix");
    gl4duLoadIdentityf();
  } else {
    GLfloat white[] = {255, 255, 255, 0}, lp[4], *mat;
    glCullFace(GL_BACK);
    glUseProgram(_shPID);
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _smTex);
    glUniform1i(glGetUniformLocation(_shPID, "smTex"), 0);
    gl4duBindMatrix("cameraViewMatrix");
    gl4duLoadIdentityf();
    gl4duLookAtf(0, 4, 22, 0, 2, 0, 0, 1, 0);
    /* lumière positionnelle */
    mat = gl4duGetMatrixData();
    MMAT4XVEC4(lp, mat, _lumpos);
    MVEC4WEIGHT(lp);
    glUniform4fv(glGetUniformLocation(_shPID, "lumpos"), 1, lp);
    gl4duBindMatrix("modelMatrix");
    gl4duLoadIdentityf();
    gl4duPushMatrix(); {
      gl4duTranslatef(_lumpos[0], _lumpos[1], _lumpos[2]);
      gl4duSendMatrices();
    } gl4duPopMatrix();
    glUniform1i(glGetUniformLocation(_shPID, "id"), 2);
    glUniform4fv(glGetUniformLocation(_shPID, "couleur"), 1, white);
    glUniform1i(glGetUniformLocation(_shPID, "id"), 1);
    glUniform1i(glGetUniformLocation(_shPID, "time"), time);
  }
  gl4duPushMatrix(); {
    gl4duRotatef(-90, 1, 0, 0);
    gl4duScalef(_plan_s, _plan_s, _plan_s);
    gl4duSendMatrices();
  } gl4duPopMatrix();
  gl4dgDraw(_quad);
  gl4duSendMatrices();
  mobileDraw(_sphere);
}

static void mobileMove(void) {
  GLfloat dt = get_dt(), d;
  _mobile.y += _mobile.vy * dt;

  if( (d = _mobile.y - _mobile.r) <= EPSILON && _state <= 6) {
    if(_mobile.vy < 0) {
      _mobile.vy = -_mobile.vy;
    }
    _mobile.y -= d - EPSILON;
  }
  _mobile.vy += _gravity * dt;
}

static double get_dt(void) {
  static double t0 = 0, t, dt;
  t = SDL_GetTicks();
  dt = (t - t0) / 1000.0;
  t0 = t;
  return dt;
}

static void draw(void) {
  static int t0 = 0, t, dt;
  t = SDL_GetTicks();
  dt = (t - t0) / 1000.0;
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  if(_state == 0 && dt == 1) {
    _state = 1;
    _mobile.color[0] = 0.0f;
    _mobile.color[1] = 0.0f;
    _mobile.color[2] = 0.0f;
    t0 = t;
  }
  else if(_state == 1 && dt == 2) {
    _state = 2;
    _mobile.color[0] = 1.0f;
    _mobile.color[1] = 1.0f;
    _mobile.color[2] = 1.0f;
    t0 = t;
  }
  else if(_state == 2 && dt == 2) {
    _state = 3;
    t0 = t;
  }
  else if(_state == 3 && dt == 2) {
    _state = 4;
    _mobile.color[0] = 0.5f;
    _mobile.color[1] = 1.0f;
    _mobile.color[2] = 1.0f;
    t0 = t;
  }
  else if(_state == 4 && dt == 2) {
    _state = 5;
    _mobile.color[0] = 0.5f;
    _mobile.color[1] = 0.5f;
    _mobile.color[2] = 1.0f;
    t0 = t;
  }
  else if(_state == 5 && dt == 2) {
    _state = 6;
    _mobile.color[0] = 1.0f;
    _mobile.color[1] = 1.0f;
    _mobile.color[2] = 0.5f;
    t0 = t;
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  }
  else if(_state == 6 && dt == 2) {
    _state = 7;
    t0 = t;
  }

  mobileMove();
  GLenum renderings[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

  glDrawBuffer(GL_NONE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _smTex, 0);

  glViewport(0, 0, SHADOW_MAP_SIDE, SHADOW_MAP_SIDE);
  glClear(GL_DEPTH_BUFFER_BIT);
  scene(GL_TRUE);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _colorTex, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, _idTex, 0);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthTex, 0);
  glViewport(0, 0, _wW, _wH);

  glDrawBuffers(1, &renderings[1]);
  glClear(GL_COLOR_BUFFER_BIT);

  glDrawBuffers(1, renderings);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glDrawBuffers(2, renderings);

  scene(GL_FALSE);


  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, _wW, _wH, 0, 0, _wW, _wH, GL_COLOR_BUFFER_BIT, GL_LINEAR);
  glBlitFramebuffer(0, 0, _wW, _wH, 0, 0, _wW, _wH, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
  if(_state == 7) glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

static void mobileDraw(GLuint obj) {
  GLint pId;
  glGetIntegerv(GL_CURRENT_PROGRAM, &pId);
  gl4duPushMatrix();
  gl4duTranslatef(_mobile.x, _mobile.y, _mobile.z);
  gl4duScalef(_mobile.r, _mobile.r, _mobile.r);
  gl4duSendMatrices();
  gl4duPopMatrix();
  glUniform1i(glGetUniformLocation(pId, "id"), 3);
  glUniform4fv(glGetUniformLocation(pId, "couleur"), 1, _mobile.color);
  glUniform1i(glGetUniformLocation(pId, "state"), _state);

  gl4dgDraw(obj);
}

static void quit(void) {
  if(_fbo) {
    glDeleteTextures(1, &_colorTex);
    glDeleteTextures(1, &_depthTex);
    glDeleteTextures(1, &_idTex);
    glDeleteTextures(1, &_smTex);
    glDeleteFramebuffers(1, &_fbo);
    _fbo = 0;
  }
  if(_screen) {
    gl4dpSetScreen(_screen);
    gl4dpDeleteScreen();
    _screen = 0;
  }
}

void shadow(int state) {
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

