
#include <iostream>
// utilities
#include "utilities/options_utils.h"
// wmproxy API
#include "glite/wms/wmproxyapi/wmproxy_api.h"

#include "services/jobcancel.h"
using namespace std ;
using namespace glite::wms::client::services ;
using namespace glite::wmsutils::exception;

/*
*	main
*/
int main (int argc,char **argv){
	JobCancel job ;
        // reads the user options
	try {
		job.readOptions(argc, argv);
                job.cancel( );
	} catch (Exception &exc) {
		job.excMsg("", exc, argv[0]);
		return 1;		
	}
	return 0;
};
