#include <cstdlib>

#include <iostream>

#include "configuration/Configuration.h"
#include "configuration/NSConfiguration.h"
#include "configuration/JCConfiguration.h"
#include "configuration/exceptions.h"

using namespace std;
using glite::wms::common;
namespace configuration = glite::wms::common::configuration;

int main( void )
{
//    setenv( "GLITE_WMS_CONFIG_DIR", "", 1 ); // This is needed to find the file in the current directory

  configuration::Configuration   *configuration;

  try {
    configuration = new configuration::Configuration( "glite_ns.conf", "JobController" );

    cout << "Condor submit = " << configuration->jc()->condor_submit() << endl;
    cout << "Condor remove = " << configuration->jc()->condor_remove() << endl;
		/*
    cout << "Condor query  = " << configuration->jc()->condor_query() << endl;
    cout << "Condor submitDAG = " << configuration->jc()->condor_submit_dag() << endl;
		*/

    cout << "JCConfiguration = (" << (void *) configuration->jc()
	 << "), NSConfiguration = (" << (void *) configuration->ns() << ")" << endl;
  }
  catch( configuration::CannotConfigure &error ) {
    cerr << error << endl;
  }

  return( 0 );
}

/*
  Another way to do the same thing...
  Swap main and func and try to compile
*/

//  int func( void )
//  {
//    setenv( "GLITE_WMS_CONFIG_DIR", "", 1 ); // This is needed to find the file in the current directory

//    try {
//      configuration::Factory::create_factory( "JobController", "glite_wms.conf" )->create();

//      cout << "Condor submit = " << configuration::JCConfiguration::instance()->condor_submit() << endl;
//      cout << "Condor remove = " << configuration::JCConfiguration::instance()->condor_remove() << endl;
//      cout << "Condor query  = " << configuration::JCConfiguration::instance()->condor_query() << endl;
//      cout << "Condor submitDAG = " << configuration::JCConfiguration::instance()->condor_submit_dag() << endl;

//      cout << "JCConfiguration = (" << reinterpret_cast<void *>(configuration::JCConfiguration::instance())
//  	 << "), NSConfiguration = (" << reinterpret_cast<void *>(configuration::NSConfiguration::instance()) << ")" << endl;
//    }
//    catch( configuration::CannotConfigure &error ) {
//      cerr << error << endl;
//    }

//    return( 0 );
//  }
