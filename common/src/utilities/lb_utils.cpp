// File: lb_utils.h
// Authors: Francesco Giacomini <Francesco.Giacomini@cnaf.infn.it>
//          Marco Cecchi <marco.cecchi@cnaf.infn.it>
// Copyright (c) Members of the EGEE Collaboration 2004
// For license conditions see http://public.eu-egee.org/license/license.html

#include <lb_utils.h>

namespace glite {
namespace wms {
namespace common {
namespace utilities {

void sleep_while(
  unsigned int seconds,
  boost::function<bool()> termination_condition
)
{
  for (unsigned int i = 0; i < seconds && !termination_condition(); ++i) {
    ::sleep(1);
  }
}

std::string
get_proxy_subject(std::string const& x509_proxy)
{
  static std::string const null_string;

  std::FILE* fd = std::fopen(x509_proxy.c_str(), "r");
  if (!fd) return null_string;
  boost::shared_ptr<std::FILE> fd_(fd, std::fclose);

  ::X509* const cert = ::PEM_read_X509(fd, 0, 0, 0);
  if (!cert) return null_string;
  boost::shared_ptr< ::X509> cert_(cert, ::X509_free);

  char* const s = ::X509_NAME_oneline(::X509_get_subject_name(cert), 0, 0);
  if (!s) return null_string;
  boost::shared_ptr<char> s_(s, ::free);

  return std::string(s);
}

boost::tuple<int,std::string, std::string>
get_error_info(Context const& context)
{
  int error;
  std::string error_txt;
  std::string description_txt;

  char* c_error_txt = 0;
  char* c_description_txt = 0;
  error = edg_wll_Error(context.c_context(), &c_error_txt, &c_description_txt);

  if (c_error_txt) {
    error_txt = c_error_txt;
  }
  free(c_error_txt);
  if (c_description_txt) {
    description_txt = c_description_txt;
  }
  free(c_description_txt);

  return boost::make_tuple(error, error_txt, description_txt);
}

std::string
get_lb_message(Context const& context)
{
  std::string result;

  int error;
  std::string error_txt;
  std::string description_txt;
  boost::tie(error, error_txt, description_txt) = get_error_info(context);

  result += error_txt;
  result += " (";
  result += boost::lexical_cast<std::string>(error);
  result += ") - ";
  result += description_txt;

  return result;
}

std::string
format_log_message(std::string const& function_name, Context const& context)
{
  std::string result(function_name);
  result += " failed for ";

  edg_wlc_JobId c_jobid;
  int e = edg_wll_GetLoggingJob(context.c_context(), &c_jobid);
  assert(e == 0);
  wmsutils::jobid::JobId jobid(c_jobid);
  edg_wlc_JobIdFree(c_jobid);

  result += jobid.toString() + " (" + get_lb_message(context) + ')';

  return result;
}

}}}}
