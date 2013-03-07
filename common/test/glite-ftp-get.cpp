/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners for details on the
copyright holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

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

