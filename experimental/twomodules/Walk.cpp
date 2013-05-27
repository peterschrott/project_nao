#include "Walk.h"

using namespace AL;


Walk::Walk(boost::shared_ptr<ALBroker> broker, const std::string& name):
		ALModule(broker, name) {
	setModuleDescription("walks 20 cm up and the same distance down again, twice.");
}

void Walk::init() {
	/** Init is called just after construction.   */
	walkUpAndDown();
}

void Walk::walkUpAndDown() {
	std::string robotIp = "127.0.0.1";

	ALMotionProxy motion(robotIp);
	ALRobotPostureProxy robotPosture(robotIp);
	
	robotPosture.goToPosture("Sit",0.8f);
	
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
	
	robotPosture.goToPosture("SitRelax", 0.8f);
	
	return;
}

Walk::~Walk(){}
