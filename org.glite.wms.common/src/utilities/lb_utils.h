// File: lb_utils.h
// Authors: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
//          Marco Cecchi <marco.cecchi@cnaf.infn.it>
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

#ifndef GLITE_WMS_COMMON_UTILITIES_LB_UTILS_H
#define GLITE_WMS_COMMON_UTILITIES_LB_UTILS_H

#include <boost/tuple/tuple.hpp>

#include <openssl/pem.h>
#include <openssl/x509.h>

#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h"

#include "lb.h"

namespace glite {
namespace wms {
namespace common {
namespace utilities {

unsigned int const five_seconds = 5;

std::string get_proxy_subject(std::string const& x509_proxy);
void sleep_while(unsigned int seconds, boost::function<bool()> condition);
boost::tuple<int, std::string,std::string> get_error_info(Context const& context);
std::string get_lb_message(Context const& context);
std::string format_log_message(std::string const& function_name, Context const& context);
}}}}
#endif
