// File: wm_commands.cpp
// Author: Francesco Giacomini

// $Id$

#include "glite/wms/common/utilities/wm_commands.h"
#include "glite/wms/common/utilities/classad_utils.h"
#include <algorithm>
#include <cctype>
#include <boost/scoped_ptr.hpp>

namespace glite {
namespace wms {
namespace common {
namespace utilities {

namespace {
std::string const command_requirements(
  "[requirements="
  "  other.version==\"1.0.0\""
  "  && isString(other.command)"
  "  && isClassad(other.arguments)"
  "  && (other.command == \"jobsubmit\""
  "      && isClassad(other.arguments.ad)"
  "      && isString(other.arguments.ad.edg_jobid)"
  "      && isString(other.arguments.ad.lb_sequence_code)"
  "      && isString(other.arguments.ad.X509UserProxy)"
  "      || other.command == \"jobresubmit\""
  "      && isString(other.arguments.id)"
  "      && isString(other.arguments.lb_sequence_code)"
  "      || other.command == \"jobcancel\""
  "      && isString(other.arguments.id)"
  "      && isString(other.arguments.lb_sequence_code)"
  "      || other.command == \"match\""
  "      && isClassad(other.arguments.ad)"
  "      && isString(other.arguments.ad.CertificateSubject)"
  "      && isString(other.arguments.file)"
  "      && (isUndefined(other.arguments.number_of_results)"
  "          || isInteger(other.arguments.number_of_results))"
  "      && (isUndefined(other.arguments.include_brokerinfo)"
  "          || isBoolean(other.arguments.include_brokerinfo))"
  "     )"
  "]"
);

} // {anonymous}

bool
command_is_valid(classad::ClassAd const& command_ad)
{
  static classad::ClassAd const* command_ad_requirements(
    parse_classad(command_requirements)
  );
  return left_matches_right(command_ad, *command_ad_requirements);
}

std::string
command_get_command(classad::ClassAd const& command_ad)
{
  std::string result = evaluate_attribute(command_ad, "command");

  std::transform(result.begin(), result.end(), result.begin(), ::tolower);

  return result;
}

classad::ClassAd
submit_command_create(classad::ClassAd const& job_ad)
{
  classad::ClassAd result;

  result.InsertAttr("version", std::string("1.0.0"));
  result.InsertAttr("command", std::string("jobsubmit"));
  std::auto_ptr<classad::ClassAd> args(new classad::ClassAd);
  std::auto_ptr<classad::ClassAd> job_ad_copy(new classad::ClassAd(job_ad));
  args->Insert("ad", job_ad_copy.release());
  result.Insert("arguments", args.release());

  return result;
}

classad::ClassAd const*
submit_command_get_ad(classad::ClassAd const& submit_command_ad)
{
  return evaluate_expression(submit_command_ad, "arguments.ad");
}

classad::ClassAd
resubmit_command_create(std::string const& job_id, std::string const& sequence_code)
{
  classad::ClassAd result;

  result.InsertAttr("version", std::string("1.0.0"));
  result.InsertAttr("command", std::string("jobresubmit"));
  std::auto_ptr<classad::ClassAd> args(new classad::ClassAd);
  args->InsertAttr("id", job_id);
  args->InsertAttr("lb_sequence_code", sequence_code);
  result.Insert("arguments", args.release());

  return result;
}



std::string
resubmit_command_get_id(classad::ClassAd const& command_ad)
{
  return evaluate_expression(command_ad, "arguments.id");
}

std::string
resubmit_command_get_lb_sequence_code(classad::ClassAd const& command_ad)
{
  return evaluate_expression(command_ad, "arguments.lb_sequence_code");
}

classad::ClassAd
cancel_command_create(std::string const& job_id)
{
  classad::ClassAd result;

  result.InsertAttr("version", std::string("1.0.0"));
  result.InsertAttr("command", std::string("jobcancel"));
  std::auto_ptr<classad::ClassAd> args(new classad::ClassAd);
  args->InsertAttr("id", job_id);
  result.Insert("arguments", args.release());

  return result;
}

std::string
cancel_command_get_id(classad::ClassAd const& cancel_command_ad)
{
  return evaluate_expression(cancel_command_ad, "arguments.id");
}

std::string
cancel_command_get_lb_sequence_code(classad::ClassAd const& command_ad)
{
  return evaluate_expression(command_ad, "arguments.lb_sequence_code");
}

classad::ClassAd
match_command_create(
  classad::ClassAd const& jdl,
  std::string const& file,
  int number_of_results,
  bool include_brokerinfo
)
{
  classad::ClassAd result;

  result.InsertAttr("version", std::string("1.0.0"));
  result.InsertAttr("command", std::string("match"));
  std::auto_ptr<classad::ClassAd> args(new classad::ClassAd);
  std::auto_ptr<classad::ClassAd> jdl_copy(new classad::ClassAd(jdl));
  args->Insert("ad", jdl_copy.release());
  args->InsertAttr("file", file);
  args->InsertAttr("number_of_results", number_of_results);
  args->InsertAttr("include_brokerinfo", include_brokerinfo);
  result.Insert("arguments", args.release());

  return result;
}

classad::ClassAd const*
match_command_get_ad(classad::ClassAd const& match_command_ad)
{
  return evaluate_expression(match_command_ad, "arguments.ad");
}

std::string
match_command_get_file(classad::ClassAd const& match_command_ad)
{
  return evaluate_expression(match_command_ad, "arguments.file");
}

int match_command_get_number_of_results(classad::ClassAd const& match_command_ad)
try {
  return evaluate_expression(match_command_ad, "arguments.number_of_results");
} catch (InvalidValue&) {
  return -1;
}

bool match_command_get_include_brokerinfo(classad::ClassAd const& match_command_ad)
try {
  return evaluate_expression(match_command_ad, "arguments.include_brokerinfo");
} catch (InvalidValue&) {
  return false;
}

}}}} // glite::wms::manager::common
