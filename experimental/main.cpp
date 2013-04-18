/**
 * Copyright (c) 2011 Aldebaran Robotics
 */

#ifndef HELLOWORLD_H
#define HELLOWORLD_H

#include <boost/shared_ptr.hpp>
#include <alcommon/almodule.h>

namespace AL
{
  // This is a forward declaration of AL:ALBroker which
  // avoids including <alcommon/albroker.h> in this header
  class ALBroker;
}

/**
 * A simple example module that says "Hello world" using
 * text to speech, or prints to the log if we can't find TTS
 *
 * This class inherits AL::ALModule. This allows it to bind methods
 * and be run as a remote executable or as a plugin within NAOqi
 */
class HelloWorld : public AL::ALModule
{
  public:
    HelloWorld(boost::shared_ptr<AL::ALBroker> pBroker, const std::string& pName);

    virtual ~HelloWorld();

    /** Overloading ALModule::init().
    * This is called right after the module has been loaded
    */
    virtual void init();

    /**
     * Make Nao say "Hello world" or just print a message to
     * the console if not connected to a robot.
     */
    void sayHello();

    /**
    * Make Nao say a sentence given in argument.
    */
    void sayText(const std::string& toSay);

    /**
    * Make Nao say a sentence and return its length.
    */
    int sayTextAndReturnLength(const std::string &toSay);

};
#endif // HELLOWORLD_H


/**
 * Copyright (c) 2010 Aldebaran Robotics
 */

#ifndef _WIN32
# include <signal.h>
#endif

#include <alcommon/albroker.h>
#include <alcommon/albrokermanager.h>
#include <alcommon/altoolsmain.h>
//#include "helloworld.h"

#ifdef HELLOWORLD_IS_REMOTE
# define ALCALL
#else
// when not remote, we're in a dll, so export the entry point
# ifdef _WIN32
#  define ALCALL __declspec(dllexport)
# else
#  define ALCALL
# endif
#endif

extern "C"
{
  ALCALL int _createModule(boost::shared_ptr<AL::ALBroker> pBroker)
  {
    // init broker with the main broker instance
    // from the parent executable
    AL::ALBrokerManager::setInstance(pBroker->fBrokerManager.lock());
    AL::ALBrokerManager::getInstance()->addBroker(pBroker);
    // create module instances
    AL::ALModule::createModule<HelloWorld>(pBroker, "HelloWorld");
    return 0;
  }

  ALCALL int _closeModule(  )
  {
    return 0;
  }
} // extern "C"
