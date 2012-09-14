
#include <iostream>
// utilities
#include "utilities/options_utils.h"
// wmproxy API
#include "glite/wms/wmproxyapi/wmproxy_api.h"

#include "services/jobloginfo.h"

using namespace std ;
using namespace glite::wms::client::utilities ;
using namespace glite::wms::wmproxyapi;

/*
*	main
*/
int main (int argc,char **argv){
/*
	ConfigContext *cfs = NULL;
	string *endpoint = NULL ;
*/
	vector<string> jobids;
	try {
		// reads the user options
		Options opts (Options::JOBLOGINFO) ;
		opts.readOptions(argc, (const char**)argv);
		// gets the jobids
		jobids = opts.getJobIds( );
	} catch (exception &ex) {
		cout << flush << ex.what() << "\n" ;
		return 1;		
	}
	return 0;
};
