// File: filelist_recovery.h
// Author: Francesco Giacomini
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

// $Id$

#ifndef GLITE_WMS_MANAGER_SERVER_FILELIST_RECOVERY_H
#define GLITE_WMS_MANAGER_SERVER_FILELIST_RECOVERY_H

#include "filelist_utils.h"
#include "TaskQueue.hpp"

namespace glite {
namespace wms {
namespace manager {
namespace server {

void recovery(ExtractorPtr extractor, TaskQueue& tq);

}}}} // glite::wms::manager::server

#endif
