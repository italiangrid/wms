/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the copyright
holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "MatchingPipe_nb.h"
#include <boost/lexical_cast.hpp>
#include <exception>
#include <sys/time.h>

#include "utilities/logging.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"

#include "utilities/wmpexception_codes.h"
#include "utilities/wmpexceptions.h"

#include "server/configuration.h"

extern WMProxyConfiguration conf;

namespace glite {
namespace wms {
namespace wmproxy {
namespace commands {

namespace wmputilities  = glite::wms::wmproxy::utilities;

/**
 * A Non-blocking PipeWrapper for listmatch results retrieval.
 *
 * @version
 * @date November 18 2004
 * @author Marco Pappalardo
 */

/**
 * Constructor.
 */
//  MatchingPipe_nb::MatchingPipe_nb(std::string &path) {
// Remove any previous FIFO.
//unlink(path.c_str());
//pipepath = path;
//}

/**
 * Opens the pipe.
 * @return a boolean indicating whether the command was successful or not.
 */
bool MatchingPipe_nb::open()
{
   // Create the FIFO.
   ispipeopen = (mkfifo(pipepath.c_str(), 0666) < 0 ? false : true);
   if (ispipeopen) {
      /*
       * Open the FIFO for reading.
       */
      if ((pipefd = ::open(pipepath.c_str(), O_RDONLY | O_NONBLOCK )) < 0) {
         ispipeopen = false;
      }
   }
   return ispipeopen;
}

/**
 * Read the pipe containt. Non blocking.
 * @return a string representation of return values taken from the pipe.
 */
std::string MatchingPipe_nb::read()
{

   struct timeval limitTime ;

   // timeout value read from configuration
   int timeout = conf.getListMatchTimeout() ;
   std::string buffer; // the string each loop appends to
   bool done = false;

   // request time
   gettimeofday( &limitTime, 0 ) ;
   // total timeout
   limitTime.tv_sec += timeout ;

   while (!done) {

      struct timeval timeoutVal ;
      struct timeval currentTime ;

      // wait for input to be available
      fd_set read_fds;
      FD_ZERO(&read_fds);
      FD_SET(pipefd, &read_fds);

      // current time
      gettimeofday( &currentTime, 0 );
      // relative timeout updated
      timersub ( &limitTime, &currentTime, &timeoutVal ) ;

      int result ;
      while ( (result = select(pipefd + 1, &read_fds, 0, 0, &timeoutVal)) < 0) {
         if (errno == EINTR) {
            continue;
         } else {
            throw std::string( "select failed with errno " +
                               boost::lexical_cast<std::string>(errno));
         }
      }
      if ( result == 0 ) {
         edglog(critical) << "Method read(): " << "Timeout reached, command execution will be terminated now" << std::endl ;
         throw wmputilities::JobTimedoutException (__FILE__, __LINE__, "jobListMatch()", wmputilities::WMS_OPERATION_TIMEDOUT,  "Timeout reached, command execution will be terminated now");
      }
      // read available input
      char buf[5120];
      int nread;
      while ((nread = ::read(pipefd, buf, 5120)) < 0) {
         if (errno == EINTR) {
            continue;
         } else {
            throw std::string( "select failed with errno " +
                               boost::lexical_cast<std::string>(errno));
         }
      }
      if (nread == 0) {           // EOF
         done = true;
      } else {
         buffer.append(buf, buf + nread);
      }
   }
   // buf[0]='\0';
   return buffer;
}

}}}}
