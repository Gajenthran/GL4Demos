#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
#include <iostream>
#include "Leap.h"

/* TODO
 *  - Zoom (mode vue)
 *  - gestuelles plus précises
 */


using namespace Leap;

static void init(void);
static void resize(int w, int h);
static int  isInCircle(int x, int y, int xo, int yo, int rad);
static void idle(void);
static void draw(void);
static void drawMobile(void);
static void drawHandSkeleton(void);
static void drawHand(void);
static void drawOptions(void);
static void quit(void);

/*!\brief enumération des gestuelles */
enum gesture_e {
  LM_PEN = 0,     /* geste pour le stylo */
  LM_SWIP_UP,     /* geste pour les deux mains vers le haut */
  LM_SWIP_DOWN,   /* geste pour les deux mains vers le bas */
  LM_ERASER,      /* geste pour la gomme */
  LM_OPTIONS,     /* geste pour les options */
  LM_TAP,         /* geste pour la selection des options */
  LM_ZOOM,         /* geste pour zoomer */
  LM_GESTURES     /* nombre de gestes */
};

/*!\brief enumération des doigts d'une main */
enum finger_e {
  LM_THUMB = 0,   /* pouce */
  LM_INDEX,       /* index */
  LM_MIDDLE,      /* majeur */
  LM_RING,        /* annulaire */
  LM_PINKY,       /* auriculaire */
  LM_FINGERS      /* nombre de doigts */
};

/*!\brief enumération des os sur chaque doigt */
enum bones_e {
  LM_FMETACARPAL, /* métacarpe */
  LM_FPROXIMAL,   /* phalange proximale */
  LM_FMIDDLE,     /* phalange intermédiaire */
  LM_FDISTAL,     /* phalange distale */
  LM_FBONES       /* nombre d'os */
};

/*!\brief largeur et hauteur de la fenêtre */
static int _wW = 1024, _wH = 768;
/*!\brief identifiant du GLSL program */
static GLuint _pId = 0;
/*!\brief identifiant de figures géométriques de GL4Dummies */
static GLuint _cylinder = 0, _sphere = 0, _cube = 0;
/*!\brief ensemble des gestuelles */
static int _lmGestures[LM_GESTURES] = {0};
/*!\brief informations concernant les deux mains */
static Hand _hands[2];
/*!\brief nombre de mains actives */
static int _nbHands = 0;
/*!\brief background color */
static GLfloat _color[] = {0.1, 0.1, 0.1, 1.0};

class lmListener : public Listener {
  public:
    virtual void onInit(const Controller&);
    virtual void onConnect(const Controller&);
    virtual void onDisconnect(const Controller&);
    virtual void onExit(const Controller&);
    virtual void onFrame(const Controller&);
    virtual void onFocusGained(const Controller&);
    virtual void onFocusLost(const Controller&);
    virtual void onDeviceChange(const Controller&);
    virtual void onServiceConnect(const Controller&);
    virtual void onServiceDisconnect(const Controller&);

  private:
};

void lmListener::onInit(const Controller& controller) {
  std::cout << "Initialized" << std::endl;
}

void lmListener::onConnect(const Controller& controller) {
  std::cout << "Connected" << std::endl;
  controller.enableGesture(Gesture::TYPE_CIRCLE);
  controller.enableGesture(Gesture::TYPE_KEY_TAP);
  controller.enableGesture(Gesture::TYPE_SCREEN_TAP);
  controller.enableGesture(Gesture::TYPE_SWIPE);
}

void lmListener::onDisconnect(const Controller& controller) {
  // Note: not dispatched when running in a debugger.
  std::cout << "Disconnected" << std::endl;
}

void lmListener::onExit(const Controller& controller) {
  std::cout << "Exited" << std::endl;
}

void lmListener::onFocusGained(const Controller& controller) {
  std::cout << "Focus Gained" << std::endl;
}

void lmListener::onFocusLost(const Controller& controller) {
  std::cout << "Focus Lost" << std::endl;
}

void lmListener::onDeviceChange(const Controller& controller) {
  std::cout << "Device Changed" << std::endl;
  const DeviceList devices = controller.devices();

  for (int i = 0; i < devices.count(); ++i) {
    std::cout << "id: " << devices[i].toString() << std::endl;
    std::cout << "isStreaming: " << (devices[i].isStreaming() ? "true" : "false") << std::endl;
  }
}

void lmListener::onServiceConnect(const Controller& controller) {
  std::cout << "Service Connected" << std::endl;
}

void lmListener::onServiceDisconnect(const Controller& controller) {
  std::cout << "Service Disconnected" << std::endl;
}

