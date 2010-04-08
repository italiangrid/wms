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

#include <iostream>
#include <algorithm>

#include "glite/wms/common/utilities/LineParser.h"
#include "LineParserExceptions.h"

using namespace std;

namespace glite {
namespace wms {
namespace common {
namespace utilities {

const string LineParser::lp_s_empty;

option_s LineOption::get_struct( void ) const
{
  option_s     translate;

  translate.name = this->o_name.c_str();
  translate.has_arg = this->o_has_arguments;
  translate.flag = NULL;
  translate.val = (int) this->o_value;

  return translate;
}

ParserData::ParserData( const vector<LineOption> &options, int parn ) : pd_paramnumber( parn ), pd_optstring(), pd_progname(),
									pd_options(), pd_argmap(), pd_help()
{
  option_s                            last = { NULL, 0, NULL, 0 }, helpopt = { "help", 0, NULL, 'h' };
  vector<LineOption>::const_iterator  optIt;
  string                              help, helpstring;

  for( optIt = options.begin(); optIt != options.end(); optIt++ ) {
    this->pd_optstring.append( 1, optIt->get_value() );
    this->pd_options.push_back( optIt->get_struct() );

    help.assign( "\t-" ); help.append( 1, optIt->get_value() );
    help.append( " --" ); help.append( optIt->get_name() );

    switch( optIt->has_arguments() ) {
    case no_argument:
    default:
      break;
    case required_argument:
      this->pd_optstring.append( 1, ':' );
      help.append( "=<argument>" );

      break;
    case optional_argument:
      this->pd_optstring.append( 2, ':' );
      help.append( "[=argument]" );

      break;
    }

    help.append( 1, '\n' ); help.append( optIt->get_help() );

    this->pd_argmap.insert( map<char, int>::value_type(optIt->get_value(), optIt->has_arguments()) );
    this->pd_help.insert( map<char, string>::value_type(optIt->get_value(), help) );
  }

  help.append( 1, '\n' ); help.append( "\t\tShow this help and exit." );

  this->pd_options.push_back( helpopt ); this->pd_optstring.append( 1, 'h' );

  this->pd_options.push_back( last );

  return;
}

ParserData::~ParserData( void ) {}

void ParserData::usage( ostream &os ) const
{
  map<char, string>::const_iterator   helpIt;

  os << "Usage: " << this->pd_progname << " ";

  if( !this->pd_help.empty() ) os << "[options] ";

  if( this->pd_paramnumber == (int) one_or_more ) os << "arg1 [arg2...]";
  else if( this->pd_paramnumber == (int) zero_or_more ) os << "[arg1 arg2...]";
  else if( this->pd_paramnumber == 1 ) os << "<arg>";
  else if( this->pd_paramnumber > 1 ) os << "<arg1...arg" << this->pd_paramnumber << ">";

  if( !this->pd_help.empty() ) {
    os << "\nWhere [options] can be:" << endl;

    for( helpIt = this->pd_help.begin(); helpIt != this->pd_help.end(); helpIt++ )
      os << helpIt->second << endl;
  }

  os << endl;

  return;
}

LineParser::LineParser( vector<LineOption> &options, int argn ) : lp_map(), lp_arguments(), lp_data( options, argn )
{}

LineParser::~LineParser( void ) {}

LineParser &LineParser::parse( int argn, char *const *argv )
{
  int             val, arg, diff;
  Mixed           optional;
  const option_s *begin = &(*this->lp_data.pd_options.begin());

  while( (val = getopt_long(argn, argv, this->lp_data.pd_optstring.c_str(), begin, NULL)) != EOF ) {
    if( val == 'h' ) {
      this->lp_data.pd_progname.assign( argv[0] );
      throw ShowHelp( this->lp_data );
    }
    else if( val == '?' ) {
      this->lp_data.pd_progname.assign( argv[0] );
      throw InvalidOption( this->lp_data, optopt );
    }
    else if( this->lp_data.pd_argmap.count(val) != 0 ) {
      switch( this->lp_data.pd_argmap[val] ) {
      case no_argument:
	this->lp_map.insert( map<char, Mixed>::value_type((char) val, Mixed(true)) );
	break;
      case required_argument:
	this->lp_map.insert( map<char, Mixed>::value_type((char) val, Mixed(optarg)) );
	break;
      case optional_argument:
      default:
	if( optarg == NULL ) {
	  if( (optind < argn) && (argv[optind][0] != '-') ) {
	    optional.setStringValue( argv[optind] );
	    optind += 1;
	  }
	  else optional.setLogicalValue( true );
	}
	else optional.setStringValue( optarg );

	this->lp_map.insert( map<char, Mixed>::value_type((char) val, optional) );
	break;
      }
    }
  }

  diff = argn - optind;
  if( (((this->lp_data.pd_paramnumber == (int) ParserData::one_or_more) && (diff > 0)) ||
       ((this->lp_data.pd_paramnumber == (int) ParserData::zero_or_more) && (diff >= 0)) ||
       ((this->lp_data.pd_paramnumber >= 0) && (diff == this->lp_data.pd_paramnumber))) ) {
    for( arg = optind; arg < argn; arg++ )
      this->lp_arguments.push_back( argv[arg] );
  }
  else {
    this->lp_data.pd_progname.assign( argv[0] );
    throw InvalidArgNumber( this->lp_data, diff );
  }

  return *this;
}

const LineParser &LineParser::print( ostream &os ) const
{
  map<char, Mixed>::const_iterator    mapIt;
  vector<string>::const_iterator      argIt;

  for( mapIt = this->lp_map.begin(); mapIt != this->lp_map.end(); mapIt++ )
    os << mapIt->first << "\t-> " << mapIt->second << endl;

  if( this->lp_arguments.size() != 0 ) {
    for( argIt = this->lp_arguments.begin(); argIt != this->lp_arguments.end(); argIt++ )
      os << *argIt << " ";

    os << endl;
  }

  return *this;
}

} // utilities namespace
} // common namespace
} // wms namespace
} // glite namespace
