#include "BlobDetection.h"

// c / c++ includes
#include <iostream>
#include <math.h>

// Aldebaran includes
#include <alvision/alvideo.h>
#include <alproxies/alvideodeviceproxy.h>
#include <alvision/alvisiondefinitions.h>
#include <alvision/alimage.h>
#include <alproxies/albehaviormanagerproxy.h>
#include <alproxies/almemoryproxy.h>
#include <alproxies/alledsproxy.h>

// utils includes
#include "../math/angles.h"

using namespace cv;
using namespace std;
using namespace AL;

// constants for the led groups
const string FACE_LED_BLUE = "face_leds_blue";
const string FACE_LED_GREEN = "face_leds_green";
const string FACE_LED_RED = "face_leds_red";
// constens for the behaviour
const std::string ARM_RIGHT_UP = "liftuprightarm_1";
const std::string ARM_LEFT_UP = "liftupleftarm_1";
const std::string ARM_BOTH_UP = "liftuparmboth_1";
const std::string STAND = "StandUp";
// constants to movement detection
const int minMoveDistance = 4;
const int maxMoveDistacne = 50;
// constent for the minimal blob area
const int minBlobArea = 200;

// global viariables
int touched = 0;
int headMoved = 0;
Hand handRight;
// nao status
int handStatus = BOTH_DOWN;

// global variables for the proxies
ALBehaviorManagerProxy behaviourProxy;
ALLedsProxy ledProxy;
ALVideoDeviceProxy camProxy;
ALMemoryProxy memProxy;

BlobDetection::BlobDetection(boost::shared_ptr<ALBroker> broker, const std::string& name):
		ALModule(broker, name) {

	setModuleDescription("The module recognizes a head and two hands as three blobs and figures the gestures out.");

    // bin the callback functions for the tactil sensors
	functionName("onFrontTactilTouched", getName(), "Method wich is called after front tactil is touched.");
	BIND_METHOD(BlobDetection::onFrontTactilTouched);
    functionName("onMiddleTactilTouched", getName(), "Method wich is called after middle tactil is touched.");
	BIND_METHOD(BlobDetection::onMiddleTactilTouched);
}

