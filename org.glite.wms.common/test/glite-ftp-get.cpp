#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <netdb.h>

#include <iostream>

#include <string>
#include "globus_common.h"
#include "globus_io.h"
#include "globus_ftp_utils.h"

#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"

using namespace std;

namespace utilities = glite::wms::common::utilities;
namespace logger    = glite::wms::common::logger;

int main(int argc, char **argv)
{
    globus_module_activate(GLOBUS_FTP_CLIENT_MODULE);
    logger::threadsafe::edglog.open( std::cout, logger::info );
    if (argc != 3 || !argv[1] || !argv[2])
    {
        cout << "Usage: glite-ftp-get <source-url> <local-file>\n";
        return 1;
    }
    bool result = utilities::globus::get(string("gsiftp://") + string(argv[1]), string(argv[2]));
    globus_module_deactivate_all();
    return !result; 
}

