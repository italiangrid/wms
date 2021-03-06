/* Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the copyright
holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <config.h>
#include <user_log.c++.h>

#include "glite/wms/common/configuration/Configuration.h"
#include "common/src/configuration/LMConfiguration.h"
#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"
#include "common/src/utilities/streamdescriptor.h"
#include "jobcontrol_namespace.h"
#include "logmonitor/SizeFile.h"

#include "MonitorData.h"
#include "EventFactory.h"
#include "EventInterface.h"
#include "EventUnhandled.h"
#include "EventSubmit.h"
#include "EventGlobusSubmit.h"
#include "EventExecute.h"
#include "EventTerminated.h"
#include "EventPostTerminated.h"
#include "EventAborted.h"
#include "EventGlobusSubmitFailed.h"
#include "EventGlobusResourceDown.h"
#include "EventGlobusResourceUp.h"
#include "EventGridSubmit.h"
#include "EventGridResourceDown.h"
#include "EventGridResourceUp.h"
#include "EventJobHeld.h"
#include "EventGeneric.h"
#include "EventJobReleased.h"

USING_COMMON_NAMESPACE;
using namespace std;
RenameLogStreamNS( elog );

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor { namespace processer {

EventFactory::EventFactory( boost::shared_ptr<MonitorData> &data ) : ef_data( data )
{}

EventFactory::~EventFactory( void )
{}

EventInterface *EventFactory::create_processor( ULogEvent *event, bool removeTimer )
{
  EventInterface   *processer = NULL;

  switch( event->eventNumber ) {
  case ULOG_SUBMIT:
    processer = new EventSubmit( event, this->ef_data.get() );
    break;
  case ULOG_GLOBUS_SUBMIT:
    processer = new EventGlobusSubmit( event, this->ef_data.get() );
    break;
  case ULOG_EXECUTE:
    processer = new EventExecute( event, this->ef_data.get() );
    break;
  case ULOG_JOB_TERMINATED:
    processer = new EventTerminated( event, this->ef_data.get() );
    break;
  case ULOG_POST_SCRIPT_TERMINATED:
    processer = new EventPostTerminated( event, this->ef_data.get() );
    break;
  case ULOG_JOB_ABORTED:
    processer = new EventAborted( event, this->ef_data.get(), removeTimer );
    break;
  case ULOG_GLOBUS_SUBMIT_FAILED:
    processer = new EventGlobusSubmitFailed( event, this->ef_data.get() );
    break;
  case ULOG_GLOBUS_RESOURCE_DOWN:
    processer = new EventGlobusResourceDown( event, this->ef_data.get() );
    break;
  case ULOG_GLOBUS_RESOURCE_UP:
    processer = new EventGlobusResourceUp( event, this->ef_data.get() );
    break;					
  case ULOG_GRID_SUBMIT:
    processer = new EventGridSubmit( event, this->ef_data.get() );
    break;
  case ULOG_GRID_RESOURCE_DOWN:
    processer = new EventGridResourceDown( event, this->ef_data.get() );
    break;
  case ULOG_GRID_RESOURCE_UP:
    processer = new EventGridResourceUp( event, this->ef_data.get() );
    break;					
  case ULOG_JOB_HELD:
    processer = new EventJobHeld( event, this->ef_data.get() );
    break;
  case ULOG_JOB_RELEASED:
    processer = new EventJobReleased(event, this->ef_data.get());
    break;
  case ULOG_GENERIC:
    processer = new EventGeneric( event, this->ef_data.get() );
    break;
  default:
    processer = new EventUnhandled( event, this->ef_data.get() );
    break;
  }

  return processer;
}

}} // Namespace processer, logmonitor

} JOBCONTROL_NAMESPACE_END