void lmListener::onFrame(const Controller& controller) {
  const Frame frame = controller.frame();
  _nbHands = frame.hands().count();
  HandList hands = frame.hands();
  _hands[0] = hands[0];
  _hands[1] = hands[1];
}

/*!\brief Initialise les paramètres OpenGL et les données */
static void init(void) {
  glClearColor(_color[0], _color[1], _color[2], 0.0f);
  _pId  = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  resize(_wW, _wH);
  _cylinder = gl4dgGenCylinderf(30, GL_TRUE);
  _sphere = gl4dgGenSpheref(30, 30);
  _cube = gl4dgGenCubef();
}

/*!\brief Paramètre la vue (viewport) OpenGL en
 * fonction des dimensions de la fenêtre.
 */
static void resize(int w, int h) {
  _wW  = w; _wH = h;
  glViewport(0, 0, _wW, _wH);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * _wH / _wW, 0.5 * _wH / _wW, 1.0, 1000.0);
  gl4duBindMatrix("modelViewMatrix");
}

/* !\brief Libère les éléments OpenGL utilisés */
static void quit(void) {
  gl4duClean(GL4DU_ALL);
}

static void idle(void) {
  for(int g = 0; g < LM_GESTURES; g++) {
    _color[0] = 0.1f; _color[1] = 0.1f; _color[2] = 0.1f;
    _lmGestures[g] = 0;
  }

  for(int i = 0; i < 2; i++) {
    FingerList fingers = _hands[i].fingers().extended();

    int nbExtendedFingers = fingers.count();

    // gestion de la gomme en fermant la main (LM)
    if(_hands[i].id() != -1 &&
       nbExtendedFingers == 0 &&
       _nbHands == 1)
      _lmGestures[LM_ERASER] = 1;

    // std::cout << "direction: " << _hands[0].direction()[2] << std::endl;
    // gestion d'options (LM)
    if(!_hands[0].isLeft() && 
       _hands[0].palmNormal()[2] > 0 && 
       nbExtendedFingers == LM_FINGERS)
      _lmGestures[LM_OPTIONS] = 1;

    for(int ef = 0; ef < nbExtendedFingers; ef++) {
      // gestion de sélection-options (LM)
      if(i == 1 &&
         _lmGestures[LM_OPTIONS] &&
         _hands[1].isLeft() &&
         nbExtendedFingers == 1 &&
         (int)fingers[ef].type() == LM_INDEX &&
         _hands[i].palmVelocity()[2] > 100)
        _lmGestures[LM_TAP] = 1;
    }

    // gestion de l'index levé (LM)
    if((int)fingers[i].type() == LM_INDEX && nbExtendedFingers == 1 && _nbHands == 1) {
      Bone::Type boneType = static_cast<Bone::Type>(LM_FDISTAL);
      Bone bone = fingers[i].bone(boneType);
      _lmGestures[LM_PEN] = 1;
    }
  }

  // gestion deux mains vers le haut (LM)
  float thresholdY = 100.0f;
  float diffY = fabs(_hands[0].palmPosition()[1] - _hands[1].palmPosition()[1]);
  float hPalmNormal = _hands[0].palmNormal()[1], h2PalmNormal = _hands[1].palmNormal()[1];
  float hPalmVelocity = _hands[0].palmVelocity()[1], h2PalmVelocity = _hands[1].palmVelocity()[1];

  if(diffY < thresholdY && 
     hPalmNormal > 0 && 
     h2PalmNormal > 0 && 
     hPalmVelocity > 500.0 && 
     h2PalmVelocity > 500.0) {
    _lmGestures[LM_SWIP_UP] = 1;
  } else if(diffY < thresholdY && 
          hPalmNormal < 0 && 
          h2PalmNormal < 0 &&
          -hPalmVelocity > 500.0 && 
          -h2PalmVelocity > 500.0) {
    _lmGestures[LM_SWIP_DOWN] = 1;
  }

  int h = 0;
  if(!_hands[h].isLeft())
    h = 1;

  FingerList fingers[2];
  fingers[0] = _hands[h].fingers().extended();
  fingers[1] = _hands[!h].fingers().extended();

  // gestion pour zoomer
  for(int f = 0; f < LM_FINGERS; f++) {
    if(fingers[0].count() == 1 && 
       fingers[1].count() == 1 &&
       (int)fingers[0][f].type() == LM_INDEX &&
       (int)fingers[1][f].type() == LM_INDEX) {
      if(_hands[0].palmVelocity()[0] < 100.0 &&
         _hands[0].palmVelocity()[1] > 100.0 &&
         _hands[1].palmVelocity()[0] > 100.0 &&
         _hands[1].palmVelocity()[1] < 100.0) {
        _lmGestures[LM_ZOOM] = 1;
      }
    }
  }

  if(!_lmGestures[LM_OPTIONS])
    return;

  // gestion pour la selection des options 
  for(int f = 0; f < LM_FINGERS; f++) {
    if((int)fingers[0][f].type() == LM_INDEX) {
      Bone::Type boneType = static_cast<Bone::Type>(LM_FDISTAL);
      Bone bone = fingers[0][f].bone(boneType);
      Vector indexPos = bone.nextJoint();

      if(isInCircle(
        _hands[0].palmPosition()[0] - 100.0f, _hands[0].palmPosition()[1] + 50.0f,
        indexPos[0], indexPos[1],
        10.0f)) {
        _color[0] = 1; _color[1] = 1; _color[2] = 0; 
      } else if(isInCircle(
                _hands[0].palmPosition()[0] - 75.0f, _hands[0].palmPosition()[1],
                indexPos[0], indexPos[1],
                10.0f)) {
        _color[0] = 0; _color[1] = 1; _color[2] = 0;
      } else if(isInCircle(
                _hands[0].palmPosition()[0] - 75.0f, _hands[0].palmPosition()[1] + 100.0f,
                indexPos[0], indexPos[1],
                10.0f)) {
        _color[0] = 0; _color[1] = 1; _color[2] = 1;
      }
    }
  }
}

