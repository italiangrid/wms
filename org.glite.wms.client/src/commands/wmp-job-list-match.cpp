
#include <iostream>
// utilities
#include "utilities/options_utils.h"
// exceptions
#include "utilities/excman.h"

#include "services/joblistmatch.h"

using namespace std ;
using namespace glite::wms::client::services ;
using namespace glite::wms::client::utilities ;
using namespace glite::wmsutils::exception;

/*
*	main
*/
int main (int argc,char **argv){
	JobListMatch job ;
        // reads the user options
	try {
                // reads the user options
		job.readOptions (argc, argv);
                // performs the main operations
                job.listMatching( );
	} catch (Exception &exc) {
		job.excMsg("", exc, argv[0]);
		return 1;
	}
	return 0;
};
