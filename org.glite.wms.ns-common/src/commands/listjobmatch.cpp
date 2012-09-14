/*
 * File: listjobmatch.cpp
 * Author: Salvatore Monforte <Salvatore.Monforte@ct.infn.it>
 * Author: Marco Pappalardo <Marco.Pappalardo@ct.infn.it>
 * Copyright (c) 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */


// $Id$ 

#include <classad_distribution.h>
#include <classad.h>
#include <boost/scoped_ptr.hpp>
#include <boost/filesystem/exception.hpp>
#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/CommonConfiguration.h"
// #include "glite/wms/broker/RBSimpleImpl.h"
// #include "glite/wms/broker/RBMaximizeFilesImpl.h"
// #include "glite/wms/broker/RBMinimizeAccessCostImpl.h"
// #include "glite/wms/matchmaking/matchmaker.h"
// #include "glite/wms/matchmaking/utility.h"
// #include "glite/wms/matchmaking/exceptions.h"
#include "Command.h"
#include "logging.h"
#include "const.h"
#include "MatchingPipe_nb.h"
#include "glite/wms/common/utilities/edgstrstream.h"
#include "glite/wmsutils/classads/classad_utils.h"
#include "glite/jdl/ManipulationExceptions.h"
#include "glite/jdl/JobAdManipulation.h"
#include "glite/jdl/JDLAttributes.h"
#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"
#include "glite/wmsutils/jobid/JobIdExceptions.h"

using namespace std;

// namespace broker        = glite::wms::broker;
// namespace brokerinfo    = glite::wms::brokerinfo;
// namespace matchmaking   = glite::wms::matchmaking;
namespace configuration = glite::wms::common::configuration;
namespace logger        = glite::wms::common::logger;
namespace utilities     = glite::wms::common::utilities;
namespace requestad     = glite::jdl;
namespace jobid         = glite::wmsutils::jobid;
namespace utils         = glite::wmsutils::classads;

// typedef brokerinfo::BrokerInfo<brokerinfo::brokerinfoGlueImpl> BrokerInfo;

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace commands {

  /*
bool listjobmatch(Command* cmd)
{
  edglog_fn("CFSI::listjobmatch");
  std::string request_str;
  cmd->getParam("jdl", request_str);
  boost::scoped_ptr<classad::ClassAd> ad;  
  edglog(medium) << "Received listjobmatch request." << std::endl;
  edglog(medium) << request_str << std::endl;

  try 
  {
    ad.reset( utils::parse_classad(request_str) );
  }
  catch( utils::CannotParseClassAd& e )
  {
    edglog(severe) << "Cannot Parse classAd: " << request_str << std::endl;
    return false;	  
  }

  const configuration::CommonConfiguration *commonconf;
  commonconf = configuration::Configuration::instance()->common();
  bool use_cached_info_for_gris = commonconf->use_cache_instead_of_gris();

  broker::ResourceBroker rb( new broker::RBSimpleImpl(use_cached_info_for_gris) );
  bool input_data_exists = false;
  std::vector<std::string> input_data;
  requestad::get_input_data(*ad, input_data, input_data_exists);

  boost::scoped_ptr<BrokerInfo> BI;

  if(input_data_exists) {

       BI.reset( new BrokerInfo);
	// Here we have to check if the rank expression in the request
        // is rank = other.dataAccessCost and change the implementation
        // of the broker (RBMinimizeAccessCost)
        classad::ExprTree* rank_expr = ad->Lookup("rank");
        if( rank_expr ) {

            std::vector<std::string> rankAttributes;
            utils::insertAttributeInVector(&rankAttributes, rank_expr, utils::is_reference_to("other"));
            if( rankAttributes.size() == 1 &&
                rankAttributes.front()=="DataAccessCost" ) {
		// RBMinimizeAccessCostImpl doesn't rank based on the
		// Info Service. So the use_cached_info_for_gris 
		// flag isn't needed.
                rb.changeImplementation( new broker::RBMinimizeAccessCostImpl( BI.get() ) );
            }
	    else {
	 	rb.changeImplementation( new broker::RBMaximizeFilesImpl( BI.get(), use_cached_info_for_gris ) );
            }
  	}
  }

  boost::scoped_ptr<matchmaking::match_table_t> suitableCEs;
  std::string mm_error("");
  bool error = false;
  try {
    suitableCEs.reset(rb.findSuitableCEs(ad.get()));
  } catch (matchmaking::ISNoResultError const& e) {
    mm_error = std::string( 
     "The user is not authorized on any resource currently registered in "
     + e.host());
    error = true;
  } catch (matchmaking::InformationServiceError const& e) {
    mm_error = std::string(e.what());  
    error = true;
  } catch (matchmaking::RankingError const& e) {
    mm_error = std::string(e.what());
    error = true;
  } catch (requestad::CannotGetAttribute const& e) {
    mm_error = std::string("Attribute " + e.parameter() + 
			   " does not exist or has the wrong type (expected unknown)"); 
    error = true;
  } catch (requestad::CannotSetAttribute const& e) {
    mm_error = std::string("Cannot set attribute " + e.parameter());
    error = true;
  } catch (jobid::JobIdException& jide) {
    edglog( error ) << jide.what() << std::endl;
    mm_error = std::string("Invalid value unknown for attribute " + requestad::JDL::JOBID + 
			   " (expecting valid jobid)");
    error = true;			   
  } catch( boost::filesystem::filesystem_error& fse ) {
    edglog( error ) << fse.what() << std::endl;
    mm_error = std::string(fse.what());
    error = true;
  } catch (std::exception& e) {
    //  catching  (matchmaking::MatchMakingError& e)
    //  catching  (matchmaking::ISConnectionError& e)
    //  catching  (matchmaking::ISQueryError& e)
    //  catching  (matchmaking::ISClusterQueryError& e)
    //  catching  (matchmaking::BadCEIdFormatEx& e)
    edglog( error ) << e.what() << std::endl;
    mm_error = std::string(e.what());
    error = true;
  } catch (...) {
    mm_error = std::string("Unknown exception caught.");
    error = true;
  } 

  if (error) {
    cmd->setParam("MatchMakingDone", false);
    cmd->setParam("MatchMakingError", mm_error );
    std::vector<std::string> match_list;
    match_list.push_back(EDG_WL_NSMATCHMAKINGERROR);
    match_list.push_back(mm_error);
    cmd->setParam( "MatchResult", match_list );
    return true;
  }

  if (!suitableCEs->empty() ) {
	  
    matchmaking::match_vector_t suitableCEs_vector(suitableCEs->begin(), suitableCEs->end());
    std::stable_sort(suitableCEs_vector.begin(), suitableCEs_vector.end(), matchmaking::rank_greater_than_comparator());
    std::vector<std::string> match_list;
    for( matchmaking::match_vector_t::const_iterator it = suitableCEs_vector.begin(); it != suitableCEs_vector.end(); it++) {
      
      utilities::oedgstrstream s;
      s << it -> first << " = " << it -> second.getRank();
      match_list.push_back( s.str() );
    }
    cmd->setParam("MatchResult", match_list);
  }  
  
  edglog(medium) << "ListJobMatch done." << std::endl;
  cmd->setParam("MatchMakingDone", true);
  return true;
}

*/

  bool listjobmatchex(Command* cmd) {
    int iserror;
    std::string reason;
    std::string pipepath;
    std::string result;
    const std::string error_attribute("ERROR_ATTRIBUTE");
    const std::string reason_attribute("REASON_ATTRIBUTE");
    boost::scoped_ptr<classad::ClassAd> ad;
    std::vector<std::string> match_strings;
    std::vector<std::string> match_list;


    std::string credentials_file;
    cmd -> getParam("X509UserProxy", credentials_file);
    cmd -> getParam("file", pipepath);
    MatchingPipe_nb p(pipepath);
    try {
      if (p.open()) {
        result = p.read();
        p.close();
      } else {
	edglog(severe) << "Could not open pipe: " << pipepath << std::endl;	
      }
    } catch(std::string& ex) {
      edglog(severe) << "*****************************************************************" << result << std::endl;
      edglog(severe) << "* Exceptions caught while waiting for ListMatch result from WM."   << result << std::endl;
      edglog(severe) << "* " << ex << std::endl;
      edglog(severe) << "*****************************************************************" << result << std::endl;
      // Removing User Proxy
      remove(credentials_file.c_str());
      return false;
    } catch (...) {
      edglog(severe) << "*****************************************************************" << result << std::endl;
      edglog(severe) << "* Exceptions caught while waiting for ListMatch result from WM."   << result << std::endl;
      edglog(severe) << "*****************************************************************" << result << std::endl;
      // Removing User Proxy
      remove(credentials_file.c_str());
      return false;
    }

    try { 
      ad.reset( utils::parse_classad(result) );
    } catch( utils::CannotParseClassAd& e ) { 
      edglog(severe) << "Cannot Parse classAd: " << result << std::endl;
      return false; 
    }

    if (ad.get()->EvaluateAttrInt(error_attribute, iserror) ) {
      edglog( severe ) << "Error in listjobmatchex: Cannot evaluate int (error) attribute in" << std::endl;
      // edglog( severe ) << "Classad: " << utils::asString(*(ad.get())) << std::endl;
      cmd->setParam("MatchMakingDone", false);
      cmd->setParam("MatchMakingError", std::string( "Error in ListJobMatchEx. Reason: cannot evaluate int attribute in matchmaking result.")); 
      return false;
    }

    if (iserror) {
      cmd->setParam("MatchMakingDone", false);
      match_list.push_back(EDG_WL_NSMATCHMAKINGERROR);
      if (ad.get()->EvaluateAttrString(reason_attribute, reason)) {
	cmd->setParam("MatchMakingError", reason);
	match_list.push_back(reason);
      } else {
	cmd->setParam("MatchMakingError", std::string("Unknown reason"));
	match_list.push_back("Unknown reason");
      }
      cmd->setParam( "MatchResult", match_list );
      return false;
    }

    /*
      I received
      [
	error = 0;
	reason = "No Matching Resources found.";
	match_result = { }
      ] 
      or
      [
	error = 0;
	match_result = {
          [ 
	    host = "ce1.mydomain.com";
	    rank = numrank
	  ],
	  ....
	}
      ]
    */

    if (!utils::EvaluateAttrListOrSingle(*(ad.get()),"match_result", match_strings)) { 
      cmd -> setParam("MatchMakingDone", true); 
      cmd -> setParam("MatchMakingError", std::string("No Matching Resources found."));
      edglog(critical) << "No Matching Resources found." << std::endl; 
      return true; 
    } 
    
    edglog(debug) << "ListMatch result by WM:" << std::endl;
    for( std::vector<std::string>::const_iterator it = match_strings.begin(); it != match_strings.end(); it++) {
      std::string hostandrank( *it );
      size_t comma_pos = hostandrank.rfind(",");
      std::string host_str( hostandrank.substr(0, comma_pos) );
      std::string rank_str( hostandrank.substr(comma_pos + 1, string::npos) );
      utilities::oedgstrstream s;
      s << host_str << " = " << rank_str;
      match_list.push_back( s.str() );
      edglog(debug) << "/tMatch: " << s.str() << std::endl;
    } 

    cmd -> setParam("MatchMakingDone", true);       
    cmd->setParam( "MatchResult", match_list );
    return true;
  }

} // namespace commands
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite
  
