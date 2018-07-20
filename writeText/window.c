#include <stdio.h>
#include <assert.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4dp.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_ttf.h>

/* Texte de max. 1024 caractères */
#define NTEXTS 1024

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
static void resize(int w, int h);
static void draw(void);
static void quit(void);
static void initText(GLuint * ptId, const char * text);
static void keydown(int keycode);
static void keyup(int keycode);

/*!\brief dimensions de la fenêtre */
static int _wW = 800, _wH = 600;
/*!\brief taille de la police */
static int _fontSize = 128;
/*!\brief taille de la surface où on peut écrire */
static int _surfaceSize = 1024;
/*!\brief identifiant du programme GLSL */
static GLuint _pId = 0;
/*!\brief identifiant de la géométrie d'un plan */
static GLuint _quad = 0;
/*!\brief identifiant de la texture contenant le texte */
static GLuint _textTexId = 0;
/* !\brief chaîne de caractères où l'utilisateur peut écrire */
static char _wrText[NTEXTS] = "";
/* !\brief indice de la chaîne de caractère */
static unsigned int iTxt = 0;
/* !\brief lettre entrée par l'utilisateur sur son clavier */
static char _letter;
/* !\brief activation de la majuscule */
static int _capsLock = 0;
/* !\brief activation de l'effacement */
static int _delete = 0;

/*!\brief La fonction principale créé la fenêtre d'affichage,
 * initialise GL et les données, affecte les fonctions d'événements et
 * lance la boucle principale d'affichage.*/
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "writeText", 0, 0, 
       _wW, _wH, GL4DW_RESIZABLE | GL4DW_SHOWN))
    return 1;
  init();
  atexit(quit);
  gl4duwResizeFunc(resize);
  gl4duwKeyUpFunc(keyup);
  gl4duwKeyDownFunc(keydown);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}

/*!\brief initialise les paramètres OpenGL et les données */
static void init(void) {
  glEnable(GL_DEPTH_TEST);
  glClearColor(1.0f, 0.7f, 0.7f, 0.0f);
  _pId = gl4duCreateProgram("<vs>shaders/text.vs", "<fs>shaders/text.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  resize(_wW, _wH);
  _quad = gl4dgGenQuadf();
  /* initText(&_textTexId, 
     "Totoooooooooooop\nddqsjofjqso"); */
}

static void keyup(int keycode) {
  switch(keycode) {
  default:
    break;
  }
}

static void keydown(int keycode) {
  switch(keycode) {
    case SDLK_CAPSLOCK:
      _capsLock = !_capsLock;
      break;
    case SDLK_RETURN:
      _letter = '\n';
      break;
    case SDLK_BACKSPACE:
      _delete = 1;
      break;
  }
  
  if((keycode >= 'a' && keycode <= 'z') || 
     (keycode >= '0' && keycode <= '9'))
    _letter = (_capsLock ? keycode - 32 : keycode);
}

/*!\brief Cette fonction paramétre la vue (viewport) OpenGL en
 * fonction des dimensions de la fenêtre.*/
static void resize(int w, int h) {
  _wW  = w; _wH = h;
  glViewport(0, 0, _wW, _wH);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-1, 1, -_wH / (GLfloat)_wW, _wH / (GLfloat)_wW, 1, 1000);
  gl4duBindMatrix("modelViewMatrix");
}

static void writeText(void) {
  if(_delete && iTxt > 0) {
    _wrText[--iTxt] = ' '; 
    _delete = 0;
  } else _delete = 0;

  if(_letter != '\0' && iTxt+1 < NTEXTS) {
    _wrText[iTxt++] = _letter;
    _letter = '\0';
    if(iTxt+1 == NTEXTS) _wrText[iTxt+1] = '\0';
  }
    
  initText(&_textTexId, _wrText);

}

/*!\brief dessine dans le contexte OpenGL actif. */
static void draw(void) {
  writeText();
  static GLfloat t0 = -1;
  GLfloat t, d;
  GLint vp[4];
  GLboolean gdt = glIsEnabled(GL_DEPTH_TEST);
  glGetIntegerv(GL_VIEWPORT, vp);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * vp[3] / vp[2], 0.5 * vp[3] / vp[2], 1.0, 1000.0);
  gl4duBindMatrix("modelViewMatrix");

  gl4duLoadIdentityf();
    gl4duTranslatef(1.2, 0, -2);
  if(t0 < 0.0f)
    t0 = SDL_GetTicks();
  t = (SDL_GetTicks() - t0) / 1000.0f, d = -2.4f + 0.40f * t;
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glUseProgram(_pId);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _textTexId);
  glUniform1i(glGetUniformLocation(_pId, "inv"), 1);
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  gl4duTranslatef(0, 0, -2);
  gl4duScalef(0.5, .5, .5);
  gl4duSendMatrices();
  gl4dgDraw(_quad);
  glUseProgram(0);

  if(!gdt)
    glDisable(GL_DEPTH_TEST);
}

/*!\brief appelée au moment de sortir du programme (atexit), libère les éléments utilisés */
static void quit(void) {
  if(_textTexId) {
    glDeleteTextures(1, &_textTexId);
    _textTexId = 0;
  }
  gl4duClean(GL4DU_ALL);
}

/*!\brief création d'une texture avec du texte. 
 *
 * \param ptId pointeur vers la variable stockant l'identifiant de
 * texture OpenGL. Si l'identifiant existe déjà (!= 0) l'utiliser
 * sinon (== 0) en générer un nouveau.
 *
 * \param text pointeur vers la chaine de caractères contenant le
 * texte.
 */
static void initText(GLuint * ptId, const char * text) {
  static int firstTime = 1;
  SDL_Color c = { 255, 255, 255, 255 };
  SDL_Surface * d, *s;
  TTF_Font * font = NULL;
  if(firstTime) {
    /* initialisation de la bibliothèque SDL2 ttf */
    if(TTF_Init() == -1) {
      fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
      exit(2);
    }
    firstTime = 0;
  }
  if(*ptId == 0) {
    /* initialisation de la texture côté OpenGL */
    glGenTextures(1, ptId);
    glBindTexture(GL_TEXTURE_2D, *ptId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }
  /* chargement de la font */
  if( !(font = TTF_OpenFont("Anonymous_Pro_B.ttf", _fontSize)) ) {
    fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
    return;
  }
  /* création d'une surface SDL avec le texte */
  d = TTF_RenderUTF8_Blended_Wrapped(font, text, c, _surfaceSize);
  if(d == NULL) {
    TTF_CloseFont(font);
    return;
  }

  /* copie de la surface SDL vers une seconde aux spécifications qui correspondent au format OpenGL */
  s = SDL_CreateRGBSurface(0, d->w, d->h, 32, R_MASK, G_MASK, B_MASK, A_MASK);
  assert(s);
  SDL_BlitSurface(d, NULL, s, NULL);
  SDL_FreeSurface(d);
  /* transfert vers la texture OpenGL */
  glBindTexture(GL_TEXTURE_2D, *ptId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
  fprintf(stderr, "Dimensions de la texture : %d %d\n", s->w, s->h);

  SDL_FreeSurface(s);
  TTF_CloseFont(font);
  glBindTexture(GL_TEXTURE_2D, 0);
}
