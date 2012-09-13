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

#ifndef GLITE_WMS_COMMON_UTILITIES_LINEPARSEREXCEPTIONS_H
#define GLITE_WMS_COMMON_UTILITIES_LINEPARSEREXCEPTIONS_H

#include <exception>

namespace glite {
namespace wms {
namespace common {
namespace utilities {

class LineParsingError : public std::exception {
public:
  LineParsingError( const ParserData &d, int code );
  LineParsingError( const LineParsingError &lpe );
  virtual ~LineParsingError( void ) throw();

  virtual void usage( std::ostream &os ) const = 0;
  virtual const char *what( void ) const throw();

  inline int return_code( void ) { return this->lpe_retcode; }

private:
  LineParsingError &operator=( const LineParsingError & ); // Not implemented

protected:
  int                  lpe_retcode;
  const ParserData    &lpe_data;

  static const char   *lpe_s_what;
};

class ShowHelp : public LineParsingError {
public:
  ShowHelp( const ParserData &d );
  ShowHelp( const ShowHelp &sh );
  virtual ~ShowHelp( void ) throw();

  virtual void usage( std::ostream &os ) const;
};

class InvalidOption : public LineParsingError {
public:
  InvalidOption( const ParserData &d, int opt );
  InvalidOption( const InvalidOption &io );
  virtual ~InvalidOption( void ) throw();

  virtual void usage( std::ostream &os ) const;

private:
  int   io_opt;
};

class InvalidArgNumber : public LineParsingError {
public:
  InvalidArgNumber( const ParserData &d, int argn );
  InvalidArgNumber( const InvalidArgNumber &ian );
  virtual ~InvalidArgNumber( void ) throw();

  virtual void usage( std::ostream &os ) const;

private:
  int    ian_argn;
};

} // utilities namespace
} // common namespace
} // wms namespace
} // glite namespace

inline std::ostream &operator<<( std::ostream &os, const glite::wms::common::utilities::LineParsingError &lpe )
{ lpe.usage( os ); return os; }

#endif /* GLITE_WMS_COMMON_UTILITIES_LINEPARSEREXCEPTIONS_H */

// Local Variables:
// mode: c++
// End:
