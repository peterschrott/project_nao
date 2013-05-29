/**
* @author Gwennael Gate
* Copyright (c) Aldebaran Robotics 2010
*/

#include <signal.h>
#include <boost/shared_ptr.hpp>
#include <alcommon/albroker.h>
#include <alcommon/almodule.h>
#include <alcommon/albrokermanager.h>
#include <alcommon/altoolsmain.h>

#include "RecordVid.h"
#include "Walk.h"


#ifdef TWOMODULES_IS_REMOTE
# define ALCALL
#else
# ifdef _WIN32
#  define ALCALL __declspec(dllexport)
# else
#  define ALCALL
# endif
#endif

extern "C" {
	
	boost::shared_ptr<AL::ALBroker> pBrkr;
	
	void* thread_RecordVid(void*) { 
		std::cerr << "   start recoding thread" << std::endl;
		AL::ALModule::createModule<RecordVid>(pBrkr, "RecordVid");
		return 0;
	}

	void* thread_Walk(void*) { 
		std::cerr << "   start walking thread" << std::endl;
		AL::ALModule::createModule<Walk>(pBrkr, "Walk");
		return 0;
	}
	
	ALCALL int _createModule(boost::shared_ptr<AL::ALBroker> pBroker) {
		// init broker with the main broker instance
		// from the parent executable
		AL::ALBrokerManager::setInstance(pBroker->fBrokerManager.lock());
		AL::ALBrokerManager::getInstance()->addBroker(pBroker);
		
		pBrkr = pBroker;
		
		pthread_t thread1;
		pthread_t thread2;
		// ein Initialisieren bedeutet zugleich das Erzeugen eines Threads:
		pthread_create( &thread1, NULL, thread_RecordVid, NULL );
		pthread_create( &thread2, NULL, thread_Walk, NULL );
		
		return 0;
	}
	
	ALCALL int _closeModule() {
		return 0;
	}
}

#ifdef TWOMODULES_IS_REMOTE
int main(int argc, char *argv[]) {
	// pointer to createModule
	TMainType sig;
	sig = &_createModule;

	// call main
	ALTools::mainFunction("RecordVid",argc, argv,sig);
}
#endif

