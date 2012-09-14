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

#include <cstdlib>

#include <string>
#include <vector>
#include <fstream>
#include <memory>

#include <classad_distribution.h>

#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/exception.hpp>
#include <boost/regex.hpp>

#include "glite/wmsutils/classads/classad_utils.h"
#include "utilities/boost_fs_add.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "JCConfiguration.h"
#include "LMConfiguration.h"
#include "NSConfiguration.h"
#include "WMConfiguration.h"
#include "WMCConfiguration.h"
#include "WMPConfiguration.h"
#include "ICEConfiguration.h"
#include "CommonConfiguration.h"
#include "glite/wms/common/configuration/exceptions.h"

namespace fs = boost::filesystem;
namespace utilities = glite::wms::common::utilities;
using namespace std;

namespace glite {
namespace wms {
namespace common {
namespace configuration {

const Configuration  *Configuration::c_s_instance = NULL;
const char *Configuration::c_s_paths[] = { "/etc/glite-wms", "/opt/glite/etc", "/etc" };

namespace {

classad::ClassAd *parse_and_copy_classad( const classad::ClassAd *original )
{
  char                               *cval;
  classad::ClassAd                   *copy = new classad::ClassAd;
  string                              result, prima, variabile, dopo;
  classad::AttrList::const_iterator   itr;
  classad::Value                      value;
  classad::EvalState                  state;
  boost::match_results<string::const_iterator>    pieces;
  static boost::regex      expression( "^(.*)\\$\\{(.+)\\}(.*)$" );

  for( itr = original->begin(); itr != original->end(); ++itr ) {
    switch( itr->second->GetKind() ) {
    case classad::ExprTree::LITERAL_NODE:
      itr->second->Evaluate( state, value );

      if( value.IsStringValue(result) ) {
	if( result.size() != 0 ) {
	  while( boost::regex_match(result, pieces, expression) ) {
	    prima.assign( pieces[1].first, pieces[1].second );
	    variabile.assign( pieces[2].first, pieces[2].second );
	    dopo.assign( pieces[3].first, pieces[3].second );

	    if( (cval = getenv(variabile.c_str())) != NULL )
	      result = prima + string(cval) + dopo;
	    else throw UndefinedVariable( variabile );
	  }

	  if( result.size() != 0 ) copy->InsertAttr( itr->first, result );
	}
      }
      else
	copy->Insert( itr->first, itr->second->Copy() );

      break;
    default:
      copy->Insert( itr->first, itr->second->Copy() );

      break;
    }
  }

  return copy;
}

} // Namespace closure

void Configuration::loadFile( const char *filename )
{
  int                           type;
  classad::ExprTree            *test;
  classad::ClassAd             *part;
  ifstream                      ifs( filename );
  string                        expression;
  classad::ClassAdParser        parser;

  if( ifs.good() ) {
    part = parser.ParseClassAd( ifs );

    if( part == NULL ) throw CannotReadFile( filename );
    this->c_read.reset( part );
  }
  else throw CannotOpenFile( filename );

  test = this->c_read->Lookup( "Common" );
  if( test == NULL ) throw InvalidExpression( "Common" );

  part = dynamic_cast<classad::ClassAd *>( test );
  this->c_common.reset( new CommonConfiguration(part) );

  for( type = (static_cast<int>(ModuleType::unknown) + 1); type < static_cast<int>(ModuleType::last_module); ++type ) {
    expression.assign( ModuleType::module_name(static_cast<ModuleType::module_type>(type)) );

    test = this->c_read->Lookup( expression.c_str() );

    if( test == NULL ) throw InvalidExpression( expression );
    part = dynamic_cast<classad::ClassAd *>( test );

    switch( type ) {
    case ModuleType::network_server:
      this->c_ns.reset( new NSConfiguration(part) );

      break;
    case ModuleType::workload_manager:
      this->c_wm.reset( new WMConfiguration(part) );

      break;
    case ModuleType::job_controller:
      this->c_jc.reset( new JCConfiguration(part) );

      break;
    case ModuleType::log_monitor:
      this->c_lm.reset( new LMConfiguration(part) );

      break;
    case ModuleType::wms_client:
      this->c_wc.reset( new WMCConfiguration(part) );

      break;
    case ModuleType::workload_manager_proxy:
      this->c_wp.reset( new WMPConfiguration(part) );
      
      break;
    case ModuleType::interface_cream_environment:
      this->c_ice.reset( new ICEConfiguration(part) );

      break;
    default:
      break;
    }
  }

  return;
}

void Configuration::createConfiguration( const string &filename )
try {
  char                        *value;
  vector<string>               spaths;
  vector<string>::iterator     pathIt;
  fs::path      complete, name( filename );

  if( (value = getenv("GLITE_WMS_CONFIG_DIR")) != NULL )
    spaths.push_back( utilities::normalize_path(value) );

  spaths.insert( spaths.end(), c_s_paths, c_s_paths + (sizeof(c_s_paths) / sizeof(char *)) );

  for (pathIt = spaths.begin(); pathIt != spaths.end(); ++pathIt) {
    complete = fs::path(*pathIt, fs::native) / name;

    if (fs::exists(complete)) {
      break;
    }
  }

  if( pathIt == spaths.end() ) throw CannotFindFile( filename, spaths );

  this->loadFile( complete.native_file_string().c_str() );

  return;
}
catch( fs::filesystem_error &err ) {
  throw OtherErrors( err.what() );
}

Configuration::Configuration( const string &filename, const ModuleType &type ) : c_jc(), c_lm(), c_ns(), c_wm(), c_wc(), c_wp(), c_ice(), c_common(), c_read(), c_mtype( type )
{
  if( this->c_mtype.get_codetype() == ModuleType::unknown ) throw ModuleMismatch( this->c_mtype );

  if( c_s_instance == NULL ) {
    c_s_instance = this;
    this->createConfiguration( filename );
  }
}

Configuration::Configuration( const ModuleType &type ) : c_jc(), c_lm(), c_ns(), c_wm(), c_wc(), c_wp(), c_ice(), c_common(), c_read(), c_mtype( type )
{
  char       *filename;

  if( this->c_mtype.get_codetype() == ModuleType::unknown ) throw ModuleMismatch( this->c_mtype );

  if( (filename = getenv("GLITE_WMS_CONFIG_FILENAME")) == NULL )
    throw OtherErrors( "environment variable \"GLITE_WMS_CONFIG_FILENAME\" unset." );
  else if( c_s_instance == NULL ) {
    c_s_instance = this;
    this->createConfiguration( filename );
  }
}

Configuration::~Configuration( void )
{
  if( c_s_instance ) c_s_instance = NULL;
}

classad::ClassAd *Configuration::get_classad( void )
{
  classad::ClassAd    *total = new classad::ClassAd;

  total->Insert( "JobController", parse_and_copy_classad(this->c_jc->get_classad()) );
  total->Insert( "LogMonitor", parse_and_copy_classad(this->c_lm->get_classad()) );
  total->Insert( "NetworkServer", parse_and_copy_classad(this->c_ns->get_classad()) );
  total->Insert( "WorkloadManager", parse_and_copy_classad(this->c_wm->get_classad()) );
  total->Insert( "WmsClient", parse_and_copy_classad(this->c_wc->get_classad()) );  
  total->Insert( "WorkloadManagerProxy", parse_and_copy_classad(this->c_wp->get_classad()) );
  total->Insert( "InterfaceCreamEnvironment", parse_and_copy_classad(this->c_ice->get_classad()) );
  total->Insert( "Common", parse_and_copy_classad(this->c_common->get_classad()) );
  total->Insert( "ICE", parse_and_copy_classad(this->c_ice->get_classad()) );

  return total;
}

} // configuration namespace
} // common namespace
} // wms namespace
} // glite namespace

