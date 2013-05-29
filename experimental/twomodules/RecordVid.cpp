#include "RecordVid.h"

using namespace AL;


RecordVid::RecordVid(boost::shared_ptr<ALBroker> broker, const std::string& name):
  ALModule(broker, name) {
	setModuleDescription("records a 15 second mjpg video and stores it on the robots hdd.");
}

void RecordVid::init() {
	/** Init is called just after construction.   */
	recordVideo();
}

void RecordVid::recordVideo() {
	std::string robotIp = "127.0.0.1";
	
	ALVideoRecorderProxy alvrp(robotIp);
	
	std::cerr << "VideoRecorderProxy started..." << std::endl;
	
	std::string vidName = "capture_02";
	
	// This records a 320*240 MJPG video at 10 fps.
	// Note MJPG can't be recorded with a framerate lower than 3 fps.
	alvrp.setResolution(2);   // 2 = kVGA
	alvrp.setFrameRate(10);
	alvrp.setVideoFormat("MJPG");   // "MJPG" (compressed avi)
	
	sleep(5);
	// start the recoreding
	std::cerr << "   start recoding" << std::endl;
	alvrp.startRecording("/home/nao/recordings/cameras", vidName);
	sleep(15);
	std::cerr << "   stop recoding"  << std::endl;
	alvrp.stopRecording();
	// Video file is saved on the robot in the
	// /home/nao/recordings/cameras/ folder.
	return; 
}

RecordVid::~RecordVid(){}
