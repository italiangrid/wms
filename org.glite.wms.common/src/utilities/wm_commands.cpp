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
// File: wm_commands.cpp
// Author: Francesco Giacomini

// $Id$

#include "glite/wms/common/utilities/wm_commands.h"
#include "glite/wmsutils/classads/classad_utils.h"
#include <algorithm>
#include <cctype>

using namespace glite::wmsutils::classads;

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
submit_command_create(std::auto_ptr<classad::ClassAd> jdl)
{
  classad::ClassAd result;

  result.InsertAttr("version", std::string("1.0.0"));
  result.InsertAttr("command", std::string("jobsubmit"));
  std::auto_ptr<classad::ClassAd> args(new classad::ClassAd);
  args->Insert("ad", jdl.get());
  jdl.release();
  result.Insert("arguments", args.get());
  args.release();

  return result;  
}

classad::ClassAd
submit_command_create(classad::ClassAd const& jdl)
{
  std::auto_ptr<classad::ClassAd> jdl_(new classad::ClassAd(jdl));
  return submit_command_create(jdl_);
}

classad::ClassAd const*
submit_command_get_ad(classad::ClassAd const& submit_command_ad)
{
  return evaluate_expression(submit_command_ad, "arguments.ad");
}

std::auto_ptr<classad::ClassAd>
submit_command_remove_ad(classad::ClassAd& submit_command_ad)
{
  std::auto_ptr<classad::ClassAd> result(
    static_cast<classad::ClassAd*>(
      static_cast<classad::ClassAd*>(
        submit_command_ad.Lookup("arguments")
      )->Remove("ad")
    )
  );

  return result;
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
  result.Insert("arguments", args.get());
  args.release();

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
  result.Insert("arguments", args.get());
  args.release();

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
  std::auto_ptr<classad::ClassAd> jdl,
  std::string const& file,
  int number_of_results,
  bool include_brokerinfo
)
{
  classad::ClassAd result;

  result.InsertAttr("version", std::string("1.0.0"));
  result.InsertAttr("command", std::string("match"));
  std::auto_ptr<classad::ClassAd> args(new classad::ClassAd);
  args->Insert("ad", jdl.get());
  jdl.release();
  args->InsertAttr("file", file);
  args->InsertAttr("number_of_results", number_of_results);
  args->InsertAttr("include_brokerinfo", include_brokerinfo);
  result.Insert("arguments", args.get());
  args.release();

  return result;
}

classad::ClassAd
match_command_create(
  classad::ClassAd const& jdl,
  std::string const& file,
  int number_of_results,
  bool include_brokerinfo
)
{
  std::auto_ptr<classad::ClassAd> jdl_(new classad::ClassAd(jdl));
  return
    match_command_create(jdl_, file, number_of_results, include_brokerinfo);
}

classad::ClassAd const*
match_command_get_ad(classad::ClassAd const& match_command_ad)
{
  return evaluate_expression(match_command_ad, "arguments.ad");
}

std::auto_ptr<classad::ClassAd>
match_command_remove_ad(classad::ClassAd& match_command_ad)
{
  std::auto_ptr<classad::ClassAd> result(
    static_cast<classad::ClassAd*>(
      static_cast<classad::ClassAd*>(
        match_command_ad.Lookup("arguments")
      )->Remove("ad")
    )
  );

  return result;
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
