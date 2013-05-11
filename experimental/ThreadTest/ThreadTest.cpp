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
	int i = 0;
	for(;i < 20;i++) {
		std::cerr << "Thread 1" << std::endl;
		sleep(1);
	}
 
  return 0; 
} 

void * ThreadTest::thread_function2(void*) { // Der Type ist wichtig: void* als Parameter und Rückgabe
	int i = 0;
	for(;i < 20;i++) {
		std::cerr << "Thread 2" << std::endl;
		sleep(2);
	}
 
  return 0; 
} 

ThreadTest::~ThreadTest(){}
