#include "space.h"

#define ECHANTILLONS 1024
static Mix_Music * _mmusic = NULL;
static fftw_complex * _in4fftw = NULL, * _out4fftw = NULL;
static fftw_plan _plan4fftw = NULL;
static GLfloat _basses = 0, _aigus = 0;

static void quit(void) {
  if(_mmusic) {
    if(Mix_PlayingMusic())
      Mix_HaltMusic();
    Mix_FreeMusic(_mmusic);
    _mmusic = NULL;
  }
  Mix_CloseAudio();
  Mix_Quit();
  if(_plan4fftw) {
    fftw_destroy_plan(_plan4fftw);
    _plan4fftw = NULL;
  }
  if(_in4fftw) {
    fftw_free(_in4fftw); 
    _in4fftw = NULL;
  }
  if(_out4fftw) {
    fftw_free(_out4fftw); 
    _out4fftw = NULL;
  }

	spaceDelete();
  gl4duClean(GL4DU_ALL);
}

static void draw(void) {
  gl4dpClearScreenWith(RGB(0, 0, 0));
 	spaceDraw();
  gl4dpUpdateScreen(NULL);
}

static void idle(void) {
	spaceMove(_basses);
}

static void mixCallback(void *udata, Uint8 *stream, int len) {
  if(_plan4fftw) {
    int i, l = MIN(len >> 1, ECHANTILLONS);
    Sint16 *d = (Sint16 *)stream;
    for(i = 0; i < l; i++)
      _in4fftw[i][0] = d[i] / ((1 << 15) - 1.0);
    fftw_execute(_plan4fftw);
    for(i = 0, _basses = 0, _aigus = 0; i < l >> 2; i++) {
      if(i < l >> 3)
				_basses += sqrt(_out4fftw[i][0] * _out4fftw[i][0] + _out4fftw[i][1] * _out4fftw[i][1]);
      else
				_aigus  += sqrt(_out4fftw[i][0] * _out4fftw[i][0] + _out4fftw[i][1] * _out4fftw[i][1]);
    }

    _basses /= l >> 3;
    _aigus  /= l >> 3;
  }
}

static void initAudio(const char * filename) {
#if defined(__APPLE__)
  int mult = 1;
#else
  int mult = 2;
#endif
  int mixFlags = MIX_INIT_MP3, res;
  res = Mix_Init(mixFlags);
  if( (res & mixFlags) != mixFlags ) {
    fprintf(stderr, "Mix_Init: Erreur lors de l'initialisation de la bibliothèque SDL_Mixer\n");
    fprintf(stderr, "Mix_Init: %s\n", Mix_GetError());
  }

  if(Mix_OpenAudio(44100, AUDIO_S16LSB, 1, mult * ECHANTILLONS) < 0)
    exit(4);  
  if(!(_mmusic = Mix_LoadMUS(filename))) {
    fprintf(stderr, "Erreur lors du Mix_LoadMUS: %s\n", Mix_GetError());
    exit(5);
  }
  Mix_SetPostMix(mixCallback, NULL);
  if(!Mix_PlayingMusic())
    Mix_PlayMusic(_mmusic, 1);
}

static void init(const char * filename) {
  _in4fftw   = fftw_malloc(ECHANTILLONS *  sizeof *_in4fftw);
  memset(_in4fftw, 0, ECHANTILLONS *  sizeof *_in4fftw);
  assert(_in4fftw);
  _out4fftw  = fftw_malloc(ECHANTILLONS * sizeof *_out4fftw);
  assert(_out4fftw);
  _plan4fftw = fftw_plan_dft_1d(ECHANTILLONS, _in4fftw, _out4fftw, FFTW_FORWARD, FFTW_ESTIMATE);
  assert(_plan4fftw);
  initAudio(filename);
}

int main(int argc, char ** argv) {
  if(argc != 2) {
    fprintf(stderr, "usage: %s <audio_file>\n", argv[0]);
    return 2;
  }

  if(!gl4duwCreateWindow(argc, argv,
			 "space",
			 10, 10, 640, 480,
			 GL4DW_SHOWN)) {
    return 1;
  }

  init(argv[1]);
  atexit(quit);
  gl4dpInitScreen();
  gl4duwIdleFunc(idle);
  gl4duwDisplayFunc(draw);
  spaceInit(gl4dpGetWidth(), gl4dpGetHeight());
  gl4duwMainLoop();

  return 0;
}
