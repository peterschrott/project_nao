#include "ThreadTest.h"

using namespace AL;


ThreadTest::ThreadTest(boost::shared_ptr<ALBroker> broker, const std::string& name):
  ALModule(broker, name) {
	setModuleDescription("creates two threads");
}

void ThreadTest::init() {
	/** Init is called just after construction.   */
	createThreads();
}

void ThreadTest::createThreads() {
	//pthread_t thread1;
	//pthread_t thread2;
	// ein Initialisieren bedeutet zugleich das Erzeugen eines Threads:
	//pthread_create( &thread1, NULL, thread_function1, NULL );
	//pthread_create( &thread2, NULL, thread_function2, NULL );
	boost::thread t1(thread_function1);
	boost::thread t2(thread_function2);
	t1.join();
	t2.join();
	
	return;
}

void ThreadTest::thread_function1(void) { // Der Type ist wichtig: void* als Parameter und Rückgabe
	std::string robotIp = "127.0.0.1";

	ALMotionProxy motion(robotIp);
	ALRobotPostureProxy robotPosture(robotIp);

	robotPosture.goToPosture("Sit", 0.8f);
	robotPosture.goToPosture("StandInit", 0.8f);
	
	int i = 0;
	while(i < 2) {
		// Example showing the moveTo command
		// as length of path is less than 0.4m
		// the path will be an SE3 interpolation
		// The units for this command are meters and radians
		float x  = 0.2f;
		float y  = 0.0f;
		// pi/2 anti-clockwise (90 degrees)
		float theta = 0.0f;
		motion.moveTo(x, y, theta);
		
		x  = -0.2f;
		y  = -0.0f;
		motion.moveTo(x, y, theta);
		
		// count the number of movements
		i++;
	}
	robotPosture.goToPosture("LyingBelly", 0.8f);
	return; 
} 

void ThreadTest::thread_function2(void) { // Der Type ist wichtig: void* als Parameter und RÃ¼ckgabe
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
	std::cerr << "   start recording" << std::endl;
	alvrp.startRecording("/home/nao/recordings/cameras", vidName);
	sleep(40);
	std::cerr << "   stop recording"  << std::endl;
	alvrp.stopRecording();
	// Video file is saved on the robot in the
	// /home/nao/recordings/cameras/ folder.
	return; 
} 

ThreadTest::~ThreadTest(){}
