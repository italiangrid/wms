// File: JobController.cpp
// Author: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
//         Rosario Peluso <Rosario.Peluso@pd.infn.it>
// Copyright (c) 2001 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

#include "JobController.h"
#include "JobControllerImpl.h"
#include "JobControllerFactory.h"

JOBCONTROL_NAMESPACE_BEGIN {

namespace controller {

JobController::JobController(boost::shared_ptr<jccommon::EventLogger> ctx)
 : jc_impl(JobControllerFactory::instance()->create_server(ctx))
{ }

JobController::~JobController()
{ delete this->jc_impl; }

int JobController::msubmit(std::vector<classad::ClassAd*> v)
{ return this->jc_impl->msubmit(v); }

int JobController::submit(classad::ClassAd *ad)
{ return this->jc_impl->submit(ad); }

bool JobController::cancel( const glite::wmsutils::jobid::JobId &id, const char *logfile )
{ return this->jc_impl->cancel( id, logfile ); }

bool JobController::cancel( int condorid, const char *logfile )
{ return this->jc_impl->cancel( condorid, logfile ); }

}; // namespace controller
} JOBCONTROL_NAMESPACE_END;

