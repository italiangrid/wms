/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners for details on the
copyright holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include <iostream>

#include "glite/wms/common/logger/manipulators.h"
#include "glite/wms/common/logger/logstream_ts.h"

using namespace std;

namespace glite {
namespace wms {
namespace common {
namespace logger {

StatePusher::StatePusher( threadsafe::logstream &os, const char *func ) : fp_buffer( os.logbuf() ), fp_data()
{
  this->setState( func );
}

StatePusher::StatePusher( threadsafe::logstream &os, const std::string &func ) : fp_buffer( os.logbuf() ), fp_data()
{
  this->setState( func.c_str() );
}

#ifndef LOGGER_FUTURE_IMPLEMENTATION
threadsafe::logstream &operator<<( threadsafe::logstream &ls, const setfunction &sf )
{
  ls.logbuf()->function( sf.sf_function.c_str() );

  return ls;
}
#endif /* !LOGGER_FUTURE_IMPLEMENTATION */

threadsafe::logstream &operator<<( threadsafe::logstream &ls, const settimeformat &stf )
{
  ls.logbuf()->time_format( stf.stf_format.c_str() );

  return ls;
}

threadsafe::logstream &operator<<( threadsafe::logstream &ls, const setlevel &sl )
{
  ls.logbuf()->next_level( sl.sl_level );

  return ls;
}

threadsafe::logstream &operator<<( threadsafe::logstream &ls, const setmultiline &sm )
{
  ls.logbuf()->multiline( sm.sm_multi, sm.sm_prefix );

  return ls;
}

/*
  These operators have to lock the internal mutex of the logstream
*/

threadsafe::logstream &operator<<( threadsafe::logstream &ls, const setcurrent &sc )
{
  boost::mutex::scoped_lock    lock(ls.get_mutex());

  ls.logbuf()->buffer_level( sc.sc_level );

  return ls;
}

threadsafe::logstream &operator<<( threadsafe::logstream &ls, const setshowseverity &ss )
{
  boost::mutex::scoped_lock   lock(ls.get_mutex());

  ls.logbuf()->show_severity( ss.ss_show );

  return ls;
}

} // logger namespace
} // common namespace
} // wms namespace
} // glite namespace
