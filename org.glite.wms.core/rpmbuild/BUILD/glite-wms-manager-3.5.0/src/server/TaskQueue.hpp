// File: TaskQueue.hpp
// Author: Francesco Giacomini
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

// $Id$

#ifndef GLITE_WMS_MANAGER_SERVER_TASKQUEUE_HPP
#define GLITE_WMS_MANAGER_SERVER_TASKQUEUE_HPP

#include <map>
#include <boost/shared_ptr.hpp>
#include "Request.hpp"

namespace glite {
namespace wms {
namespace manager {
namespace server {

typedef boost::shared_ptr<Request> RequestPtr;
typedef boost::shared_ptr<Request const> RequestConstPtr;

// should be JobId, but JobId is not safe
typedef std::map<std::string, RequestPtr> TaskQueue;

}}}} // glite::wms::manager::server

#endif
