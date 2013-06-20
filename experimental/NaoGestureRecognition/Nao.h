#ifndef Nao_H
#define Nao_H

// c/c++ includes
#include <boost/shared_ptr.hpp>

// aldebaran includes
#include <alproxies/alvideodeviceproxy.h>
#include <alproxies/albehaviormanagerproxy.h>
#include <alproxies/almemoryproxy.h>
#include <alproxies/alledsproxy.h>
#include <alproxies/almotionproxy.h>

// opencv includes
#include <opencv2/core/core.hpp>

//
#include "states.h"

using namespace cv;
using namespace std;
using namespace AL;

//typedef enum {LEFT_UP_RIGHT_DOWN = 2,LEFT_DOWN_RIGHT_UP = 1, BOTH_UP = 3, BOTH_DOWN = 0} FsmStatus;
//typedef enum {LEFT_FLIP_DOWN = 0,LEFT_FLIP_UP = 1,RIGHT_FLIP_DOWN = 2,RIGHT_FLIP_UP = 3} Gesture;

class Nao {

    public:
        Nao();
        Nao(boost::shared_ptr<AL::ALBroker>);
        virtual ~Nao();

        Mat getCameraImage();
        void trackPointWithHead(int, int);

        void updateStatus(Gesture);

        void moveArmRightUp();
        void moveArmLeftUp();
        void moveArmBothUp();
        void moveArmBothDown();
        void standUp();

        void switchEyeLedsGreenOn();
        void switchEyeLedsGreenOff();
        void switchEyeLedsBlueOn();
        void switchEyeLedsBlueOff();
        void switchEyeLedsRedOn();
        void switchEyeLedsRedOff();

    private:
        // ####### attributes #######
        boost::shared_ptr<AL::ALBroker> parentBroker;
        // variables for the proxies
        ALBehaviorManagerProxy* behaviourProxy;
        ALLedsProxy* ledProxy;
        ALVideoDeviceProxy* camProxy;
        ALMemoryProxy* memProxy;
        ALMotionProxy* motionProxy;
        // subscriber id for nao cam
        string cameraId;
        // led status flags
        bool eyeLedsGreenOn;
        bool eyeLedsBlueOn;
        bool eyeLedsRedOn;

        // for naos fsm
        HandStatus fsmMask[4][4];
        HandStatus fsmStatus;

        // led groups
        vector<string> eyeLedsBlueGroup;
        vector<string> eyeLedsRedGroup;
        vector<string> eyeLedsGreenGroup;

        // ######## functions ########
        void initProxies();
        void initLeds();
        void initFsm();

        void doBehaviour(string behaviour);
};

#endif
