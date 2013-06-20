/**
* @author Peter Schrott, Alex Goetz
*/

#ifndef NAOGESTURERECOGNITION_H
#define NAOGESTURERECOGNITION_H

// c / c++ includes
#include <string>
#include <boost/shared_ptr.hpp>
#include <stdio.h>

// aldebaran includes
#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
#include <alerror/alerror.h>

// opencv includes
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>

typedef enum {NONE = 0,PORTRAIT = 1,LANDSCAPE = 2} HandOrientation;
typedef enum {NOCHANGE = 0,PORTRAIT_TO_LANDSCAPE = 1,LANDSCAPE_TO_PORTRAIT = 2} HandOrientationChange;
typedef enum {LEFT = 0,RIGHT = 1} Handside;
typedef enum {LEFT_FLIP_DOWN = 0,LEFT_FLIP_UP = 1,RIGHT_FLIP_DOWN = 2,RIGHT_FLIP_UP = 3} Gesture;
typedef enum {UP = 1,DOWN = 0} HandPosition;
typedef enum {LEFT_UP_RIGHT_DOWN = 2,LEFT_DOWN_RIGHT_UP = 1, BOTH_UP = 3, BOTH_DOWN = 0} HandStatus;

class NaoGestureRecognition : public AL::ALModule
{
public:
    /**
    * Default Constructor for modules.
    * @param broker the broker to which the module should register.
    * @param name the boadcasted name of the module.
    */
    NaoGestureRecognition(boost::shared_ptr<AL::ALBroker> broker, const std::string& name);

    // Destructor.
    virtual ~NaoGestureRecognition();

    virtual void init();
private:
    void onFrontTactilTouched();
    void onMiddleTactilTouched();
    //int isMoving(Hand h);
    int handleGestures(Gesture doGesture);
    int updateStatus(Gesture gesture);
    HandOrientationChange detectHandStateChange(HandOrientation last, HandOrientation current);
    Handside getHandside(cv::Rect head, cv::Rect hand);
    HandOrientation getOrientationOfRect(cv::Rect rect);
    cv::Point getCenterPoint(cv::Rect rect);
    void initStatusMask();
    void initLeds();
    void trackHead(int x, int y);
};

#endif
