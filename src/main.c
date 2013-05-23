
#include "configuration.h"
#include "gestures.h"
#include "action.h"
#include "shapes.h"

#include <boost/thread.hpp>
#include <alcommon/almodule.h>











bool mainloop()
{
	return false;
}






#ifdef PC

int main( void )
{
	while( mainloop() ); // run our mainloop until it lets us know, it is finished
	return 0;
}

#else


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
class Prog : public AL::ALModule
{
  public:
    Prog(boost::shared_ptr<AL::ALBroker> pBroker, const std::string& pName);

    virtual ~Prog();

    /** Overloading ALModule::init().
    * This is called right after the module has been loaded
    */
    virtual void init();

};


Prog::Prog(boost::shared_ptr<ALBroker> broker, const std::string& name):
  ALModule(broker, name)
{
	// TODO: here there will be everything the Nao needs to display infos about this module
}

void Prog::init()
{
	// TODO: do init stuff
	
	while( mainloop() ); // run our mainloop until it lets us know, it is finished
}




#endif
