#include "mobile.h"
#include <assert.h>
#include <math.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>

static mobile_t _mobile;
static char _pi[] = "3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745028410270193852110555964462294895493038196442881097566593344612847564823378678316527120190914564856692346034861045432664821339360726024914127372458700660631558817488152092096282925409171536436789259036001133053054882046652138414695194151160943305727036575959195309218611738193261179310511854807446237996274956735188575272489122793818301194912983367336244065664308602139494639522473719070217986094370277053921717629317675238467481846766940513200056812714526356082778577134275778960917363717872146844090122495343014";
static int _idx = 0;
static GLuint _red = RGB(200, 48, 32);
static GLuint _blue = RGB(98, 166, 240);

static float myRand(float max) {
  float rand = gl4dmURand() * max;
  float sgn = gl4dmURand() * 2.0;
  sgn = sgn > 1.0 ? 1.0 : -1.0;
  rand *= sgn;
  return rand;
}

void mobileInit(void) {
  GLubyte r, g, b;
  _mobile.x = gl4dpGetWidth()/2;
  _mobile.y = gl4dpGetHeight()/2;
  _mobile.r = (gl4dpGetWidth() / 40.0f) * (0.2f + 0.8f * rand() / (RAND_MAX + 1.0));
  r = rand()&0xFF; g = rand()&0xFF; b = rand()&0xFF;
  _mobile.c = RGB(r, g, b);
}

void mobileDraw(void) {
  int sgn = 1;
  if(!_idx) {
    gl4dpClearScreenWith(_red);
    gl4dpSetColor(_red);
    gl4dpFilledCircle(gl4dpGetWidth()/2, gl4dpGetHeight()/2, 200);
  }

  int dig = _pi[_idx] - '0';
  int nxtDig = _pi[_idx+1] - '0';
  _idx++;

  float diff = (2 * M_PI) / 4;
  float a1 = dig * 4 / (2 * M_PI) + gl4dmURand() * diff * sgn;
  float a2 = nxtDig * 4 / (2 * M_PI) + gl4dmURand() * diff * sgn;

  float x1 = 200 * cos(a1) + gl4dpGetWidth()/2;
  float y1 = 200 * sin(a1) + gl4dpGetHeight()/2;

  float x2 = 200 * cos(a2) + gl4dpGetWidth()/2;
  float y2 = 200 * sin(a2) + gl4dpGetHeight()/2;

  gl4dpSetColor(_blue);
  gl4dpLine(x1, y1, x2, y2);

  sgn = (sgn > 0 ? -1 : 1);
}