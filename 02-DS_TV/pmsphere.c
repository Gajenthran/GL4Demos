#include <assert.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <GL4D/gl4dh.h>
#include "audioHelper.h"

static void  init(int w, int h);
static void  draw(void);
static void  quit(void);

static int _w, _h;
static GLuint _screen = 0;

static GLuint _tId = 0;
static GLuint _pId = 0;
static int _swirl = 0;
static int _state = 0, _pixelPrec = 1;
static int _basses = 0, _pixel = 0;

static GLuint _sphere = 0;
static GLuint _longitudes = 200, _latitudes = 200;
static GLfloat _lumPos0[4] = {-15.1, 20.0, 20.7, 1.0};

static const char * _texture_filename = "images/pacman.png";
static const char * _sampler_name = "pacman";

static void init(int w, int h) {
  _w = w; _h = h;
  if(!_tId) {
    glGenTextures(1, &_tId);
    SDL_Surface * t;
    glBindTexture(GL_TEXTURE_2D, _tId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    if( (t = IMG_Load(_texture_filename)) != NULL ) {
      int mode = t->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;    
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->w, t->h, 0, mode, GL_UNSIGNED_BYTE, t->pixels);
      SDL_FreeSurface(t);
    } else {
      fprintf(stderr, "can't open file %s : %s\n", _texture_filename, SDL_GetError());
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
  }

  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  _pId  = gl4duCreateProgram("<vs>shaders/pmsphere.vs", "<fs>shaders/pmsphere.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  _sphere = gl4dgGenSpheref(_longitudes, _latitudes);
}

static void draw(void) {
  static GLfloat a0 = 0.0;
  static Uint32 t0 = 0;
  GLint vp[4];
  GLfloat dt = 0.0, steps[2] = {1.0f / _w, 1.0f / _h};
  GLfloat lumPos[4], *mat;
  Uint32 t = SDL_GetTicks();
  dt = (t - t0) / 1000.0;
  t0 = t;

  GLboolean gdt = glIsEnabled(GL_DEPTH_TEST);
  glGetIntegerv(GL_VIEWPORT, vp);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * vp[3] / vp[2], 0.5 * vp[3] / vp[2], 1.0, 1000.0);
  gl4duBindMatrix("modelViewMatrix");
  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  gl4duTranslatef(0, 0, -3);
  mat = gl4duGetMatrixData();
  MMAT4XVEC4(lumPos, mat, _lumPos0);

  glUseProgram(_pId);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tId);
  gl4duRotatef(a0, 0, 1, 0);
  gl4duScalef(0.50, 0.50, 0.50);
  glUniform1i(glGetUniformLocation(_pId, _sampler_name), 0);
  glUniform1i(glGetUniformLocation(_pId, "basses"), _basses);
  glUniform1i(glGetUniformLocation(_pId, "swirl"), _swirl);
  glUniform1i(glGetUniformLocation(_pId, "pixel"), _pixel);
  glUniform1i(glGetUniformLocation(_pId, "pixelPrec"), _pixelPrec);
  glUniform1i(glGetUniformLocation(_pId, "time"), t);
  glUniform2fv(glGetUniformLocation(_pId, "steps"), 1, steps);
  glUniform4fv(glGetUniformLocation(_pId, "lumPos"), 1, lumPos);
  gl4duSendMatrices();
  gl4dgDraw(_sphere);
  
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);

  a0 += 1000.0 * dt / 24.0;
  if(_basses >= 13) _state++;
  if(_state == 2) { _swirl = 1; }
  if(_state >= 7) { a0 = 270.0; _swirl = 0; _pixel = 1; _pixelPrec++; }
  if(!gdt)
    glDisable(GL_DEPTH_TEST);
}

static void quit(void) {
  if(_tId) {
    glDeleteTextures(1, &_tId);
    _tId = 0;
  }

  if(_screen) {
    gl4dpSetScreen(_screen);
    gl4dpDeleteScreen();
    _screen = 0;
  }
}

void pmsphere(int state) {
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