void BlobDetection::init() {
	/** Init is called just after construction.   */
	try {
		// create all proxies on the robot.
        initProxies();

        // initialize the led handling
        initLeds();

		// Subscribe a client image requiring 640*480px and RGB colorspace.
		cameraId = camProxy.subscribeCamera("camera_01", 0, AL::kVGA, AL::kRGBColorSpace , 10);

		memProxy.subscribeToEvent("FrontTactilTouched", "BlobDetection","onFrontTactilTouched");
		memProxy.subscribeToEvent("MiddleTactilTouched", "BlobDetection","onMiddleTactilTouched");

        // init gesture recogntion
        HandOrientation rightOrientationLast = NONE;
        HandOrientation leftOrientationLast = NONE;
        HandOrientation rightOrientationCur = NONE;
        HandOrientation leftOrientationCur = NONE;

		// stand up
		behaviourProxy.runBehavior(STAND);

        while(1) {
            if(touched) {
                //Switch LEDs RED OFF, BLUE ON
                if(red_on == 1) {
                    ledProxy.off(FACE_LED_RED);
                    red_on = 0;
                }
                if(blue_on == 0) {
                    ledProxy.on(FACE_LED_BLUE);
                    blue_on = 1;
                }

                // fetch a image from the camera
                Mat img_nao = getCameraImage();

                // process the camera image in prepare it for blob detection
                Mat img_skinPixels = processImage(img_nao);

                // Assing the Mat (C++) to an IplImage (C), this is necessary because the blob detection is writtn in old opnCv C version
                IplImage ipl_imageSkinPixels = img_skinPixels;

                // find the blobs in the convertet picture
                CBlobResult blobs = findBlobs(&ipl_imageSkinPixels);

                // ##### Gestures #####
                std::cout << "Number of Blobs: " << blobs.GetNumBlobs() <<endl;
                if(blobs.GetNumBlobs() == 0){
                    //picture empty
                }else if(blobs.GetNumBlobs() == 1){
                    //head detected
                }else if(blobs.GetNumBlobs() == 2 || blobs.GetNumBlobs() == 3) {
                    //head + one hand || head + two hands
                    Rect rect[3];
                    int indexHead = -1, indexHandLeft = -1, indexHandRight = -1;

                    //Get Bounding Boxes
                    for(int i = 0; i< blobs.GetNumBlobs(); i++){
                        rect[i] = blobs.GetBlob(i)->GetBoundingBox();
                    }

                    //Detect Head and Hand indexes
                    if(blobs.GetNumBlobs() == 2){
                        // head and one hand
                        int indexHand = -1;
                        if(getCenterPoint(rect[0]).y < getCenterPoint(rect[1]).y){
                            // rect[0] is head
                            indexHead = 0;
                            indexHand = 1;
                        }else{
                            // rect[1] is head
                            indexHead = 1;
                            indexHand = 0;
                        }
                        if(getHandside(rect[indexHead], rect[indexHand]) == LEFT){
                            // hand is left
                            indexHandLeft = 1;
                            indexHandRight = -1;
                        }else{
                            // hand ist right
                            indexHandLeft = -1;
                            indexHandRight = 1;
                        }
                    } else {
                        //two hands
                        int indexHand1 = -1;
                        int indexHand2 = -1;
                        if(getCenterPoint(rect[0]).y < getCenterPoint(rect[1]).y && getCenterPoint(rect[0]).y < getCenterPoint(rect[2]).y) {
                            // rect[0] is head
                            indexHead = 0;
                            indexHand1 = 1;
                            indexHand2 = 2;
                        }else if(getCenterPoint(rect[1]).y < getCenterPoint(rect[0]).y && getCenterPoint(rect[1]).y < getCenterPoint(rect[2]).y) {
                            // rect[1] is head
                            indexHead = 1;
                            indexHand1 = 0;
                            indexHand2 = 2;
                        }else{
                            // rect[2] is head
                            indexHead = 2;
                            indexHand1 = 0;
                            indexHand2 = 1;
                        }

                        if(getHandside(rect[indexHead], rect[indexHand1]) == LEFT){
                            indexHandLeft = indexHand1;
                            indexHandRight = indexHand2;
                        }else{
                            indexHandLeft = indexHand2;
                            indexHandRight = indexHand1;
                        }
                    }

                    // bobs are detected.
                    // adjuste naos head to detected head-bolb
                    trackHead(getCenterPoint(rect[indexHead]).x, getCenterPoint(rect[indexHead]).y);

                    /*// follow the right hand
                    if(indexHandRight > 0) {
                        //std::cout << "right hand deteced" <<endl;
                        if(isMoving(handRight)) {
                            std::cout << "hand moving" <<endl;
                            handRight.centerPrev = handRight.centerCurr;
                            handRight.centerCurr = getCenterPoint(rect[indexHandRight]);
                        } else {
                            std::cout << "hand not moving" <<endl;
                            if(handRight.centerInit.y != 0 && abs(handRight.centerInit.y - handRight.centerCurr.y) > 20) {
                                if(handRight.centerInit.y < handRight.centerCurr.y) {
                                    // hand moved down
                                    std::cout << "                           hand moved down" <<endl;
                                } else {
                                    // hand moved up
                                    std::cout << "                           hand moved up" <<endl;
                                }
                            }
                            handRight.centerInit = getCenterPoint(rect[indexHandRight]);
                            handRight.centerPrev = handRight.centerCurr;
                            handRight.centerCurr = getCenterPoint(rect[indexHandRight]);
                        }
                    }*/

                    //Get Orientations from Hand rects
                    leftOrientationCur = (indexHandLeft != -1)?getOrientationOfRect(rect[indexHandLeft]):NONE;
                    rightOrientationCur = (indexHandRight != -1)?getOrientationOfRect(rect[indexHandRight]):NONE;

                    if(!headMoved) {
                        // NAOs head was moved. so wait for another loop to get a new picture
                        headMoved = 0;
                        //Check Change of Left hand
                        switch(detectHandStateChange(leftOrientationLast, leftOrientationCur)){
                            case PORTRAIT_TO_LANDSCAPE:
                                handleGestures(LEFT_FLIP_DOWN);
                                break;
                            case LANDSCAPE_TO_PORTRAIT:
                                handleGestures(LEFT_FLIP_UP);
                                break;
                            case NOCHANGE:
                                // TODO
                            default:
                                break;
                        }

                        //Check Change of Right hand
                        switch(detectHandStateChange(rightOrientationLast, rightOrientationCur)){
                            case PORTRAIT_TO_LANDSCAPE:
                                handleGestures(RIGHT_FLIP_DOWN);
                                break;
                            case LANDSCAPE_TO_PORTRAIT:
                                handleGestures(RIGHT_FLIP_UP);
                                break;
                            case NOCHANGE:
                                //TODO
                            default:
                                break;
                        }
                    }

                }else if(blobs.GetNumBlobs() > 3){
                    //too much information
                    cout<<"too much information"<<endl;
                }

                leftOrientationLast = leftOrientationCur;
                rightOrientationLast = rightOrientationCur;

                // Free all the resources.
                //IplImage* p_iplImage = &ipl_imageSkinPixels;
                //cvReleaseImage(&p_iplImage);

            } else {
                if(red_on == 0) {
                    ledProxy.on(FACE_LED_RED);
                    red_on = 1;

                    // stand up
                    behaviourProxy.runBehavior(STAND);
                }
                if(blue_on == 1) {
                    ledProxy.off(FACE_LED_BLUE);
                    blue_on = 0;
                }
            }
        }
	} catch (const AL::ALError& e) {
		std::cerr << "Caught exception: " << e.what() << std::endl;
		return;
	}
	return;
}

