/**
 * @author Peter Schrott
 */

#ifndef THREAD_TEST_H
#define THREAD_TEST_H

#include <iostream>
#include <string>
//#include <pthread.h>
#include <boost/shared_ptr.hpp>
#include <stdio.h>
#include <boost/thread.hpp>

#include <alvalue/alvalue.h>
#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
#include <alcommon/albrokermanager.h>

#include <alproxies/alvideorecorderproxy.h>
#include <alproxies/almotionproxy.h>
#include <alproxies/alrobotpostureproxy.h>

namespace AL
{
  class ALBroker;
}

class ThreadTest : public AL::ALModule
{
  public:
    /**
     * Default Constructor for modules.
     * @param broker the broker to which the module should register.
     * @param name the boadcasted name of the module.
     */
    ThreadTest(boost::shared_ptr<AL::ALBroker> broker, const std::string& name);

    /// Destructor.
    virtual ~ThreadTest();

    virtual void init();

    void createThreads();

private:

	static void thread_function1(void);
	static void thread_function2(void);

};

#endif