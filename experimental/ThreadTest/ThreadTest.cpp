#include "ThreadTest.h"

using namespace AL;


ThreadTest::ThreadTest(boost::shared_ptr<ALBroker> broker, const std::string& name):
  ALModule(broker, name) {
	setModuleDescription("creates two threads");

	functionName("createThreads", "ThreadTest", "test");
	BIND_METHOD(ThreadTest::createThreads);
}

void ThreadTest::init() {
	/** Init is called just after construction.   */
	createThreads();
}

void ThreadTest::createThreads() {
	pthread_t thread1;
	pthread_t thread2;
	// ein Initialisieren bedeutet zugleich das Erzeugen eines Threads:
	pthread_create( &thread1, NULL, thread_function1, NULL );
	pthread_create( &thread2, NULL, thread_function2, NULL );
}

void * ThreadTest::thread_function1(void*) { // Der Type ist wichtig: void* als Parameter und Rückgabe
	std::string robotIp = "127.0.0.1";

	ALMotionProxy motion(robotIp);
	ALRobotPostureProxy robotPosture(robotIp);

	robotPosture.goToPosture("StandInit", 0.5f);

	int i = 0;
	while(i < 10) {
		// Example showing the moveTo command
		// as length of path is less than 0.4m
		// the path will be an SE3 interpolation
		// The units for this command are meters and radians
		float x  = 0.2f;
		float y  = 0.2f;
		// pi/2 anti-clockwise (90 degrees)
		float theta = 0.0f;
		motion.moveTo(x, y, theta);
		
		x  = -0.2f;
		y  = -0.2f;
		motion.moveTo(x, y, theta);
		
		// count the number of movements
		i++;
	}
	return 0; 
} 

void * ThreadTest::thread_function2(void*) { // Der Type ist wichtig: void* als Parameter und RÃ¼ckgabe
	int i = 0;
	for(;i < 12;i++) {
		std::cerr << "Thread 2" << std::endl;
		sleep(2);
	}
 
  return 0; 
} 

ThreadTest::~ThreadTest(){}
