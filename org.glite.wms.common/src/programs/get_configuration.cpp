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
