
#include "glite/wms/common/utilities/FileList.h"
#include "glite/wms/common/utilities/FileLocker.h"
#include "glite/wms/common/utilities/FileListLock.h"
#include "glite/ce/cream-client-api-c/string_manipulation.h"
// #include "glite/wmsutils/exception/Exception.h"
// #include "glite/wmsutils/jobid/JobId.h"
// #include "glite/wms/jdl/RequestAdExceptions.h"
// #include "glite/wms/jdl/JobAd.h"
#include "classad_distribution.h"

#include <string>
#include <iostream>
#include <cstdlib>
#include <sys/time.h>
#include <unistd.h>
#include <exception>
#include <fstream>

using namespace glite::wms::common::utilities;
using namespace std;


int getRandom(double);

int main(int argc, char* argv[]) 
{
    /**
     * Usage: putFL_cancel <grid_job_id>
     *
     */
    if ( argc != 3 ) {
        cerr << "Usage: " << argv[0] << " <filelist> <gridjobid>" << endl;
        return -1;
    }

    string request = "[ version = \"1.0.0\"; command = \"jobcancel\"; arguments = [ id = \"" + string(argv[2]) + "\"; lb_sequence_code = \"pippo\" ] ]";

    FileList<string> fl;
        
    cout << "Adding JDL <" 
         << request << "> to filelist..." << endl;
    {
        try{
            fl.open(argv[1]);
        }
        catch(std::exception& ex) {
            cerr << ex.what()<<endl;
            _exit(1);
        }
        FileListMutex mx(fl);
        FileListLock  lock(mx);
        try {
            fl.push_back(request);
        } catch(std::exception& ex) {
            cerr << ex.what()<<endl;
            _exit(1);
        }
    }
}
