
#include "configuration.hpp"
#include "gestures.hpp"
#include "action.hpp"
#include "shapes.hpp"

int mainloop()
{
	return 1;
}


#ifdef PC
// #### From here  we want to run the source locally on a PC
int main( void )
{
	while( mainloop() ); // run our mainloop until it lets us know, it is finished
	return 0;
}

#else
// #### From here we are in a local module, running on NAO
#include <iostream>
#include <boost/shared_ptr.hpp>

#include <alcommon/almodule.h>
#include <alcommon/albroker.h>

using namespace AL;

/**
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
	/** Describe the module here. This will appear on the webpage*/
	setModuleDescription("Prog");
}

Prog::~Prog() 
{
	/** Destructor */
}

void Prog::init()
{
	/** Init is called just after construction.
	*/

	// TODO: do init stuff
	
	while( mainloop() ); // run our mainloop until it lets us know, it is finished
}

#endif
