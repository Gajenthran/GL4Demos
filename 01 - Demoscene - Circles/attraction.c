#include <assert.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <GL4D/gl4dh.h>
#include "audioHelper.h"

#define NTEXTURES 12

typedef struct mobile_t mobile_t;
struct mobile_t {
  double x, y;
  float r;
  int alive;
  int texId;
};

static void         init(int w, int h);
static void         initGL(void);
static void         initData(void);
static void         mobileInit(void);
static void         draw(void);
static float        myRand(float max);
static void         mobile2texture(float * f);
static int          distance(int x0, int y0, int x1, int y1);
static int          bestMove(mobile_t mobile);
static void         quit(void);

enum { NOTHING, LEFT, LEFT_UP, UP, RIGHT_UP, RIGHT, RIGHT_DOWN, DOWN, LEFT_DOWN, NDIR };
enum { TEX_STAR00, TEX_STAR01, TEX_STAR02, TEX_STAR03, TEX_STAR04 };

static int _w, _h;
static GLuint _screen = 0;
static GLuint _pId = 0;
static GLuint _tId[NTEXTURES] = {0};

static GLuint _sphere = {0};
static GLuint _torus = 0;
static int _torusRotate = 0;

static int _state = 0;
static int _scale = 0;
static int _swirl = 0;
static int _direction = 0;

static int _nb_mobiles = 220;
static int _cur_mobile = 1;
static int _white_mobiles = 1;
static mobile_t * _mobile = NULL;
static int _cur_id = 0;

static float _basses = 0;
static float _aigus = 0;
static int _dirx[] = { 0, -1, -1, 0, 1, 1,  1,  0, -1 };
static int _diry[] = { 0,  0,  1, 1, 1, 0, -1, -1, -1 };

static float myRand(float max) {
  float rand = gl4dmURand() * max;
  float sgn = gl4dmURand() * 2.0;
  sgn = sgn > 1.0 ? 1.0 : -1.0;
  rand *= sgn;
  return rand;
}