/**
* Function to set up all neccessary Proxies.
*/
void BlobDetection::initProxies() {
    // get the parent broker for proxy initialisation
    boost::shared_ptr<ALBroker> parentBroker = getParentBroker();

    // initialze all neccessary proxies
    camProxy = ALVideoDeviceProxy(parentBroker);
    behaviourProxy = ALBehaviorManagerProxy(parentBroker);
    ledProxy = ALLedsProxy(parentBroker);
    memProxy = ALMemoryProxy(parentBroker);
}

/**
* Initialze the led groups.
*/
void BlobDetection::initLeds() {
    // Create a vector to access all RED leds
    names_red.push_back("Face/Led/Red/Left/0Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Left/45Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Left/90Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Left/135Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Left/180Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Left/225Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Left/270Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Left/315Deg/Actuator/Value");

    names_red.push_back("Face/Led/Red/Right/0Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Right/45Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Right/90Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Right/135Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Right/180Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Right/225Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Right/270Deg/Actuator/Value");
    names_red.push_back("Face/Led/Red/Right/315Deg/Actuator/Value");

    // Create a vector to access all GREEN leds
    names_green.push_back("Face/Led/Green/Left/0Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Left/45Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Left/90Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Left/135Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Left/180Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Left/225Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Left/270Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Left/315Deg/Actuator/Value");

    names_green.push_back("Face/Led/Green/Right/0Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Right/45Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Right/90Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Right/135Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Right/180Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Right/225Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Right/270Deg/Actuator/Value");
    names_green.push_back("Face/Led/Green/Right/315Deg/Actuator/Value");

    //Create a vector to access all BLUE leds
    names_blue.push_back("Face/Led/Blue/Left/0Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Left/45Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Left/90Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Left/135Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Left/180Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Left/225Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Left/270Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Left/315Deg/Actuator/Value");

    names_blue.push_back("Face/Led/Blue/Right/0Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Right/45Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Right/90Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Right/135Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Right/180Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Right/225Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Right/270Deg/Actuator/Value");
    names_blue.push_back("Face/Led/Blue/Right/315Deg/Actuator/Value");

    // create the led groups
    ledProxy.createGroup(FACE_LED_GREEN, names_green);
    ledProxy.createGroup(FACE_LED_RED, names_red);
    ledProxy.createGroup(FACE_LED_BLUE, names_blue);

    // set the intensity of all led groups to maximum
    ledProxy.setIntensity(FACE_LED_BLUE, 1);
    ledProxy.setIntensity(FACE_LED_RED, 1);
    ledProxy.setIntensity(FACE_LED_GREEN, 1);

    // initiall shwich off all leds
    ledProxy.off(FACE_LED_RED);
    ledProxy.off(FACE_LED_BLUE);
    ledProxy.off(FACE_LED_GREEN);

    // set the led status
    red_on = 0;
    green_on = 0;
    blue_on = 0;
}

