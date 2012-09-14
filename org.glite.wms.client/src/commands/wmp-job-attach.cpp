
#include <iostream>
// utilities
#include "utilities/options_utils.h"
// wmproxy API
#include "glite/wms/wmproxyapi/wmproxy_api.h"

#include "services/jobattach.h"

using namespace std ;
using namespace glite::wms::client::utilities ;
using namespace glite::wms::wmproxyapi;

int main (int argc,char **argv){
	string jobid = "";
	try {
		Options opts (Options::JOBATTACH) ;
		opts.readOptions(argc, (const char**)argv);
		// gets the jobids
		vector<string> jobids = opts.getJobIds( );
		if (jobids.size() != 1 ){
			cerr << "ERROR: only 1 jobid !!!\n";
			throw exception();
		}

	} catch (exception &ex) {
		cout << flush << ex.what() << "\n" ;
	}

};
