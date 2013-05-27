/**
 * @author Peter Schrott
 */

#ifndef RECORD_VID_H
#define RECORD_VID_H

#include <iostream>
#include <string>
#include <pthread.h>
#include <boost/shared_ptr.hpp>
#include <stdio.h>

#include <alvalue/alvalue.h>
#include <alcommon/almodule.h>
#include <alcommon/albroker.h>
#include <alcommon/albrokermanager.h>

#include <alproxies/alvideorecorderproxy.h>

namespace AL
{
  class ALBroker;
}

class RecordVid : public AL::ALModule
{
  public:
    /**
     * Default Constructor for modules.
     * @param broker the broker to which the module should register.
     * @param name the boadcasted name of the module.
     */
    RecordVid(boost::shared_ptr<AL::ALBroker> broker, const std::string& name);

    /// Destructor.
    virtual ~RecordVid();

    virtual void init();

    void recordVideo();

};

#endif