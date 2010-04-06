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
#include <memory>

#include <classad_distribution.h>

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/exceptions.h"

using namespace glite::wms::common;
using namespace std;

int main( int argc, char *argv[] )
try {
  int                               ret = 0;
  const classad::ClassAd           *ad;
  const classad::ExprList          *lst;
  configuration::Configuration      config( "glite_wms.conf", "LogMonitor" );
  auto_ptr<classad::ClassAd>        complete( config.get_classad() );
  classad::Value                    value;
  classad::PrettyPrint              unp;
  string                            buffer;

  if( argc == 2 ) {
    complete->EvaluateExpr( string(argv[1]), value );

    switch( value.GetType() ) {
    case classad::Value::NULL_VALUE:
    case classad::Value::ERROR_VALUE:
    case classad::Value::UNDEFINED_VALUE:
    default:
      ret = 1;
      break;
    case classad::Value::BOOLEAN_VALUE:
    case classad::Value::INTEGER_VALUE:
    case classad::Value::REAL_VALUE:
    case classad::Value::STRING_VALUE:
      cout << value << endl;
      break;
    case classad::Value::CLASSAD_VALUE:
      value.IsClassAdValue( ad );
      cout << *ad << endl;
      break;
    case classad::Value::LIST_VALUE:
      value.IsListValue( lst );

      unp.Unparse( buffer, lst );
      cout << buffer << endl;

      break;
    }
  }
  else if( argc < 2 )
    cout << *complete << endl;
  else ret = 1;

  return ret;
}
catch( configuration::CannotConfigure &error ) {
  cerr << error << endl;

  return 1;
}
