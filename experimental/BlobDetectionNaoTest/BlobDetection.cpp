#include "BlobDetection.h"

#include <iostream>

// opencv includes
#include <opencv2/opencv.hpp>

// Aldebaran includes
#include <alvision/alvideo.h>
#include <alproxies/alvideodeviceproxy.h>
#include <alvision/alvisiondefinitions.h>
#include <alvision/alimage.h>

// Include cvBlob
#include "cvblob/BlobResult.h"

using namespace cv;
using namespace std;
using namespace AL;

BlobDetection::BlobDetection(boost::shared_ptr<ALBroker> broker, const std::string& name):
		ALModule(broker, name) {
	setModuleDescription("the module detects blobs and nao says the amout of fingers shown.");

	//functionName("Leds");
	//BIND_METHOD(ThreadTest::createThreads);
}

void BlobDetection::init() {
	/** Init is called just after construction.   */
            videoFile.write((char*) ipl_imageSkinPixels->imageData, ipl_imageSkinPixels->imageSize);


			CBlobResult blobs;
			blobs.ClearBlobs();
			blobs = CBlobResult(ipl_imageSkinPixels, NULL, 0);	// Use a black background color.


			if(blobs.GetNumBlobs() == 1){
			
			}
			else{

			}
}
/*
Point BlobDetection::getCenterPoint(Rect rect) {
	return Point(rect.x + rect.width, rect.y + rect.height);
}

handOrientation BlobDetection::getOrientationOfRect(Rect rect) {
	if(rect.width > rect.height) {
		return LANDSCAPE;
	}
	if(rect.height > rect.width) {
		return PORTRAIT;
	}
	return NONE;
}

handside BlobDetection::getHandside(Rect head, Rect hand) {
	//Camera Picture is inversed
	if(getCenterPoint(head).x > getCenterPoint(hand).x){
		return RIGHT;
	}else{
		return LEFT;
	}
}

handOrientationChange BlobDetection::detectHandStateChange(handOrientation last, handOrientation current) {
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

int BlobDetection::handleGestures(gestures doGesture) {
	switch(doGesture){
		case LEFT_FLIP_DOWN:
			cout<<"LEFT FLIP DOWN"<<endl;
			break;
		case LEFT_FLIP_UP:
			cout<<"LEFT FLIP UP"<<endl;
			break;
		case RIGHT_FLIP_DOWN:
			cout<<"RIGHT FLIP DOWN"<<endl;
			break;
		case RIGHT_FLIP_UP:
			cout<<"RIGHT FLIP UP"<<endl;
			break;
		default:
			break;
	}
}
*/
BlobDetection::~BlobDetection(){}
