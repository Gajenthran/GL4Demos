#include "mobile.h"

static void init(void);
static void resize(int w, int h);
static void keydown(int keycode);
static void draw(void);
static void quit(void);

/*!\brief window's width and height */
static int _windowWidth = 600, _windowHeight = 600;
/*!\brief GLSL program Id */
static GLuint _pId = 0;
static int _basses = 0;
/*!\brief A generated Quad Id */
static GLuint _quad = 0;

/*!\brief main function, creates the window, initialise OpenGL
 *  parameters and objects, sets GL4Dummies callback function and
 *  starts the main loop.
 */
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "attraction", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
			 _windowWidth, _windowHeight, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN))
    return 1;
  init();
  atexit(quit);
  gl4duwResizeFunc(resize);
  gl4duwKeyDownFunc(keydown);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}

/*!\brief initialise OpenGL parameters and data. */
static void init(void) {
  /* setting OpenGL clear color (for next glClear) */
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  /* loading and compiling a GLSL program composed with a vertex shader and a fragment shader */
  _pId = gl4duCreateProgram("<vs>shaders/attraction.vs", "<fs>shaders/attraction.fs", NULL);
  /* setting OpenGL viewPort by calling resize (see below) */
  resize(_windowWidth, _windowHeight);
  /* generating a Quad (Vertex coords + Vertex Normal + Vertex TexCoord */
  _quad = gl4dgGenQuadf();
  /* creating a 1D texture to store mobile coords */
  glBindTexture(GL_TEXTURE_1D, 0);
  /* calling mobileInit */
}

/*!\brief sets the OpenGL viewport according to the window width and height.
 */
static void resize(int w, int h) {
  _windowWidth  = w;
  _windowHeight = h;
  glViewport(0, 0, _windowWidth, _windowHeight);
}

/*!\brief manages keyboard-down event */
static void keydown(int keycode) {
  switch(keycode) {
  case SDLK_ESCAPE:
  case 'q':
    exit(0);
  }
}

/*!\brief mobile simulation and draw
 */
static void draw(void) {
  static GLfloat onde = 3.0;
  int time = SDL_GetTicks();
  glDisable(GL_DEPTH_TEST);
  glUseProgram(_pId);

  glClear(GL_COLOR_BUFFER_BIT);  
  glUniform1i(glGetUniformLocation(_pId, "time"), time);
  glUniform1i(glGetUniformLocation(_pId, "basses"), _basses);

  gl4dgDraw(_quad);
  glBindVertexArray(0);
  glBindTexture(GL_TEXTURE_1D, 0);
  glUseProgram(0);
  onde -= 0.01;
  if(onde < 0) onde = 3.0;
}

/*!\brief called at exit and delete and clean used data. */
static void quit(void) {
  gl4duClean(GL4DU_ALL);
}
