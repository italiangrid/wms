#include <iostream>
#include <string>

#include "glite/wms/common/utilities/LineParser.h"
#include "utilities/LineParserExceptions.h"
#include "utilities/globus_ftp_utils.h"
#include "glite/wms/common/logger/edglog.h"

namespace utilities     = glite::wms::common::utilities;
namespace logger        = glite::wms::common::logger::threadsafe;

using namespace std;
using namespace utilities;

LineOption  options[] = {
        { 'e', no_argument, "exists",  "\t checks only existence." },
        { 'p', no_argument, "parents", "\t make parent directories as needed." },
        { 'v', no_argument, "verbose", "\t print a message for each created directory." }
};

int main(int argc, char *argv[])
{
  vector<LineOption>    optvec( options, options + sizeof(options)/sizeof(LineOption) );
  LineParser            options( optvec, 1 );
  std::string 		destination;
  
  try {

  options.parse( argc, argv );
  destination.assign( options.get_arguments()[0] );

  if( options.is_present('p') ) {
	  std::string dst(destination);
          size_t last_pos  = dst.find('/');
          size_t slash_pos = 0;

          while( (slash_pos = dst.find('/',last_pos + 1)) != std::string::npos ) {
                 std::string parent(dst.substr(0, slash_pos));
                 last_pos = slash_pos;
                 if ( !utilities::globus::exists(string("gsiftp://") + parent))
                 {
                   if (!options.is_present('e')) {
			   utilities::globus::mkdir(string("gsiftp://") + parent);
		   	   if (options.is_present('v')) 
				   logger::edglog << "[globus_ftp_client] creating remote directory gsiftp://" << parent << std::endl; 
		 
		   }
		 }
		 else if (options.is_present('e')) 
			 logger::edglog << "[globus_ftp_client] gsiftp://" << parent << " already exists" << std::endl;
          }
  }
  if (options.is_present('e')) {
	  if ( utilities::globus::exists(string("gsiftp://") + destination))
	  	logger::edglog << "[globus_ftp_client] gsiftp://" << destination << " already exists" << std::endl;				   
  }
  else {
	  utilities::globus::mkdir(string("gsiftp://")+destination);
  	  if (options.is_present('v')) logger::edglog << "[globus_ftp_client] creating remote directory gsiftp://" << destination << std::endl;
  }
  } catch( LineParsingError &error ) {
      cerr << error << endl;
      exit( error.return_code() );
    }
 
  return 0;
}

