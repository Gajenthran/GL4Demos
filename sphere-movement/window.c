#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include "sphere.h"

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void         initGL(void);
static void         initData(void);
static void         resize(int w, int h);
static void         keyup(int keycode);
static void         keydown(int keycode);
static void         draw(void);
static void         quit(void);

enum { PR_SPH, SEC_SPH00, SEC_SPH01, SEC_SPH02, SEC_SPH03, SEC_SPH04, NB_SPH };

/*!\brief dimensions de la fenêtre */
static int _wW = 800, _wH = 600;
/*!\brief identifiant du programme GLSL */
static GLuint _pId = 0;
/*!\brief arrete l'animation */
static GLuint _pause = 0;
/*!\brief flag pour savoir si la touche shift est enfoncée */
static GLuint _shift = GL_FALSE;
/*!\brief flag pour activer le tourbillon */
static GLuint _swirl = 0;
/*!\brief les différents modes de vue */
static GLuint _mode = 0;
/*!\brief modifier les dimensions de la sphère*/
static GLuint _scale = 0;
/*!\brief rotation de la sphère */
static GLuint _rotate = 0;
/*!\brief rotation de la sphère */
static GLuint _grow = 0;
/*!\brief tableau des identifiants de texture à charger */
static GLuint _tId[NB_SPH] = {0};
/*!\brief les spheres */
static GLuint _sphere = {0};
/*!\brief nombre de spheres */
static int _nbSpheres = 100;

/*!\brief La fonction principale créé la fenêtre d'affichage,
 * initialise GL et les données, affecte les fonctions d'événements et
 * lance la boucle principale d'affichage.
 */
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "sphereMovement", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
			 _wW, _wH, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN))
    return 1;

  initGL();
  initData();
  atexit(quit);
  gl4duwResizeFunc(resize);
  gl4duwKeyUpFunc(keyup);
  gl4duwKeyDownFunc(keydown);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();

  return 0;
}

/*!\brief initialise les paramètres OpenGL */
static void initGL(void) {
  glEnable(GL_CULL_FACE);
  glEnable(GL_DEPTH_TEST);
  glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  _pId  = gl4duCreateProgram("<vs>shaders/sphere.vs", "<fs>shaders/sphere.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  resize(_wW, _wH);
}

/*!\brief initialise les données */
static void initData(void) {
  int i;
  SDL_Surface * t;
  static char * files[] = {
    "images/pr_sphere.png",
    "images/sec_sphere00.png",
    "images/sec_sphere01.png",
    "images/sec_sphere02.png", 
    "images/sec_sphere03.png", 
    "images/sec_sphere04.png"
  };

  glGenTextures(NB_SPH, _tId);
  for(i = 0; i < NB_SPH; i++) {
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
  mobileInit(_nbSpheres, NB_SPH-1, _wW, _wH);
}

/*!\brief Cette fonction paramétre la vue (viewport) OpenGL en
 * fonction des dimensions de la fenêtre.
 */
static void resize(int w, int h) {
  _wW  = w;
  _wH = h;
  glViewport(0, 0, _wW, _wH);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * _wH / _wW, 0.5 * _wH / _wW, 1.0, 1000.0);
  gl4duBindMatrix("modelViewMatrix");
}

static void keyup(int keycode) {
  switch(keycode) {
  case SDLK_RSHIFT:
  case SDLK_LSHIFT:
    _shift = GL_FALSE;
    break;
  default:
    break;
  }
}

static void keydown(int keycode) {
  GLint v[2];
  switch(keycode) {
  case 'a':
    _rotate = !_rotate;
    break;
  case 'g':
    _grow = !_grow;
    break;
  case 'm':
    _mode = !_mode;
    break;
  case SDLK_ESCAPE:
  case 'q':
    exit(0);
  case ' ':
    _pause = !_pause;
    break;
  case 'r':
    _scale = !_scale;
    break;
  case 's':
    _swirl = (_swirl + 1) % 5;
    break;
  case 'w':
    glGetIntegerv(GL_POLYGON_MODE, v);
    if(v[0] == GL_FILL)
      glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    else
      glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    break;
  case SDLK_RSHIFT:
  case SDLK_LSHIFT:
    _shift = GL_TRUE;
    break;
  default:
    break;
  }
}

/*!\brief dessine dans le contexte OpenGL actif. */
static void draw(void) {
  int i;
  static float r = 0.0, s = 0.0, g = 0.2;
  GLfloat * f = malloc(_nbSpheres * 8 * sizeof *f);
  GLfloat dt = 0.0;
  dt = SDL_GetTicks();

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();

  if(_mode == 0)
    gl4duTranslatef(0, 0, -1.5);
  else
    gl4duTranslatef(0, 0, -2);

  glUseProgram(_pId);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _tId[0]);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, _tId[1]);
  glUniform1i(glGetUniformLocation(_pId, "tex0"), 0);
  glUniform1i(glGetUniformLocation(_pId, "temps"), dt);
  glUniform1i(glGetUniformLocation(_pId, "swirl"), _swirl);

  /* envoi de toutes les matrices stockées par GL4D */
  mobile2texture(f);
  gl4duPushMatrix(); {

    gl4duRotatef(r, 0, 1, 0);
    gl4duTranslatef((((f[0] * 5) / _wW) - 2.5), (f[1] * 5 / _wH) - 2.5, 0); 
    gl4duScalef(f[2]  * cos(s), f[2]  * cos(s), f[2]  * cos(s)); 
    gl4duSendMatrices();
  } gl4duPopMatrix();
  gl4dgDraw(_sphere);
  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, 0);
  glUniform1i(glGetUniformLocation(_pId, "swirl"), 0);

  for(i = 1; i < _nbSpheres; i++) {
    int id = f[8 * i + 3];
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, _tId[id]);
    gl4duPushMatrix(); {
      gl4duTranslatef((((f[8 * i + 0] * 2) / _wW)), (f[8 * i + 1] * 2 / _wH), 0);  
      gl4duScalef(f[8 * i + 2] * g, f[8 * i + 2] * g, f[8 * i + 2] * g);
      gl4duSendMatrices();
    } gl4duPopMatrix();
    gl4dgDraw(_sphere);
  }

  if(_scale)
    s += .01;

  if(_rotate)
    r++;

  if(_grow && g < 1.5)
    g += 0.01;
  else if (!_grow && g > 0.2)
    g -= 0.01;
}

/*!\brief appelée au moment de sortir du programme (atexit), libère les éléments utilisés */
static void quit(void) {
  mobileClean();
  gl4duClean(GL4DU_ALL);
}

