/******************************************************************************\
* Copyright (C) 2012-2014 Leap Motion, Inc. All rights reserved.               *
* Leap Motion proprietary and confidential. Not for distribution.              *
* Use subject to the terms of the Leap Motion SDK Agreement available at       *
* https://developer.leapmotion.com/sdk_agreement, or another agreement         *
* between Leap Motion and you, your company or other organization.             *
\******************************************************************************/


#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4df.h>
#include <GL4D/gl4dfBlurWeights.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <GL4D/gl4dh.h>
#include <iostream>
#include <cstring>
#include "Leap.h"

/*
 * TODO:
 * Indiquer les mains (isLeft)
 * Boucle for inutile
 */
using namespace Leap;

static void init(void);
static void resize(int w, int h);
static void draw(void);
static void quit(void);

typedef struct hand hand_t;

struct hand {
  Vector palmPosition;
  Vector indexPosition;
};


enum gesture_e {
  LM_PEN = 0,
  LM_SWIP_UP,
  LM_SWIP_DOWN,
  LM_ERASER,
  LM_OPTIONS,
  LM_TAP,
  LM_GESTURES
};

enum finger_e {
  LM_THUMB = 0,
  LM_INDEX,
  LM_MIDDLE,
  LM_RING,
  LM_PINKY,
  LM_FINGERS
};

enum bones_e {
  LM_FMETACARPAL,
  LM_FPROXIMAL,
  LM_FMIDDLE,
  LM_FDISTAL,
  LM_FBONES
};

static int _wW = 1024, _wH = 768;
static GLuint _pId = 0;
static hand_t _hand[2];
static GLuint _cylinder = 0, _sphere = 0, _cube = 0;
static int _lmGestures[LM_GESTURES] = {0};

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
  for(int i = 0; i < LM_GESTURES; i++) {
    _lmGestures[i] = 0;
  }

  // Get the most recent frame and report some basic information
  const Frame frame = controller.frame();
  /* std::cout << "Frame id: " << frame.id()
            << ", timestamp: " << frame.timestamp()
            << ", hands: " << frame.hands().count()
            << ", extended fingers: " << frame.fingers().extended().count()
            << ", tools: " << frame.tools().count()
            << ", gestures: " << frame.gestures().count() << std::endl; */

  int nbHands = frame.hands().count();
  HandList hands = frame.hands();

  Vector lhandPalmPosition, lhandPalmNormal;

  // TODO: boucle for à retirer !
  for (HandList::const_iterator hl = hands.begin(); hl != hands.end(); ++hl) {
    // Get the first hand
    const Hand hand = hands[0];
    const Hand hand2 = hands[1];
  
    // std::cout << hand.direction()[2] << std::endl;
    // std::cout << hand.palmVelocity()[2] << std::endl;

    _hand[0].palmPosition = hand.palmPosition();
    _hand[1].palmPosition = hand2.palmPosition();

    /* std::string handType = hand.isLeft() ? "Left hand" : "Right hand";
    std::cout << std::string(2, ' ') << handType << ", id: " << hand.id()
              << ", palm position: " << hand.palmPosition() << std::endl; */
    
    // Get the hand's normal vector and direction
    // const Vector direction = hand.direction();

    // Calculate the hand's pitch, roll, and yaw angles
    /* std::cout << std::string(2, ' ') <<  "pitch: " << direction.pitch() * RAD_TO_DEG << " degrees, "
              << "roll: " << normal.roll() * RAD_TO_DEG << " degrees, "
              << "yaw: " << direction.yaw() * RAD_TO_DEG << " degrees" << std::endl; */

    // Get the Arm bone
    Arm arm = hand.arm();
    /* std::cout << std::string(2, ' ') <<  "Arm direction: " << arm.direction()
              << " wrist position: " << arm.wristPosition()
              << " elbow position: " << arm.elbowPosition() << std::endl; */

    // Get fingers
    const FingerList fingers = hand.fingers().extended();
    const FingerList fingers2 = hand2.fingers().extended();


    int extendedFingers = fingers.count();
    int extendedFingers2 = fingers2.count();

    // gestion de la gomme en fermant la main (LM)
    if(extendedFingers == 0 && nbHands == 1) {
      _lmGestures[LM_ERASER] = 1;
    } else {
      _lmGestures[LM_ERASER] = 0;
    }

    if(extendedFingers != 1 || nbHands != 1) {
      _lmGestures[LM_PEN] = 0;
    }

    for (FingerList::const_iterator fl = fingers.begin(); fl != fingers.end(); ++fl) {
      const Finger finger = *fl;

      // gestion d'options (LM)
      if(!hand.isLeft() && hand.palmNormal()[2] > 0)
        _lmGestures[LM_OPTIONS] = 1;

      // std::cout << nbHands << std::endl;
      for(int i = 0; i < extendedFingers2; i++) {
        // gestion de sélection-options (LM)
        if(_lmGestures[LM_OPTIONS] &&
           hand2.isLeft() &&
           extendedFingers2 == 1 &&
           (int)fingers2[i].type() == LM_INDEX &&
           hand2.palmVelocity()[2] > 100)
          _lmGestures[LM_TAP] = 1;
      }

      // gestion de l'index levé (LM)
      if((int)finger.type() == LM_INDEX && extendedFingers == 1 && nbHands == 1) {
        Bone::Type boneType = static_cast<Bone::Type>(LM_FDISTAL);
        Bone bone = finger.bone(boneType);
        _hand[0].indexPosition = bone.nextJoint();
        _lmGestures[LM_PEN] = 1;
      } else {
        _lmGestures[LM_PEN] = 0;
      }

      // gestion deux mains vers le haut (LM)
      float thresholdY = 100.0f;
      float diffY = fabs(hand.palmPosition()[1] - hand2.palmPosition()[1]);
      float hPalmNormal = hand.palmNormal()[1], h2PalmNormal = hand2.palmNormal()[1];
      float hPalmVelocity = hand.palmVelocity()[1], h2PalmVelocity = hand2.palmVelocity()[1];

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
      } else {
        _lmGestures[LM_SWIP_UP] = _lmGestures[LM_SWIP_DOWN] = 0;
      }

      /* std::cout << std::string(4, ' ') <<  fingerNames[finger.type()]
                << " finger, id: " << finger.id()
                << ", length: " << finger.length()
                << "mm, width: " << finger.width() << std::endl; */

      // Get finger bones
      for (int b = 0; b < 4; ++b) {
        Bone::Type boneType = static_cast<Bone::Type>(b);
        Bone bone = finger.bone(boneType);
        /* std::cout << std::string(6, ' ') <<  boneNames[boneType]
                  << " bone, start: " << bone.prevJoint()
                  << ", end: " << bone.nextJoint()
                  << ", direction: " << bone.direction() << std::endl; */
      }
    }
  }
}

