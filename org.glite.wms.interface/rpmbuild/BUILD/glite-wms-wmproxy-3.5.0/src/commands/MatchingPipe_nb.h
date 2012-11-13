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

#ifndef _GLITE_WMS_WMPROXY_COMMANDS_MATCHINGPIPE_NB_H_
#define _GLITE_WMS_WMPROXY_COMMANDS_MATCHINGPIPE_NB_H_

#include "MatchingPipe.h"

namespace glite {
namespace wms {
namespace wmproxy {
namespace commands {

/**
 * A Non blocking PipeWrapper for listmatch results retrieval.
 *
 * @version
 * @date April 28 2005
 * @author Marco Pappalardo
 */
class MatchingPipe_nb : public commands::MatchingPipe
{

public:
   /**
    * Constructor.
    */
   MatchingPipe_nb(std::string& path) : commands::MatchingPipe( path ) {}

   /**
    * Opens the pipe for non blocking read.
    * @return a boolean indicating whether the command was successful or not.
    */
   bool open();

   /**
    * Read the pipe containt. Non blocking.
    * @return a string representation of return values taken from the pipe.
    */
   std::string read();
};

}}}}

#endif
