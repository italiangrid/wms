
#include "iostream"
// utilities
#include "utilities/options_utils.h"
// exceptions
#include "utilities/excman.h"

#include "services/delegateproxy.h"

using namespace std ;
using namespace glite::wms::client::services ;
using namespace glite::wmsutils::exception;
/*
*	main
*/
int main (int argc,char **argv){
	DelegateProxy job ;
        // reads the user options
	try {
		job.readOptions(argc, argv);
                job.delegation( );
	} catch (Exception &exc) {
		job.excMsg("", exc, argv[0]);
		return 1;
	}
	return 0;
};
