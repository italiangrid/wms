#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"

#include "glite/wms/rls/catalog_access_utils.h"

#include "glite/wms/common/configuration/exceptions.h"

#include "glite/wms/common/logger/logger_utils.h"


#include "glite/wms/common/configuration/CommonConfiguration.h"
#include "glite/wms/common/utilities/LineParser.h"
#include "glite/wms/common/utilities/LineParserExceptions.h"




#include <glite/wmsutils/classads/classad_utils.h>

#include <classad_distribution.h>

#include <iostream>
#include <string>
#include <fstream>


using namespace std;
using namespace glite::wms::common::utilities;
using namespace glite::wmsutils::classads;
using namespace glite::wms::common::configuration;

using namespace glite::wms::rls;



LineOption  options[] = {
    { 'c', 1, "conf_file", "\t use conf_file as configuration file. glite_wms.conf is the default" },
    { 'j', 1, "jdl_file", "\t use jdl_file as input file." },
    { 'n', 1, "number", "\t number of retry" },
};

void
print_file_mapping_info(
   filemapping::value_type const& i
)
{
  std::vector<std::string> const& sfn(
    i.second
  );
  std::string lfn( i.first );
  std::cout << lfn << " = {\n";
  std::copy(
    sfn.begin(),
    sfn.end(),
    ostream_iterator<string>(std::cout, "\n")
  );
  std::cout << "}\n"; 
}

int main(int argc, char* argv[])
{
  std::vector<LineOption> optvec( options, options + sizeof(options)/sizeof(LineOption) );
  LineParser options( optvec, 0 );

  string conf_file;

  string req_file;

  try {
     options.parse( argc, argv );
     conf_file.assign( options.is_present('c') ? options['c'].getStringValue() : "glite_wms.conf" );

     Configuration conf(conf_file.c_str(), ModuleType::network_server);

     NSConfiguration const* const ns_config(conf.ns());

     if( ! options.is_present('j') )
     {
        Error( "an input file with the jdl must be passed");
        return -1;
     }
     else
        req_file.assign(options['j'].getStringValue());

     int count = 1;
     if( options.is_present('n') )  count = options['n'].getIntegerValue();

     Debug( "BEGIN" );
     Debug("---------------------------------------------------");

     ifstream fin(req_file.c_str());    
     if( fin ) 
     { 
        classad::ClassAdParser parser;
    	classad::ClassAd *reqAd = parser.ParseClassAd(fin);
        //Debug(*reqAd);

        for (int i=0; i < count; i++) {
           boost::shared_ptr<filemapping> fm(
   	     resolve_filemapping_info(*reqAd)
           );
           std::for_each(
             fm->begin(),
             fm->end(),
             print_file_mapping_info
           );
        }
     }
     else Warning("cannot open jdl file" );

     Debug( "---------------------------------------------------" );
     Debug( "END");
  
  }
  catch ( LineParsingError &er ) {
    cerr << er << endl;
    exit( er.return_code() );
  }
  catch( CannotConfigure &er ) {
    cerr << er << endl;
  }
  catch( ... ) {
    cout << "Uncaught exception..." << endl;
  }	 
  return 0;

}
