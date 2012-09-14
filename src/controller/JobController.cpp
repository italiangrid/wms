// File: JobController.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
//         Rosario Peluso <Rosario.Peluso@pd.infn.it>
// Copyright (c) Members of the EGEE Collaboration. 2009. 
// See http://www.eu-egee.org/partners/ for details on the copyright holders.  

// Licensed under the Apache License, Version 2.0 (the "License"); 
// you may not use this file except in compliance with the License. 
// You may obtain a copy of the License at 
//     http://www.apache.org/licenses/LICENSE-2.0 
// Unless required by applicable law or agreed to in writing, software 
// distributed under the License is distributed on an "AS IS" BASIS, 
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
// See the License for the specific language governing permissions and 
// limitations under the License.

// $Id$

#include "JobController.h"
#include "JobControllerImpl.h"
#include "JobControllerFactory.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

JobController::JobController( edg_wll_Context *cont ) : jc_impl( JobControllerFactory::instance()->create_server( cont ) )
{}

JobController::~JobController( void )
{ delete this->jc_impl; }

int JobController::submit( const classad::ClassAd *ad )
{ return this->jc_impl->submit( ad ); }

bool JobController::cancel( const glite::jobid::JobId &id, const char *logfile )
{ return this->jc_impl->cancel( id, logfile ); }

bool JobController::cancel( int condorid, const char *logfile )
{ return this->jc_impl->cancel( condorid, logfile ); }

bool JobController::release(int condorid, char const* logfile)
{ return this->jc_impl->release( condorid, logfile ); }

size_t JobController::queue_size( void )
{ return this->jc_impl->queue_size(); }

} // namespace controller

} JOBCONTROL_NAMESPACE_END