/**
* The callback function the front tractil.
* This function is called event-driven,
* if the front tractil on the naos head is touched.
*/
void BlobDetection::onFrontTactilTouched() {
    touched = 1;
}

/**
* The callback function the middle tractil.
* This function is called event-driven,
* if the middle tractil on the naos head is touched.
*/
void BlobDetection::onMiddleTactilTouched() {
    touched = 0;
}

/**
* Function gets a image from naos head camera and returns it as opencv Mat.
*/
Mat BlobDetection::getCameraImage() {
    //nao camera, we subscribed on. Its in RGB colorspace
    ALImage *img_cam = (ALImage*)camProxy.getImageLocal(cameraId);

    // Create an openCv Mat header to convert the aldebaran AlImage image.
    // To convert the aldebaran image only the data are of it are assigned to the openCv image.
    Mat img_nao = Mat(Size(img_cam->getWidth(), img_cam->getHeight()), CV_8UC3);
    uchar* img_data = (uchar*)malloc(img_cam->getSize());
    memcpy(img_data, (uchar*)img_cam->getData(), img_cam->getSize());
    img_nao.data = img_data;

    // release the camara imge
    camProxy.releaseImage(cameraId);


    return img_nao;
}

/**
*/
Mat BlobDetection::processImage(Mat img_input) {
    Mat img_hsv;
    // Convert the RGB image from the camera to an HSV image */
    cvtColor(img_input, img_hsv, CV_RGB2HSV);

    // Get the separate HSV color components of the color input image.
    std::vector<Mat> channels(3);
    split(img_hsv, channels);

    Mat planeH = channels[0];
    Mat planeS = channels[1];
    Mat planeV = channels[2];

    // Detect which pixels in each of the H, S and V channels are probably skin pixels.
    threshold(planeH, planeH, 150, UCHAR_MAX, CV_THRESH_BINARY_INV);//18
    threshold(planeS, planeS, 60, UCHAR_MAX, CV_THRESH_BINARY);//50
    threshold(planeV, planeV, 170, UCHAR_MAX, CV_THRESH_BINARY);//80

    // Combine all 3 thresholded color components, so that an output pixel will only
    // be white if the H, S and V pixels were also white.
    Mat img_skinPixels = Mat(img_hsv.size(), CV_8UC3);	        // Greyscale output image.
    bitwise_and(planeH, planeS, img_skinPixels);				// imageSkin = H {BITWISE_AND} S.
    bitwise_and(img_skinPixels, planeV, img_skinPixels);	    // imageSkin = H {BITWISE_AND} S {BITWISE_AND} V.

    return img_skinPixels;
}

/**
*
*/
CBlobResult findBlobs(IplImage* ipl_imageSkinPixels) {
    // Set up the blob detection.
    CBlobResult blobs;
    blobs.ClearBlobs();
    blobs = CBlobResult(ipl_imageSkinPixels, NULL, 0);	// Use a black background color.

    // Ignore the blobs whose area is less than minArea.
    blobs.Filter(blobs, B_EXCLUDE, CBlobGetArea(), B_LESS, minBlobArea);

    return blobs;
}

