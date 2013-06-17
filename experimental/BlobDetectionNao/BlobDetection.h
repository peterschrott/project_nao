/**
 * @author Peter Schrott, Alex Goetz
 */

#ifndef BlobDetection_H
#define BlobDetection_H

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

typedef enum {NONE = 0,PORTRAIT = 1,LANDSCAPE = 2} handOrientation;
typedef enum {NOCHANGE = 0,PORTRAIT_TO_LANDSCAPE = 1,LANDSCAPE_TO_PORTRAIT = 2} handOrientationChange;
typedef enum {LEFT = 0,RIGHT = 1} handside;
typedef enum {LEFT_FLIP_DOWN = 0,LEFT_FLIP_UP = 1,RIGHT_FLIP_DOWN = 2,RIGHT_FLIP_UP = 3} gestures;
typedef enum {UP = 1,DOWN = 0} handposition;
typedef enum {LEFT_UP_RIGHT_DOWN = 2,LEFT_DOWN_RIGHT_UP = 1, BOTH_UP = 3, BOTH_DOWN = 0} HandStatus;

typedef struct {
    cv::Point centerInit;
    cv::Point centerPrev;
    cv::Point centerCurr;
} Hand;

class BlobDetection : public AL::ALModule {
	public:
		/**
		* Default Constructor for modules.
		* @param broker the broker to which the module should register.
		* @param name the boadcasted name of the module.
		*/
		BlobDetection(boost::shared_ptr<AL::ALBroker> broker, const std::string& name);

		/// Destructor.
		virtual ~BlobDetection();

		virtual void init();
	private:
        void onFrontTactilTouched();
        void onMiddleTactilTouched();
        int isMoving(Hand h);
		int handleGestures(gestures doGesture);
		int updateStatus(gestures gesture);
		handOrientationChange detectHandStateChange(handOrientation last, handOrientation current);
		handside getHandside(cv::Rect head, cv::Rect hand);
		handOrientation getOrientationOfRect(cv::Rect rect);
		cv::Point getCenterPoint(cv::Rect rect);
};

#endif
