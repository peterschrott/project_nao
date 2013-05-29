/**
 * @author Peter Schrott
 */

#ifndef WALK_H
#define WALK_H

#include <iostream>
#include <string>
#include <pthread.h>
#include <boost/shared_ptr.hpp>
#include <stdio.h>

#include <alvalue/alvalue.h>
#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
#include <alcommon/albrokermanager.h>

#include <alproxies/almotionproxy.h>
#include <alproxies/alrobotpostureproxy.h>

namespace AL
{
  class ALBroker;
}

class Walk : public AL::ALModule
{
  public:
    /**
     * Default Constructor for modules.
     * @param broker the broker to which the module should register.
     * @param name the boadcasted name of the module.
     */
    Walk(boost::shared_ptr<AL::ALBroker> broker, const std::string& name);

    /// Destructor.
    virtual ~Walk();

    virtual void init();

    void walkUpAndDown();

};

#endif