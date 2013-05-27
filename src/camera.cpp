#include "camera.hpp"

// Aldebaran includes.
#include <alproxies/alvideodeviceproxy.h>
#include <alvision/alimage.h>
#include <alvision/alvisiondefinitions.h>

// Opencv includes.
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

using namespace cv;
using namespace AL;
/**
* TODO comment
*/
Mat getOpenCvImage() {
    // todo get image from camara or from video

	return Mat();
}

/** static set up of the ALVideoDeviceProxy */
ALVideoDeviceProxy *camProxy = 0;
std::string clientName = "";

ALVideoDeviceProxy *getALVideoDeviceProxy() {
	if(camProxy == 0) {
		/** Create a proxy to ALVideoDevice on the robot.*/
		camProxy = new ALVideoDeviceProxy("nao.local", 9559);
		/** Subscribe a client image requiring 640*480px and RGB colorspace.*/
		clientName = camProxy->subscribeCamera("camera_01", 1, AL::kVGA, AL::kRGBColorSpace, 10);
	}
	return camProxy;
}

/**
* TODO comment
*/
bool unsubscribe() {
	if(clientName != "") {
		/** Cleanup.*/
		camProxy->unsubscribe(clientName); 
		return true;
	}
	return false;
}

/**
* TODO comment
*/
Mat getNaoImage() {

	ALVideoDeviceProxy *cam =  getALVideoDeviceProxy();

	/** Retrieve an image from the camera.
		* The image is returned in the form of a container object, with the
		* following fields:
		* 0 = width
		* 1 = height
		* 2 = number of layers
		* 3 = colors space index (see alvisiondefinitions.h)
		* 4 = time stamp (seconds)
		* 5 = time stamp (micro seconds)
		* 6 = image buffer (size of width * height * number of layers)
	*/
	ALValue rgbImg = cam->getImageLocal(clientName);

	/** Access the image buffer (6th field) and assign it to the opencv image container. */
	/** Create an Mat header to wrap into an opencv image.*/
	Mat naoImg = Mat(Size(rgbImg[0], rgbImg[1]), CV_8UC3);
	naoImg.data = (uchar*) rgbImg[6].GetBinary();

	/** Convert image from RGB to HSV */
	cvtColor(naoImg, naoImg, CV_RGB2HSV);

	return naoImg;
}