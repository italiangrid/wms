/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://project.eu-egee.org/index.php?id=115 for details on the
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
#include <string>

#include "glite/wms/common/utilities/LineParser.h"
#include "LineParserExceptions.h"

using namespace std;

namespace glite {
namespace wms {
namespace common {
namespace utilities {

const char *LineParsingError::lpe_s_what = "Line parsing error";

LineParsingError::LineParsingError( const ParserData &d, int code ) : exception(), lpe_retcode( code ), lpe_data( d ) {}

LineParsingError::LineParsingError( const LineParsingError &lpe ) : exception(),
								    lpe_retcode( lpe.lpe_retcode ), lpe_data( lpe.lpe_data )
{}

LineParsingError::~LineParsingError( void ) throw() {}

const char *LineParsingError::what( void ) const throw()
{
  return lpe_s_what;
}

ShowHelp::ShowHelp( const ParserData &pd ) : LineParsingError( pd, 0 ) {}

ShowHelp::ShowHelp( const ShowHelp &sh ) : LineParsingError( sh ) {}

ShowHelp::~ShowHelp( void ) throw() {}

void ShowHelp::usage( ostream &os ) const
{
  this->lpe_data.usage( os );
}

InvalidOption::InvalidOption( const ParserData &pd, int opt ) : LineParsingError( pd, -1 ), io_opt( opt ) {}

InvalidOption::InvalidOption( const InvalidOption &io ) : LineParsingError( io ), io_opt( io.io_opt ) {}

InvalidOption::~InvalidOption( void ) throw() {}

void InvalidOption::usage( ostream &os ) const
{
  os << this->lpe_data.program() << ": invalid option '" << (char) this->io_opt << "'\n";
  this->lpe_data.usage( os );
}

InvalidArgNumber::InvalidArgNumber( const ParserData &pd, int argn ) : LineParsingError( pd, -1 ), ian_argn( argn ) {}

InvalidArgNumber::InvalidArgNumber( const InvalidArgNumber &ian ) : LineParsingError( ian ), ian_argn( ian.ian_argn ) {}

InvalidArgNumber::~InvalidArgNumber( void ) throw() {}

void InvalidArgNumber::usage( ostream &os ) const
{
  os << this->lpe_data.program() << ": invalid number of arguments, " << this->ian_argn 
     << "\n" << this->lpe_data.program() << ' ';

  if( this->lpe_data.arguments() == ParserData::zero_args )
    os << "does not accept parameters.";
  else if( this->lpe_data.arguments() == ParserData::one_or_more )
    os << "needs at least one argument.";
  else if( this->lpe_data.arguments() == ParserData::zero_or_more )
    os << "needs something ???"; // This should never happen
  else
    os << "needs exactly " << this->lpe_data.arguments() << " arguments.";

  os << endl;
  this->lpe_data.usage( os );

  return;
}

} // utilities namespace
} // common namespace
} // wms namespace
} // glite namespace
