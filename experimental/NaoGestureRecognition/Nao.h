#ifndef Nao_H
#define Nao_H

// c/c++ includes
#include <boost/shared_ptr.hpp>

// aldebaran includes
#include <alproxies/alvideodeviceproxy.h>
#include <alproxies/albehaviormanagerproxy.h>
#include <alproxies/almemoryproxy.h>
#include <alproxies/alledsproxy.h>

// opencv includes
#include <opencv2/core/core.hpp>

using namespace cv;
using namespace std;
using namespace AL;

class Nao {

    public:
        Nao(boost::shared_ptr<AL::ALBroker>);
        virtual ~Nao();
    private:
        // ####### attributes #######
        boost::shared_ptr<AL::ALBroker> parentBroker;
        // variables for the proxies
        ALBehaviorManagerProxy behaviourProxy;
        ALLedsProxy ledProxy;
        ALVideoDeviceProxy camProxy;
        ALMemoryProxy memProxy;
        // subscriber id for nao cam
        string cameraId;
        // led status flags
        int red_on;
        int green_on;
        int blue_on;

        int touched;

        // led groups
        vector<string> names_blue;
        vector<string> names_red;
        vector<string> names_green;

        // ######## functions ########
        void initProxies();
        void initLeds();
        void onFrontTactilTouched();
        void onMiddleTactilTouched();
        Mat getCameraImage();
        void doBehaviour(string behaviour);
};

#endif