/*!\brief Dessine dans le contexte OpenGL actif. */
static void draw(void) {
  glClearColor(_color[0], _color[1], _color[2], 1.0f);

  if(_lmGestures[LM_ERASER])
    std::cout << "ERASER ON" << std::endl;
  if(_lmGestures[LM_TAP])
    std::cout << "TAP ON" << std::endl;
  if(_lmGestures[LM_OPTIONS])
    std::cout << "OPTIONS ON" << std::endl;
  if(_lmGestures[LM_ZOOM])
    std::cout << "ZOOM ON" << std::endl;
  if(_lmGestures[LM_PEN])
    std::cout << "INDEX ON" << std::endl;
  if(_lmGestures[LM_SWIP_UP]) {
    std::cout << "SWIP-UP ON " << std::endl;
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);
  }
  if(_lmGestures[LM_SWIP_DOWN]) {
    std::cout << "SWIP-DOWN ON " << std::endl;
    glClearColor(0.5f, 0.5f, 0.5f, 0.0f);

  }

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  glUseProgram(_pId);

  if(_lmGestures[LM_OPTIONS])
    drawOptions();
  if(_lmGestures[LM_PEN])
    drawMobile();
  // drawHand();
  drawHandSkeleton();
}
  
static void drawOptions(void) {
  GLfloat green[] = {0, 1, 0, 1},
          cyan[] = {0, 1, 1, 1},
          yellow[] = {1, 1, 0, 1};

  gl4duPushMatrix(); {
    gl4duTranslatef(
      (_hands[0].palmPosition()[0] - 75.0) * 0.02,
      _hands[0].palmPosition()[1] * 0.02 - 3.0f,
      -12.0);
    gl4duScalef(0.25, 0.25, 0.25);
    gl4duSendMatrices();
    glUniform4fv(glGetUniformLocation(_pId, "color"), 1, green);
    gl4dgDraw(_sphere);
  } gl4duPopMatrix();

  gl4duPushMatrix(); {
    gl4duTranslatef(
      (_hands[0].palmPosition()[0] - 100.0) * 0.02,
      (_hands[0].palmPosition()[1] + 50.0f) * 0.02 - 3.0f,
      -12.0);
    gl4duScalef(0.25, 0.25, 0.25);
    gl4duSendMatrices();
    glUniform4fv(glGetUniformLocation(_pId, "color"), 1, yellow);
    gl4dgDraw(_sphere);
  } gl4duPopMatrix();

  gl4duPushMatrix(); {
    gl4duTranslatef(
      (_hands[0].palmPosition()[0] - 75.0) * 0.02,
      (_hands[0].palmPosition()[1] + 100.0f) * 0.02 - 3.0f,
      -12.0);
    gl4duScalef(0.25, 0.25, 0.25);
    gl4duSendMatrices();
    glUniform4fv(glGetUniformLocation(_pId, "color"), 1, cyan);
    gl4dgDraw(_sphere);
  } gl4duPopMatrix();
}

