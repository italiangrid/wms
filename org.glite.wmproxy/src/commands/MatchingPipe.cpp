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

#include "MatchingPipe.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> /* for unlink, read, write, and close */
#include <stdio.h>  /* for perror */

namespace glite {
namespace wms {
namespace wmproxy {
namespace commands {

/**
 * A PipeWrapper for listmatch results retrieval.
 *
 * @version
 * @date November 18 2004
 * @author Marco Pappalardo
 */
// int pipefd;
// bool ispipeopen;
// std::string pipepath;

/**
 * Constructor.
 */
MatchingPipe::MatchingPipe(std::string& path)
{
   // Remove any previous FIFO.
   unlink(path.c_str());
   pipepath = path;
}

/**
 * Destructor
 */
MatchingPipe::~MatchingPipe()
{
   unlink(pipepath.c_str());
}

/**
 * Opens the pipe.
 * @return a boolean indicating whether the command was successful or not.
 */
bool MatchingPipe::open()
{
   // Create the FIFO.
   ispipeopen = (mkfifo(pipepath.c_str(), 0666) < 0 ? false : true);
   if (ispipeopen) {
      /*
       * Open the FIFO for reading.
       */
      if ((pipefd = ::open(pipepath.c_str(), O_RDONLY)) < 0) {
         ispipeopen = false;
      }
   }
   return ispipeopen;
}

/**
 * Closes the pipe.
 * @return a boolean indicating whether the command was successful or not.
 */
bool MatchingPipe::close()
{
   if (ispipeopen) {
      ispipeopen= !(::close(pipefd));
   }
   return !ispipeopen;
}

/**
 * Read the pipe containt.
 * @return a string representation of return values taken from the pipe.
 */
std::string MatchingPipe::read()
{
   char buf[5120];     // 5KB buffer
   std::string buffer; // the string each loop appends to
   int n=0;            // read size

   while ((n = ::read(pipefd, buf, sizeof(buf))) > 0 ) {
      buffer.append(std::string(buf));
   }
   // classad::ClassAdParser parser;
   // classad::ClassAd* jdlad = parser.ParseClassAd(buffer);
   buf[0]='\0';
   return buffer;
}

}}}}