static void quit(void) {
  gl4duClean(GL4DU_ALL);
}

static void resize(int w, int h) {
  _wW  = w; _wH = h;
  glViewport(0, 0, _wW, _wH);
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-0.5, 0.5, -0.5 * _wH / _wW, 0.5 * _wH / _wW, 1.0, 1000.0);
  gl4duBindMatrix("modelViewMatrix");
}

static void draw(void) {
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  if(_lmGestures[LM_ERASER])
    std::cout << "ERASER ON" << std::endl;

  if(_lmGestures[LM_TAP])
    std::cout << "TAP ON" << std::endl;

  if(_lmGestures[LM_OPTIONS])
    std::cout << "OPTIONS ON" << std::endl;

  if(_lmGestures[LM_PEN])
    std::cout << "INDEX ON" << std::endl;

  if(_lmGestures[LM_SWIP_UP])
    std::cout << "SWIP-UP ON " << std::endl;

  if(_lmGestures[LM_SWIP_DOWN])
    std::cout << "SWIP-DOWN ON " << std::endl;

  GLfloat red[] = {1, 0, 0, 1},
          white[] = {1, 1, 1, 1};
          // yellow[] = {1, 1, 0, 1},
          // cyan[] = {0, 1, 1, 1}; 


  gl4duBindMatrix("modelViewMatrix");
  gl4duLoadIdentityf();
  glUseProgram(_pId);
  gl4duTranslatef(
    _hand[0].indexPosition[0] * 0.02,
    _hand[0].indexPosition[1] * 0.02 - 3.0f,
    -10.0);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "color"), 1, red);
  gl4dgDraw(_sphere);
  gl4duPopMatrix();

  if(!_lmGestures[LM_OPTIONS])
    return;

  std::cout << "OPTIONS ON" << std::endl;
  gl4duTranslatef(
    0,
    0,
    -10.0);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "color"), 1, white);
  gl4dgDraw(_cube);
  gl4duPopMatrix();

  gl4duTranslatef(
    -1.0f,
    0,
    -10.0);
  gl4duSendMatrices();
  glUniform4fv(glGetUniformLocation(_pId, "color"), 1, white);
  gl4dgDraw(_cube);
  gl4duPopMatrix();

}


void init(void) {
  glClearColor(0.1f, 0.1f, 0.1f, 0.0f);
  _pId  = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
  gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  resize(_wW, _wH);
  _cylinder = gl4dgGenCylinderf(30, GL_TRUE);
  _sphere = gl4dgGenSpheref(30, 30);
  _cube = gl4dgGenCubef();
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
  gl4duwMainLoop();

  controller.removeListener(listener);
  return 0;
}