/*!\brief Vérifie si le point appartient au cercle */
static int isInCircle(int x, int y, int xo, int yo, int rad) {
  if(sqrt(((x - xo) * (x - xo)) + ((y - yo) * (y - yo))) < rad)
    return 1;
  return 0;
}

/*!\brief Dessine la sphère qui bougera en fonction du doigt
 * (index) de l'utilisateur */
static void drawMobile(void) {
  GLfloat red[] = {1, 0, 0, 1};

  if(_hands[0].id() == -1)
    return;

  const FingerList fingers = _hands[0].fingers().extended();
  for(int f = 0; f < LM_FINGERS; f++) {
    if((int)fingers[f].type() == LM_INDEX) {
      Bone::Type boneType = static_cast<Bone::Type>(LM_FDISTAL);
      Bone bone = fingers[f].bone(boneType);
      Vector indexPos = bone.nextJoint();

      gl4duPushMatrix(); {
        gl4duTranslatef(
          indexPos[0] * 0.02,
          indexPos[1] * 0.02 - 3.0f,
          -10.0);
        gl4duSendMatrices();
        glUniform4fv(glGetUniformLocation(_pId, "color"), 1, red);
        gl4dgDraw(_sphere);
      } gl4duPopMatrix();
    }
  }
}

/*!\brief Dessine les mains de l'utilisateur lorsqu'elles
 * sont dans le champs de la Leap Motion et sont "levées" 
 */
static void drawHand(void) {
  GLfloat white[] = {1, 1, 1, 1};

  for(int i = 0; i < 2; i++) {
    if(_hands[i].id() == -1)
      continue;
    const FingerList fingers = _hands[i].fingers().extended();
    for(int f = 0; f < LM_FINGERS; f++) {
      for(int b = 0; b < LM_FBONES; b++) {
        Bone::Type boneType = static_cast<Bone::Type>(b);
        Bone bone = fingers[f].bone(boneType);
        Vector boneCoord = (bone.prevJoint() + bone.nextJoint()) * 0.5;
        gl4duPushMatrix(); {
          gl4duTranslatef(
            boneCoord[0] * 0.02,
            boneCoord[1] * 0.02 - 3.0f,
            -10.0);
          gl4duScalef(0.1, 0.1, 0.1);
          gl4duSendMatrices();
          glUniform4fv(glGetUniformLocation(_pId, "color"), 1, white);
          gl4dgDraw(_sphere);
        } gl4duPopMatrix();
      }
    }
  }
}

/*!\brief Dessine les mains de l'utilisateur lorsqu'elles
 * sont dans le champs de la Leap Motion et sont "levées" 
 */
