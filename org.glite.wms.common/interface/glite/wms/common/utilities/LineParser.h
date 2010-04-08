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

#ifndef GLITE_WMS_COMMON_UTILITIES_LINEPARSER_H
#define GLITE_WMS_COMMON_UTILITIES_LINEPARSER_H

#include <map>
#include <vector>
#include <string>

#include <unistd.h>
#include <getopt.h>

#include "glite/wms/common/utilities/mixed.h"

namespace glite {
namespace wms {
namespace common {
namespace utilities {

typedef  struct option    option_s;

class LineOption {
public:
  inline char get_value( void ) const { return this->o_value; }
  inline int has_arguments( void ) const { return this->o_has_arguments; }

  inline const std::string &get_name( void ) const { return this->o_name; }
  inline const std::string &get_help( void ) const { return this->o_help; }

  inline option_s get_struct( void ) const;

  char            o_value;
  int             o_has_arguments;
  std::string     o_name, o_help;
};

class LineParser;

class ParserData {
  friend class LineParser;

public:
  enum par_num_t { one_or_more = -2, zero_or_more, zero_args };

  inline int arguments( void ) const { return this->pd_paramnumber; }
  inline const std::string &program( void ) { return this->pd_progname; }
  inline const std::string &program( void ) const { return this->pd_progname; }

  void usage( std::ostream &os ) const;

private:
  ParserData( const std::vector<LineOption> &options, int paramnumber );
  ~ParserData( void );

  int                          pd_paramnumber;
  std::string                  pd_optstring, pd_progname;
  std::vector<option_s>        pd_options;
  std::map<char, int>          pd_argmap;
  std::map<char, std::string>  pd_help;
};

class LineParser {
public:
  LineParser( std::vector<LineOption> &options, int paramnumber = 0 );
  ~LineParser( void );

  inline bool is_present( char val ) const { return( this->lp_map.count(val) != 0 ); }
  inline const Mixed &operator[]( char val ) const { return( this->lp_map.count(val) ? this->lp_map[val] : Mixed::zero() ); }
  inline const std::string &operator[]( int num ) const
  { return( ((num >= 0) && (num < (int) this->lp_arguments.size())) ? this->lp_arguments[num] : lp_s_empty ); }

  inline const std::string &get_optstring( void ) const { return this->lp_data.pd_optstring; }
  inline const std::map<char, Mixed> &get_map( void ) const { return this->lp_map; }
  inline const std::vector<std::string> &get_arguments( void ) const { return this->lp_arguments; }

  inline LineParser &usage( std::ostream &os ) { this->lp_data.usage( os ); return *this; }
  inline const LineParser &usage( std::ostream &os ) const { this->lp_data.usage( os ); return *this; }

  LineParser &parse( int argn, char *const *argv );
  const LineParser &print( std::ostream &os ) const;

private:
  mutable std::map<char, Mixed>  lp_map;
  std::vector<std::string>       lp_arguments;
  ParserData                     lp_data;

  static const std::string       lp_s_empty;
};

} // utilities namespace
} // common namespace
} // wms namespace
} // glite namespace

#endif /* GLITE_WMS_COMMON_UTILITIES_LINEPARSER_H */

// Local Variables:
// mode: c++
// End:
