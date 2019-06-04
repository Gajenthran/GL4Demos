#include <stdlib.h>
#include <assert.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4dg.h>

/*!\typedef structure pour mobile */
typedef struct mobile_t mobile_t;
struct mobile_t {
  GLfloat x, y, z, r;
  GLfloat vx, vy, vz;
  GLfloat color[4];
};

static mobile_t _mobile;
static GLfloat _width = 1, _depth = 1;
static GLfloat _gravity[3] = {0, -9.8 * 3.0, 0};

static double get_dt(void);

void mobileInit(int n, GLfloat width, GLfloat depth) {
  _width = width; _depth = depth;
  _mobile.r = 0.5f + 0.3f;
  _mobile.x = 0.2;
  _mobile.z = 0.2;
  _mobile.y = _depth/3;
  _mobile.vx = gl4dmSURand(); 
  _mobile.vy = gl4dmURand(); 
  _mobile.vz = gl4dmSURand();
  _mobile.color[0] = 0.0f;
  _mobile.color[1] = 0.0f;
  _mobile.color[2] = 0.0f;
  _mobile.color[3] = 1.0f;
}

#define EPSILON 0.00001f

void mobileGetCoords(GLuint id, GLfloat * coords) {
  coords[0] = _mobile.x;
  coords[1] = _mobile.y;
  coords[2] = _mobile.z;
}

void mobileSetCoords(GLuint id, GLfloat * coords) {
  _mobile.x = coords[0];
  _mobile.y = coords[1];
  _mobile.z = coords[2];
}

void mobileMove(void) {
  GLfloat dt = get_dt(), d;
  // _mobile.x += _mobile.vx * dt;
  _mobile.y += _mobile.vy * dt;
  // _mobile.z += _mobile.vz * dt;
  // _mobile.vx = -_mobile.vx;
  // _mobile.x -= d - EPSILON;

  if( (d = _mobile.y - _mobile.r) <= EPSILON ) {
    if(_mobile.vy < 0) {
      _mobile.vy = -_mobile.vy;
      // _mobile.y -= d - EPSILON;
      // _mobile.vy = 0;
      // _mobile.vz = 0;
    }
    _mobile.y -= d - EPSILON;
  }
  _mobile.vx += _gravity[0] * dt;
  _mobile.vy += _gravity[1] * dt;
  _mobile.vz += _gravity[2] * dt;
}

void mobileDraw(GLuint obj) {
  GLint pId;
  glGetIntegerv(GL_CURRENT_PROGRAM, &pId);
  gl4duPushMatrix();
  gl4duTranslatef(_mobile.x, _mobile.y, _mobile.z);
  gl4duScalef(_mobile.r, _mobile.r, _mobile.r);
  gl4duSendMatrices();
  gl4duPopMatrix();
  glUniform1i(glGetUniformLocation(pId, "id"), 3);
  glUniform4fv(glGetUniformLocation(pId, "couleur"), 1, _mobile.color);
  gl4dgDraw(obj);
}

static double get_dt(void) {
  static double t0 = 0, t, dt;
  t = gl4dGetElapsedTime();
  dt = (t - t0) / 1000.0;
  t0 = t;
  return dt;
}