/**
* TODO
*/
Point BlobDetection::getCenterPoint(Rect rect) {
	return Point(rect.x + rect.width, rect.y + rect.height);
}

/**
* TODO
*/
HandOrientation BlobDetection::getOrientationOfRect(Rect rect) {
	if(rect.width > rect.height) {
		return LANDSCAPE;
	}
	if(rect.height > rect.width) {
		return PORTRAIT;
	}
	return NONE;
}

/**
* TODO
*/
Handside BlobDetection::getHandside(Rect head, Rect hand) {
	//Camera Picture is inversed
	if(getCenterPoint(head).x > getCenterPoint(hand).x){
		return RIGHT;
	}else{
		return LEFT;
	}
}

/**
* TODO
*/
int BlobDetection::isMoving(Hand h) {

    int movement = sqrt(pow(h.centerCurr.x - h.centerPrev.x, 2) + pow(h.centerCurr.y - h.centerPrev.y , 2)); // pythagoras

    if(movement > minMoveDistance && movement < maxMoveDistacne) {
        // the point is within the movement tolerance
        // moves more then minMoveDestinace and less then MaxMoveDistance
        return 0;
    } else {
        return 1;
    }
}

/**
* TODO
*/
HandOrientationChange BlobDetection::detectHandStateChange(HandOrientation last, HandOrientation current) {
	if(last == NONE) {
	    return NOCHANGE; //TODO: ???
	}
	if(current == NONE) {
	    return NOCHANGE; //TODO: ???
	}

	if(last == PORTRAIT && current == LANDSCAPE) {
	    return PORTRAIT_TO_LANDSCAPE;
	}
	if(last == LANDSCAPE && current == PORTRAIT) {
	    return LANDSCAPE_TO_PORTRAIT;
	}

	return NOCHANGE;

}

/**
* TODO
*/
int BlobDetection::handleGestures(Gestures doGesture) {
	switch(doGesture){
		case LEFT_FLIP_DOWN:
			cout<<"LEFT FLIP DOWN"<<endl;
			updateStatus(LEFT_FLIP_DOWN);
			break;
		case LEFT_FLIP_UP:
			cout<<"LEFT FLIP UP"<<endl;
			updateStatus(LEFT_FLIP_UP);
			break;
		case RIGHT_FLIP_DOWN:
			cout<<"RIGHT FLIP DOWN"<<endl;
			updateStatus(RIGHT_FLIP_DOWN);
			break;
		case RIGHT_FLIP_UP:
			cout<<"RIGHT FLIP UP"<<endl;
			updateStatus(RIGHT_FLIP_UP);
			break;
		default:
			break;
	}
}

/**
* TODO
*/
int BlobDetection::updateStatus(Gestures gesture){
	//Set new Status
	switch(gesture){
		case LEFT_FLIP_UP:
			handStatus = handStatus | (UP<<1);
			break;
		case RIGHT_FLIP_UP:
			handStatus = handStatus | (UP);
			break;
		case LEFT_FLIP_DOWN:
			handStatus = handStatus & ((DOWN << 1)^1);
			break;
		case RIGHT_FLIP_DOWN:
			handStatus = handStatus & (DOWN ^ 2 );
			break;
	}

	//call Behaviors
	switch(handStatus){
		case LEFT_UP_RIGHT_DOWN:
			behaviourProxy.runBehavior(ARM_RIGHT_UP);
			break;
		case LEFT_DOWN_RIGHT_UP:
			behaviourProxy.runBehavior(ARM_LEFT_UP);
			break;
		case BOTH_UP:
			behaviourProxy.runBehavior(ARM_BOTH_UP);
			break;
		case BOTH_DOWN:
			behaviourProxy.runBehavior(STAND);
			break;
		default:
			break;
	}
}

/**
*/
void trackFace(int x, int y) {

    pixelToRad(x, 640, degreeToRad(60.9));

}

/**
* destructor
*/
BlobDetection::~BlobDetection() {
    // unsubscribe from naos camera
    camProxy.unsubscribe(cameraId);
}