static void drawHandSkeleton(void) {
  GLfloat white[] = {1, 1, 1, 1};
  float jointScale = 0.75f, boneScale = 0.5f, palmScale = 1.15f;

  for(int i = 0; i < 2; i++) {
    Vector palmPos = _hands[i].palmPosition();
    Vector palmDir = _hands[i].direction();
    Vector palmNorm = _hands[i].palmNormal();
    Vector palmSide = palmDir.cross(palmNorm).normalized();

    const float thumbDist = _hands[i].fingers()[Finger::TYPE_THUMB].bone(Bone::TYPE_METACARPAL).prevJoint().distanceTo(_hands[i].palmPosition());
    const Vector wrist = palmPos - thumbDist * (palmDir*0.90f + (_hands[i].isLeft() ? -1.0f : 1.0f) * palmSide * 0.38f);

    FingerList fingers = _hands[i].fingers();

    float fingerRad = 0.0f;
    Vector vCurBoxBase;
    Vector vLastBoxBase = wrist;

    for (int i = 0, ei = fingers.count(); i < ei; i++) {
      const Finger& finger = fingers[i];
      fingerRad = finger.width() * 0.5f;

      // dessine chaque doigt de la main
      for (int j = Bone::TYPE_METACARPAL; j <= Bone::TYPE_DISTAL; j++) {
        const Bone& bone = finger.bone(static_cast<Bone::Type>(j));

        if (j == Bone::TYPE_METACARPAL) {
           vCurBoxBase = bone.nextJoint();
        } else {
          Vector boneCoord = (bone.prevJoint() + bone.nextJoint()) * 0.5;
          gl4duPushMatrix(); {
            gl4duTranslatef(
              boneCoord[0] * 0.02,
              boneCoord[1] * 0.02 - 3.0f,
              -10.0);
            gl4duScalef(
            boneScale * fingerRad * 0.02,
            boneScale * fingerRad * 0.02,
            boneScale * fingerRad * 0.02);
            gl4duSendMatrices();
            glUniform4fv(glGetUniformLocation(_pId, "color"), 1, white);
            gl4dgDraw(_cylinder);
          } gl4duPopMatrix();

          gl4duPushMatrix(); {
            gl4duTranslatef(
              bone.nextJoint()[0] * 0.02,
              bone.nextJoint()[1] * 0.02 - 3.0f,
              -10.0);
            gl4duScalef(
              jointScale * fingerRad * 0.02, 
              jointScale * fingerRad * 0.02, 
              jointScale * fingerRad * 0.02);
            gl4duSendMatrices();
            glUniform4fv(glGetUniformLocation(_pId, "color"), 1, white);
            gl4dgDraw(_sphere);
          } gl4duPopMatrix();
        }
      }

      Vector vbox = (vCurBoxBase + vLastBoxBase) * 0.5;
      gl4duPushMatrix(); {
        gl4duTranslatef(
          vbox[0] * 0.02,
          vbox[1] * 0.02 - 3.0f,
          -10.0);
        gl4duScalef(
          boneScale * fingerRad * 0.02,
          boneScale * fingerRad * 0.02,
          boneScale * fingerRad * 0.02);
        gl4duSendMatrices();
        glUniform4fv(glGetUniformLocation(_pId, "color"), 1, white);
        gl4dgDraw(_cylinder);
      } gl4duPopMatrix();

      gl4duPushMatrix(); {
        gl4duTranslatef(
          vCurBoxBase[0] * 0.02,
          vCurBoxBase[1] * 0.02 - 3.0f,
          -10.0);
        gl4duScalef(
          jointScale * fingerRad * 0.02,
          jointScale * fingerRad * 0.02,
          jointScale * fingerRad * 0.02);
        gl4duSendMatrices();
        glUniform4fv(glGetUniformLocation(_pId, "color"), 1, white);
        gl4dgDraw(_sphere);
      } gl4duPopMatrix();
      vLastBoxBase = vCurBoxBase;
    }

    Vector vbox = (vCurBoxBase + vLastBoxBase) * 0.5;
    fingerRad = fingers[Finger::TYPE_THUMB].width() * 0.5f;
    vCurBoxBase = wrist;
    gl4duPushMatrix(); {
      gl4duTranslatef(
        vbox[0] * 0.02,
        vbox[1] * 0.02 - 3.0f,
        -10.0);
      gl4duScalef(
        boneScale * fingerRad * 0.02, 
        boneScale * fingerRad * 0.02, 
        boneScale * fingerRad * 0.02);
      gl4duSendMatrices();
      glUniform4fv(glGetUniformLocation(_pId, "color"), 1, white);
      gl4dgDraw(_cylinder);
    } gl4duPopMatrix();

    gl4duPushMatrix(); {
      gl4duTranslatef(
        vCurBoxBase[0] * 0.02,
        vCurBoxBase[1] * 0.02 - 3.0f,
        -10.0);
      gl4duScalef(
        jointScale * fingerRad * 0.02,
        jointScale * fingerRad * 0.02,
        jointScale * fingerRad * 0.02);
      gl4duSendMatrices();
      glUniform4fv(glGetUniformLocation(_pId, "color"), 1, white);
      gl4dgDraw(_sphere);
    } gl4duPopMatrix();

    // dessine paume de la main
    gl4duPushMatrix(); {
      gl4duTranslatef(
        palmPos[0] * 0.02,
        palmPos[1] * 0.02 - 3.0f,
        -10.0);
      gl4duScalef(
        palmScale * fingerRad * 0.02,
        palmScale * fingerRad * 0.02,
        palmScale * fingerRad * 0.02);
      gl4duSendMatrices();
      glUniform4fv(glGetUniformLocation(_pId, "color"), 1, white);
      gl4dgDraw(_sphere);
    } gl4duPopMatrix();
  }
}

int main(int argc, char** argv) {
  if(!gl4duwCreateWindow(argc, argv, "GL4D with LMotion", 
      0, 0, 
      _wW, _wH,
      GL4DW_RESIZABLE | GL4DW_SHOWN)) {
    return 1;
  }

  lmListener listener;
  Controller controller;

  controller.addListener(listener);

  if(argc > 1 && strcmp(argv[1], "--bg") == 0)
    controller.setPolicy(Leap::Controller::POLICY_BACKGROUND_FRAMES);

  init();
  atexit(quit);
  gl4duwResizeFunc(resize);
  gl4duwDisplayFunc(draw);
  gl4duwIdleFunc(idle);
  gl4duwMainLoop();

  controller.removeListener(listener);
  return 0;
}