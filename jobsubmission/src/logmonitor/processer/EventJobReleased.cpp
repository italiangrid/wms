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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstdio>
#include <ctime>

#include <string>
#include <iostream>
#include <fstream>

#include <boost/lexical_cast.hpp>
#include "glite/wms/common/logger/logstream.h"
#include "glite/wms/common/logger/manipulators.h"
#include "common/EventLogger.h"

#include "EventJobReleased.h"
#include "MonitorData.h"

USING_COMMON_NAMESPACE;
RenameLogStreamNS(elog);

JOBCONTROL_NAMESPACE_BEGIN {

namespace logmonitor {
namespace processer {

EventJobReleased::EventJobReleased(ULogEvent *event, MonitorData *data)
  : EventInterface(event, data), event_(dynamic_cast<JobReleasedEvent*>(event))
{ }

void EventJobReleased::process_event()
{
  logger::StatePusher pusher(elog::cedglog, "EventJobReleased::process_event()");

  elog::cedglog << logger::setlevel(logger::info) << "Got a job release event for cluster: " << ei_condor << '\n';

  return;
}

}} // namespace processer, logmonitor
} JOBCONTROL_NAMESPACE_END
