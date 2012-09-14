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

#ifndef GLITE_WMS_WMPROXY_COMMANDS_MATCHINGPIPE_H
#define GLITE_WMS_WMPROXY_COMMANDS_MATCHINGPIPE_H

#include <classad_distribution.h>
#include <string>
#include <vector>
#include <boost/scoped_ptr.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> /* for unlink, read, write, and close */
#include <stdio.h>  /* for perror */


namespace classad
{
class ClassAd;
};

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
class MatchingPipe
{
public:

   int pipefd;
   bool ispipeopen;
   std::string pipepath;

public:
   /**
    * Constructor.
    */
   MatchingPipe(std::string& path);

   /**
    * Destructor
    */
   virtual ~MatchingPipe();

   /**
    * Opens the pipe.
    * @return a boolean indicating whether the command was successful or not.
    */
   bool open();

   /**
    * Closes the pipe.
    * @return a boolean indicating whether the command was successful or not.
    */
   bool close();

   /**
    * Read the pipe containt.
    * @return a string representation of return values taken from the pipe.
    */
   std::string read();
};

}}}}

#endif
