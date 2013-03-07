/* Copyright (c) Members of the EGEE Collaboration. 2004.
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
limitations under the License. */
#include "JobControllerFactory.h"
#include "JobControllerClient.h"
#include "JobControllerClientImpl.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

JobControllerClient::JobControllerClient( void ) : jcc_impl( JobControllerFactory::instance()->create_client() )
{}

JobControllerClient::~JobControllerClient( void )
{ delete this->jcc_impl; }

void JobControllerClient::release_request( void )
{
  this->jcc_impl->release_request();
}

void JobControllerClient::extract_next_request( void )
{
  this->jcc_impl->extract_next_request();
}

const Request *JobControllerClient::get_current_request( void )
{
  return this->jcc_impl->get_current_request();
}

} // Namespace controller

} JOBCONTROL_NAMESPACE_END
