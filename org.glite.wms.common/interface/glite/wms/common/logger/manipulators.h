/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org\partners for details on the
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

#ifndef GLITE_WMS_COMMON_LOGGER_MANIPULATORS_H
#define GLITE_WMS_COMMON_LOGGER_MANIPULATORS_H

#include <iostream>
#include <string>

#include "glite/wms/common/logger/common.h"

namespace glite {
namespace wms {
namespace common {
namespace logger {

class Logbuf;
namespace threadsafe { class logstream; };

class StatePusher {
public:
  StatePusher( std::ostream &os, const char *func = NULL );
  StatePusher( std::ostream &os, const std::string &func );
  StatePusher( threadsafe::logstream &os, const char *func = NULL );
  StatePusher( threadsafe::logstream &os, const std::string &func );
  ~StatePusher( void );

private:
  void setState( const char *func );

  StatePusher( const StatePusher &fp ); // Not implemented
  StatePusher &operator=( const StatePusher &fp ); // Not implemented

  Logbuf                *fp_buffer;
  DataContainerSingle    fp_data;
};

#ifndef LOGGER_FUTURE_IMPLEMENTATION
class setfunction {
  friend std::ostream &operator<<( std::ostream &os, const setfunction &sf );
  friend threadsafe::logstream &operator<<( threadsafe::logstream &os, const setfunction &sf );

public:
  setfunction( const char *func );
  setfunction( const std::string &func );

private:
  std::string   sf_function;
};
#endif /* !LOGGER_FUTURE_IMPLEMENTATION */

class settimeformat {
  friend std::ostream &operator<<( std::ostream &os, const settimeformat &sf );
  friend threadsafe::logstream &operator<<( threadsafe::logstream &os, const settimeformat &sf );

public:
  settimeformat( const char *format );
  settimeformat( const std::string &func );

private:
  std::string   stf_format;
};

class setlevel {
  friend std::ostream &operator<<( std::ostream &os, const setlevel &sf );
  friend threadsafe::logstream &operator<<( threadsafe::logstream &os, const setlevel &sf );

public:
  setlevel( level_t lev );

private:
  level_t      sl_level;
};

class setcurrent {
  friend std::ostream &operator<<( std::ostream &os, const setcurrent &sf );
  friend threadsafe::logstream &operator<<( threadsafe::logstream &os, const setcurrent &sf );

public:
  setcurrent( level_t lev );

private:
  level_t      sc_level;
};

class setmultiline {
  friend std::ostream &operator<<( std::ostream &os, const setmultiline &sm );
  friend threadsafe::logstream &operator<<( threadsafe::logstream &os, const setmultiline &sm );

public:
  setmultiline( bool multi, const char *prefix = NULL );

private:
  bool         sm_multi;
  const char  *sm_prefix;
};

class setshowseverity {
  friend std::ostream &operator<<( std::ostream &os, const setshowseverity &ss );
  friend threadsafe::logstream &operator<<( threadsafe::logstream &os, const setshowseverity &ss );

public:
  setshowseverity( bool show );

private:
  bool     ss_show;
};

#ifndef LOGGER_FUTURE_IMPLEMENTATION
std::ostream &operator<<( std::ostream &os, const setfunction &sf );
#endif /* !LOGGER_FUTURE_IMPLEMENTATION */
std::ostream &operator<<( std::ostream &os, const settimeformat &stf );
std::ostream &operator<<( std::ostream &os, const setlevel &sl );
std::ostream &operator<<( std::ostream &os, const setcurrent &sc );
std::ostream &operator<<( std::ostream &os, const setmultiline &sc );

#ifndef LOGGER_FUTURE_IMPLEMENTATION
threadsafe::logstream &operator<<( threadsafe::logstream &os, const setfunction &sf );
#endif /* !LOGGER_FUTURE_IMPLEMENTATION */
threadsafe::logstream &operator<<( threadsafe::logstream &os, const settimeformat &sf );
threadsafe::logstream &operator<<( threadsafe::logstream &os, const setlevel &sf );
threadsafe::logstream &operator<<( threadsafe::logstream &os, const setcurrent &sf );
threadsafe::logstream &operator<<( threadsafe::logstream &os, const setmultiline &sf );

} // Namespace logger
} // common namespace
} // wms namespace
} // glite namespace

#endif /* GLITE_WMS_COMMON_LOGGER_MANIPULATORS_H */

// Local Variables:
// mode: c++
// End:
