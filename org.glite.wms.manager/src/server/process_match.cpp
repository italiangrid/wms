#include <boost/lexical_cast.hpp>
#include <boost/bind.hpp>
#include <classad_distribution.h>
#include "glite/wms/common/utilities/scope_guard.h"
#include "glite/wms/broker/match.h"
#include "glite/jdl/JDLAttributes.h"
#include "glite/wmsutils/classads/classad_utils.h"

namespace utilities = glite::wms::common::utilities;
namespace jdl = glite::jdl;
namespace cu = glite::wmsutils::classads;

namespace glite {
namespace wms {

namespace broker {
void match(classad::ClassAd& ad, MatchTable& matches);
void match(classad::ClassAd& ad, MatchTable& matches, DataInfo& data_info);
classad::ClassAd*
create_brokerinfo(
  classad::ClassAd const& jdl_ad, 
  classad::ClassAd const& ce_ad, 
  DataInfo const& data_info
);

}

namespace manager {
namespace server {

namespace {

bool
rank_greater_than(
  broker::MatchTable::value_type const& lhs,
  broker::MatchTable::value_type const& rhs
);

bool write_to(std::string const& data, std::string const& file);

std::string
format_match_result(
  broker::MatchTable const& matches,
  int number_of_results
);
std::string
format_match_result(
  classad::ClassAd& ad,
  broker::MatchTable const& matches,
  broker::DataInfo const& data_info,
  int number_of_results
);

}

bool
match(
  classad::ClassAd& ad,
  std::string const& result_file,
  int number_of_results,
  bool include_brokerinfo
)
{
  if (result_file.empty() || result_file[0] != '/') {
    return false;
  }

  bool const input_data_exists = ad.Lookup(jdl::JDL::INPUTDATA);
  bool const data_requirements_exists = ad.Lookup(jdl::JDL::DATA_REQUIREMENTS);
  bool full_list_ = false;
  bool const full_list(
    ad.EvaluateAttrBool("FullListMatchResult", full_list_) && !full_list_
  );

  broker::MatchTable matches;
  broker::DataInfo data_info;

  if ((input_data_exists || data_requirements_exists) && !full_list) {
    match(ad, matches, data_info);
  } else {
    match(ad, matches);
  }

  std::stable_sort(matches.begin(), matches.end(), rank_greater_than);

  std::string result;
  if (include_brokerinfo) {
    result = format_match_result(ad, matches, data_info, number_of_results);
  } else {
    result = format_match_result(matches, number_of_results);
  }

  return write_to(result, result_file);
}

namespace {

bool write_to(std::string const& data, std::string const& file)
{
  int const fd = open(file.c_str(), O_WRONLY | O_NONBLOCK);
  if (fd == -1) {
    return false;
  }
  utilities::scope_guard close_fd(boost::bind(::close, fd));

  int const max_retries = 7;
  int retry_count = 0;
  int backoff_time = 1;
  int const write_size = data.size();
  int written_size = 0;
  char const* const c_data = data.c_str();

  while (written_size != write_size) {
    int n_write(
      write(fd, c_data + written_size, write_size - written_size)
    );

    if (n_write > 0 && n_write <= (write_size - written_size)) {
      written_size += n_write;
      backoff_time = 1;
      retry_count = 0;
    } else if (n_write == 0) {
      sleep(backoff_time);
      backoff_time *= 2;
      ++retry_count;
    } else {
      if (errno == EAGAIN && retry_count < max_retries) {
        sleep(backoff_time);
        backoff_time *= 2;
        ++retry_count;
      } else {
        break;
      }
    }
  }

  return written_size == write_size;
}

bool
rank_greater_than(
  broker::MatchTable::value_type const& lhs,
  broker::MatchTable::value_type const& rhs
)
{
  return lhs.rank > rhs.rank;
}

std::string
format_match_result(
  classad::ClassAd& ad,
  broker::MatchTable const& matches,
  int number_of_results
)
{
  classad::ClassAd formatted_result;
  formatted_result.InsertAttr("reason", std::string("ok"));

  std::vector<classad::ExprTree*> hosts_expr;
  broker::MatchTable::const_iterator it = matches.begin();
  broker::MatchTable::const_iterator const end = matches.end();
  
  for (
    int i = 0;
    it != end && (number_of_results == -1 || i < number_of_results);
    ++it, ++i
  ) {
    std::string const ce_id(
      cu::evaluate_attribute(*it->ce_ad, "GlueCEUniqueID")
    );
    classad::Value v;
    v.SetStringValue(ce_id + ',' + boost::lexical_cast<std::string>(it->rank));
    std::auto_ptr<classad::ExprTree> l(classad::Literal::MakeLiteral(v));
    hosts_expr.push_back(l.get());
    l.release();
  }

  std::auto_ptr<classad::ExprTree> e(
    classad::ExprList::MakeExprList(hosts_expr)
  );
  formatted_result.Insert("match_result", e.get());
  e.release();

  classad::PrettyPrint unparser;
  std::string result;
  unparser.Unparse(result, &formatted_result);

  return result;
}

std::string
format_match_result(
  classad::ClassAd& ad,
  broker::MatchTable const& matches,
  broker::DataInfo const& data_info,
  int number_of_results
)
{
  classad::ClassAd formatted_result;
  formatted_result.InsertAttr("reason", std::string("ok"));

  std::vector<classad::ExprTree*> hosts;
  broker::MatchTable::const_iterator it = matches.begin();
  broker::MatchTable::const_iterator const end = matches.end();
  
  for (
    int i = 0;
    it != end && (number_of_results == -1 || i < number_of_results);
    ++it, ++i
  ) {

    std::auto_ptr<classad::ClassAd> host(new classad::ClassAd);
    std::string const ce_id(
      cu::evaluate_attribute(*it->ce_ad, "GlueCEUniqueID")
    );
    host->InsertAttr("ce_id", ce_id);
    host->InsertAttr("rank", it->rank);
    std::auto_ptr<classad::ClassAd> brokerinfo(
      create_brokerinfo(ad, *it->ce_ad, data_info)
    );
    host->Insert("brokerinfo", brokerinfo.get());
    brokerinfo.release();
    hosts.push_back(host.get());
    host.release();
  }

  formatted_result.Insert(
    "match_result",
    classad::ExprList::MakeExprList(hosts)
  );

  classad::PrettyPrint unparser;
  std::string result;
  unparser.Unparse(result, &formatted_result);

  return result;
}

}

}}}} // glite::wms::manager::server
