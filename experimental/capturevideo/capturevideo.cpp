/**
*
* This example demonstrates how to get images from the robot remotely and how
* to display them on your screen using opencv.
*
* Copyright Aldebaran Robotics
*/

// Aldebaran includes.
#include <alcommon/alproxy.h>
#include <alvision/alimage.h>
#include <alproxies/alvideorecorderproxy.h>
#include <alvision/alvisiondefinitions.h>
#include <alproxies/alvideodeviceproxy.h>
#include <alerror/alerror.h>

// Opencv includes.
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>


#include <iostream>
#include <string>

using namespace AL;


/**
* \brief Shows images retrieved from the robot.
*
* \param robotIp the IP adress of the robot
*/
void recordMJPG() {
	/** Create a proxy to ALVideoDevice on the robot.*/
	ALVideoRecorderProxy alvrp("192.168.0.111", 9559);
	
	std::cerr << "VideoRecorderProxy started..." << std::endl;
	
	// This records a 320*240 MJPG video at 10 fps.
	// Note MJPG can't be recorded with a framerate lower than 3 fps.
	alvrp.setResolution(2);   // 2 = kVGA
	alvrp.setFrameRate(10);
	alvrp.setVideoFormat("MJPG");   // "MJPG" (compressed avi)
	alvrp.startRecording("/home/nao/recordings/cameras", "capture_01");
	std::cerr << "   started recoding" << std::endl;
	sleep(15);
	std::cerr << "   stopped recoding"  << std::endl;
	
	// Video file is saved on the robot in the
	// /home/nao/recordings/cameras/ folder.
	alvrp.stopRecording();
}

int main(int argc, char* argv[]) {
	try {
		recordMJPG();
	} catch (const AL::ALError& e) {
		std::cerr << "Caught exception " << e.what() << std::endl;
	}

	return 0;
}

