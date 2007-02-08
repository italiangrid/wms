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