static void initGL(void) {
  _pId  = gl4duCreateProgram("<vs>shaders/attraction.vs", "<fs>shaders/attraction.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
}

static void mobileInit(void) {
  int i;
  float a;
  srand(time(NULL));
  if(_mobile) {
    free(_mobile);
    _mobile = NULL;
  }

  _mobile = malloc(_nb_mobiles * sizeof *_mobile);
  assert(_mobile);

  if(_mobile) {
    free(_mobile);
    _mobile = NULL;
  }
  _mobile = malloc(_nb_mobiles * sizeof *_mobile);
  assert(_mobile);

  _mobile[0].r = 200;
  _mobile[0].x = _mobile[0].y = 0;
  _mobile[0].alive = 0;

  for(i = 5; i < 7; i++) {
    for(a = 0; a < (2 * M_PI); a += 0.1) {
      _mobile[_cur_mobile].x = _w/(i-2) * cos(a);
      _mobile[_cur_mobile].y = _w/(i-2) * sin(a);
      _mobile[_cur_mobile].r = 20.0;
      _mobile[_cur_mobile].texId = 11;
      if(_cur_mobile < _nb_mobiles)
        _cur_mobile++; 
    }
  }

  for(i = _cur_mobile; i < _nb_mobiles; i++) {
    _mobile[i].r = (!i ? 200.0 : myRand(100));
    _mobile[i].x = (!i ? 0 : myRand(1) * _w/2);
    _mobile[i].y = (!i ? 0 : myRand(1) * _h/2);
    _mobile[i].alive = 1;
    _mobile[i].texId = gl4dmURand() * 10 + 1;
  }
  _white_mobiles = _cur_mobile;
  _cur_mobile = 1;
}

static void initData(void) {
  int i;
  SDL_Surface * t;
  static char * files[] = {
    "images/star00.png", 
    "images/star001.png", 
    "images/star002.png", 
    "images/star003.png", 
    "images/star004.png", 
    "images/star005.png", 
    "images/star006.png", 
    "images/star01.png", 
    "images/star02.png", 
    "images/star03.png",
    "images/star04.png",
    "images/star05.png",
    //"images/ring.png",
  };
  glGenTextures(NTEXTURES, _tId);
  for(i = 0; i < NTEXTURES; i++) {
    glBindTexture(GL_TEXTURE_2D, _tId[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    if( (t = IMG_Load(files[i])) != NULL ) {
      int mode = t->format->BytesPerPixel == 4 ? GL_BGRA : GL_BGR;
      mode = t->format->BytesPerPixel == 4 ? GL_RGBA : GL_RGB;     
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->w, t->h, 0, mode, GL_UNSIGNED_BYTE, t->pixels);
      SDL_FreeSurface(t);
    } else {
      fprintf(stderr, "can't open file %s : %s\n", files[i], SDL_GetError());
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    }
  }
  _sphere = gl4dgGenSpheref(30, 30);
  _torus = gl4dgGenTorusf(300, 30, 0.1f);

  mobileInit();
}

static void mobile2texture(float * f) {
  int i;
  for(i = 0; i < _nb_mobiles; i++) {
    f[8 * i + 0] = _mobile[i].x; 
    f[8 * i + 1] = _mobile[i].y;
    f[8 * i + 2] = _mobile[i].r / ((GLfloat)MIN(_w, _h));
    f[8 * i + 3] = 1;
  }
}

static int distance(int x0, int y0, int x1, int y1) {
  return sqrt((x1 - x0) * (x1 - x0) + (y1 - y0) * (y1 - y0));
}

static int bestMove(mobile_t mobile) {
  int i, cur, move = (int)(_w * _h), dir = NOTHING;
  for(i = 0; i < NDIR; i++) {
    cur = distance(mobile.x + _dirx[i], mobile.y + _diry[i], _mobile[0].x, _mobile[0].y);
    if(cur < move) {
      move = cur;
      dir = i;
    }
  }
  return dir;
}

static void mobileMove(void) {
  int dt = SDL_GetTicks();
  int m;
  float dx, dy, d;
  static int rtime;
  if(dt/1500 != _state) {
    _direction = (_direction < 0 ? 1 : -1);
    _state = dt/1500;
    _cur_mobile = gl4dmURand() * _white_mobiles;
    _cur_id = (_cur_id + 1) % 7;
    rtime++;
  }

  m = bestMove(_mobile[_cur_mobile]);
  _mobile[_cur_mobile].x += _dirx[m] * 10;
  _mobile[_cur_mobile].y += _diry[m] * 10;
  dx = _mobile[_cur_mobile].x - _mobile[0].x;
  dy = _mobile[_cur_mobile].y - _mobile[0].y;
  d = _mobile[_cur_mobile].r + _mobile[0].r;
  if( ((int)(dx * dx) / (10 * 10)) == 0 &&
      ((int)(dy * dy) / (10 * 10)) == 0 &&
      _mobile[_cur_mobile].alive) {
    _mobile[_cur_mobile].alive = 0;
    _mobile[0].r += 10;
    _mobile[_cur_mobile].r = 0;
  }

  if(rtime >= 11) {
    _swirl = 1;
    _torusRotate = 1;
  }
}


static void init(int w, int h) {
  _w = w; _h = h;
  initGL();
  initData();
}

static void draw(void) {
  int i;
  GLint vp[4];
  GLfloat *mat;
  GLfloat dt = 0.0;
  GLfloat * f = malloc(_nb_mobiles * 8 * sizeof *f);
  static float lf = 0.0;
  static GLfloat a = 0.0, s[2] = { 0, 0 };


  if(_scale) {
    if(s[1] - 0.01 > 0.02) {
        s[1] -= 0.01;
    }
    else{
      s[1] = 0.01;
    }

    if(s[0] + 0.01 < 20.0) {
      s[0] += 0.05;
    }
  }
  GLboolean gdt = glIsEnabled(GL_DEPTH_TEST);
  glGetIntegerv(GL_VIEWPORT, vp);
  dt = SDL_GetTicks();
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * vp[3] / vp[2], 0.5 * vp[3] / vp[2], 1.0, 1000.0);
  gl4duBindMatrix("modelViewMatrix");

  gl4duLoadIdentityf();
    gl4duTranslatef(0, 0, -2.5);

  if(!_scale) glClearColor(0.13f, 0.14f, 0.60f, 0.0f);
  else glClearColor(0.0f, 0.0f, 0.0, 0.0f);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  mat = gl4duGetMatrixData();
  glDisable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_BLEND);
  glUseProgram(_pId);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tId[_cur_id]);
  glUniform1i(glGetUniformLocation(_pId, "tex0"), 0);
  glUniform1i(glGetUniformLocation(_pId, "temps"), dt);
  glUniform1i(glGetUniformLocation(_pId, "swirl"), _swirl);
  glUniform1i(glGetUniformLocation(_pId, "direction"), _direction);

  mobileMove();
  mobile2texture(f);
  if(!_scale) {
    s[0] = f[2];
    s[1] = f[2];
  }

  gl4duPushMatrix(); {
    gl4duRotatef(0, 0, 1, 0);
    gl4duTranslatef((f[0] * 3) / _w, (f[1] * 2) / _h, 0); 
    gl4duScalef(s[0] , s[1], f[2]); 
    gl4duSendMatrices();
  } gl4duPopMatrix();
  gl4dgDraw(_sphere);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, 0);

  if(!_scale) {
    if(_torusRotate) {
      gl4duSendMatrices();
      gl4duPushMatrix(); {
        gl4duTranslatef(0, 0.0, 0.0);
        gl4duRotatef(-1 * a+1, -3 * a+2, -3 * a+3, -1);
        gl4duScalef(0.6f, 0.6f, 0.6f);
        gl4duSendMatrices();
      } gl4duPopMatrix();
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, _tId[_cur_id]);      
      gl4dgDraw(_torus);
    }
  }

  if(!_scale) {
    for(i = 1; i < _nb_mobiles; i++) {
      glActiveTexture(GL_TEXTURE0);
      glBindTexture(GL_TEXTURE_2D, _tId[_mobile[i].texId]);
      gl4duPushMatrix(); {
        gl4duTranslatef((((f[8 * i + 0] * 3) / _w)), (f[8 * i + 1] * 2) / _h, 0);  
        gl4duScalef(f[8 * i + 2] * 0.2, f[8 * i + 2] * 0.2, f[8 * i + 2] * 0.2);
        gl4duSendMatrices();
      } gl4duPopMatrix();
      gl4dgDraw(_sphere);
    }
  }

  a+= 3;

  if((_basses < 5.0) && _aigus < 1.0 && _torusRotate) {
    _scale = 1;
  }

  lf = _basses;
  if(!gdt)
    glDisable(GL_DEPTH_TEST);
}

static void quit(void) {
  _nb_mobiles = 0;
  if(_mobile) {
    free(_mobile);
    _mobile = NULL;
  }

  if(_tId[0]) {
    glDeleteTextures(NTEXTURES, _tId);
    _tId[0] = 0;
  }

  if(_screen) {
    gl4dpSetScreen(_screen);
    gl4dpDeleteScreen();
    _screen = 0;
  }
}


void attraction(int state) {
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
    _aigus = ahGetAudioStreamAigus();
    return;
  default:
    draw();
    return;
  }
}
