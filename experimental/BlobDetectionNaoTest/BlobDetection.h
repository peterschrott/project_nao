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

typedef enum {NONE = 0,PORTRAIT = 1,LANDSCAPE = 2} handOrientation;
typedef enum {NOCHANGE = 0,PORTRAIT_TO_LANDSCAPE = 1,LANDSCAPE_TO_PORTRAIT = 2} handOrientationChange;
typedef enum {LEFT = 0,RIGHT = 1} handside;
typedef enum {LEFT_FLIP_DOWN = 0,LEFT_FLIP_UP = 1,RIGHT_FLIP_DOWN = 2,RIGHT_FLIP_UP = 3} gestures;

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
	
		int handleGestures(gestures doGesture);
		handOrientationChange detectHandStateChange(handOrientation last, handOrientation current);
		handside getHandside(cv::Rect head, cv::Rect hand);
		handOrientation getOrientationOfRect(cv::Rect rect);
		cv::Point getCenterPoint(cv::Rect rect);
};

#endif