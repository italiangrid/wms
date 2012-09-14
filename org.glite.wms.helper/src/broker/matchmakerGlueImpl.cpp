/*
Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the
copyright holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

// File: matchmakerGlueImpl.cpp
// Author: Salvatore Monforte <Salvatore.Monforte@ct.infn.it>
// Copyright (c) 2002 EU DataGrid.

// $Id$

#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

#include <glite/wms/common/ldif2classad/LDAPQuery.h>
#include <glite/wms/common/ldif2classad/LDAPSynchConnection.h>
#include <glite/wms/common/ldif2classad/LDAPForwardIterator.h>
#include <glite/wms/common/ldif2classad/exceptions.h>

#include "glite/wmsutils/exception/Exception.h"
#include "glite/wms/common/utilities/classad_utils.h"
#include "glite/wms/common/utilities/ii_attr_utils.h"

#include "glite/wms/common/configuration/Configuration.h"
#include "glite/wms/common/configuration/NSConfiguration.h"

#include "glite/wms/common/logger/edglog.h"
#include "glite/wms/common/logger/manipulators.h"

#include "matchmakerGlueImpl.h"
#include "glue_attributes.h"
#include "jdl_attributes.h"
#include "exceptions.h"

#include "glite/wms/classad_plugin/classad_plugin_loader.h"

#include "classad_distribution.h"

#define edglog(level) logger::threadsafe::edglog << logger::setlevel(logger::level)
#define edglog_fn(name) logger::StatePusher    pusher(logger::threadsafe::edglog, #name);

using namespace std;
namespace excep = glite::wmsutils::exception;

namespace glite {
namespace wms {
	
namespace configuration = common::configuration;
namespace ldif2classad  = common::ldif2classad;
namespace utilities     = common::utilities;
namespace logger        = common::logger;

namespace matchmaking { 
namespace 
{

  struct insertMatchingRegEx : binary_function<vector<string>*, string, vector<string>*>
  {
    
    insertMatchingRegEx(const string& p) 
    {
      expression.reset( new boost::regex(p) );
    }
    
    vector<string>* operator()(vector<string>* v, string a)
    {
      try {
	
	if( boost::regex_match(a, *expression) ) {
	  
	  v -> push_back(a);
	}
      }
      catch( boost::bad_expression& e ){

	edglog(fatal) << "Kaboomm" << endl;
      }    
      return v;
    }
    boost::shared_ptr<boost::regex> expression;
  };

  struct BadCEIdFormatEx
  {
  };

  string getClusterName (ldif2classad::LDIFObject &ldif_CE)
  {
    std::string cluster;
    std::vector<std::string> foreignKeys;
    std::string reg_string;
    ldif_CE.EvaluateAttribute(GS_GLUECE_HOSTINGCLUSTER, cluster);

    try {
      reg_string.assign(GS_GLUECE_FKEY_CLUSTERUNIQUEID);
      reg_string.append("\\s*=\\s*([^\\s]+)");

      ldif_CE.EvaluateAttribute(GS_GLUECE_FOREIGNKEY, foreignKeys);

      static boost::regex get_cluid( reg_string );
      boost::smatch result_cluid;
      bool found = false;

      // Looking for one GlueForeignKey (it is possibly multi-valued)
      // With the specified attribute.
      for( std::vector<std::string>::const_iterator key = foreignKeys.begin();
	   key != foreignKeys.end(); key++) {
	      
        if( boost::regex_match(*key, result_cluid, get_cluid) ) {
	  cluster.assign(result_cluid[1].first,result_cluid[1].second);
	  found = true;
	  break;
        } 

      }

      if (!found) {
        edglog( warning ) << "Cannot find " << 
	    GS_GLUECE_FKEY_CLUSTERUNIQUEID << 
	    " assignment. Using " << cluster << "." << endl;
      }

    } catch( ldif2classad::LDAPNoEntryEx& ) {
      // The GlueForeignKey was not found. 
      // We keep the gatekeeper name.
    } catch( boost::bad_expression& e ){
      edglog( error ) << "Bad regular expression " << reg_string <<
	   ". Cannot parse "<< GS_GLUECE_FOREIGNKEY << ". Using " << 
	   cluster << "." << endl;
    }
    return cluster;
  }

  bool mergeClustersInfo(const string& cluster, const vector<string>& clsAttributes, 
			 ldif2classad::LDAPConnection& IIconnection, ldif2classad::LDIFObject& ldif_CE)
  {
    string cluster_filter;
    cluster_filter.assign("(&(objectclass=GlueSubCluster)(GlueSubClusterUniqueID=" + cluster + "))");
    
    edglog( debug ) << "Checking clusters: " << cluster << ", " << cluster_filter << endl;
    
    ldif2classad::LDAPQuery cluster_query(&IIconnection, cluster_filter, clsAttributes);
    
    cluster_query.execute();
    if(!cluster_query.tuples()->empty()) {
      
      ldif2classad::LDAPForwardIterator ldap_it2(cluster_query.tuples());
      ldap_it2.first();
      if(ldap_it2.current()) {
	
	ldif_CE.merge( *ldap_it2 );
	return true;
      }
      else edglog( error )  << "subcluster undefined!" << endl <<flush;
    }
    return false;
  }

boost::scoped_ptr< classad::ClassAd > matchmakerGlueImpl::gang_match_storageAd;
	
} //anonymous namespace

matchmakerGlueImpl::matchmakerGlueImpl()
{
  m_CE_info_prefetched = false;
}

matchmakerGlueImpl::~matchmakerGlueImpl()
{
}

/**
 * Prefetch CE info
 * This method obtains the CE info objectclasses for the suitableCEs
 * for later use by both checkRequirement and checkRank
 * @param requestAd
 * @param suitableCEs
 */
void matchmakerGlueImpl::prefetchCEInfo(const classad::ClassAd* requestAd, match_table_t& suitableCEs)
{
  edglog_fn(prefetchCEInfo);     	

  m_CE_info_prefetched = false;

  const configuration::NSConfiguration* NSconf = configuration::Configuration::instance() -> ns();
  
  
  boost::scoped_ptr<ldif2classad::LDAPConnection> IIconnection;
  
  IIconnection.reset( new ldif2classad::LDAPSynchConnection( NSconf -> ii_dn(),
							     NSconf -> ii_contact(),
							     NSconf -> ii_port(),
							     NSconf -> ii_timeout()) );

  vector<string> reqAttributes;
  classad::ExprTree* requirements_expr = requestAd -> Lookup("requirements");
  utilities::insertAttributeInVector(&reqAttributes, requirements_expr,
				     utilities::is_reference_to("other"));

  classad::ExprTree* rank_expr = requestAd -> Lookup("rank");
  utilities::insertAttributeInVector(&reqAttributes, rank_expr,
				     utilities::is_reference_to("other"));
  
  reqAttributes.push_back(GS_GLUECE_ACCESSCONTROL_BASERULE);
  reqAttributes.push_back(GS_GLUECE_UNIQUEID);
  reqAttributes.push_back(GS_GLUECE_INFO_LRMS_TYPE);
  reqAttributes.push_back(GS_GLUECE_HOSTINGCLUSTER);
  reqAttributes.push_back(GS_GLUECE_FOREIGNKEY);
  reqAttributes.push_back(GS_GLUECE_STATE_ESTIMATEDRESPONSETIME);
  reqAttributes.push_back("GlueInformationServiceURL");
  
  // checks if the query for gaining clusters information is needed or not	
  vector<string> clsAttributes;
  std::accumulate(reqAttributes.begin(), reqAttributes.end(), 
		  &clsAttributes, insertMatchingRegEx("^Glue[(Host)|(Sub)].*"));
  
  // if context.suitableCEs is not empty the match-making algorithms
  // should use these CEs to match requirements for...
  // moreover it should be noted that in this case there is no need 
  // to check if the user is authorised at a give CE...since is has    
  // been alreadry checked during the data matching phase, which
  // populated the suitableCEs  
  
  bool start_from_given_CEs_set  = !suitableCEs.empty();
  
  string filter;

  if(  start_from_given_CEs_set ) {
    
    //
    // construct the or part of the LDAP filter so that 
    // only the CEs in suitableCEs will be contacted...
    //
    string ORprefix;
    string ORexpr;
    match_table_t::const_iterator or_it = suitableCEs.begin();
    
    ORexpr = "(" + string(GS_GLUECE_UNIQUEID) + "=" + (*or_it).first + ")";
    
    while( ++or_it != suitableCEs.end() ) {
      
      ORprefix.append("(|");
      ORexpr.append("(" +  string(GS_GLUECE_UNIQUEID) + "=" + (*or_it).first + "))");
    }
    filter = "(&(objectclass=GlueCE)";
    if( suitableCEs.size() > 1) filter += ORprefix;     
    filter += ORexpr + ")";
  }
  else {
    
    string ucs( utilities::evaluate_attribute(*requestAd,"CertificateSubject") );
    string vo ( utilities::evaluate_attribute(*requestAd, "VirtualOrganisation") );
    filter = "(&(objectclass=GLUECE)(|(" + string(GS_GLUECE_ACCESSCONTROL_BASERULE) + "=VO:" + vo + ")(" +
      string(GS_GLUECE_ACCESSCONTROL_BASERULE) + "=" + ucs + ")))";
  }
  
  ldif2classad::LDAPQuery query(IIconnection.get(), filter, reqAttributes);
  
  edglog( debug ) << "Filtering Information Index (CE info prefetch): " << filter << endl;
  
  try { 

    if ( !m_CE_info_cache.empty() ) m_CE_info_cache.resize(0);

    IIconnection -> open();
    query.execute();
    if( !query.tuples() -> empty() ) {

      ldif2classad::LDAPForwardIterator ldap_it( query.tuples() );   
      ldap_it.first();
      bool all_cluster_query_fail = true;

      while( ldap_it.current() ) {		

	ldif2classad::LDIFObject ldif_CE = (*ldap_it);
	  
	if( !clsAttributes.empty() ) {
	  string cluster(getClusterName(ldif_CE));
	  if( mergeClustersInfo(cluster, clsAttributes, *IIconnection, ldif_CE) ) {
	     
	    all_cluster_query_fail = false;
	  }
	} 

        m_CE_info_cache.push_back(ldif_CE);
	ldap_it.next();

      } // while( ldap_it.current() )

      if( !clsAttributes.empty() && all_cluster_query_fail ) {
	
	throw matchmaking::ISClusterQueryError(
					       NSconf->ii_contact(),
					       NSconf->ii_port(),
					       NSconf->ii_dn());
      }
    }
  }
  catch( ldif2classad::LDAPNoEntryEx& ) {
    
    edglog( severe ) << "Unexpected result while searching the IS..." << endl;
    throw matchmaking::ISQueryError(
				    NSconf->ii_contact(),
				    NSconf->ii_port(),
				    NSconf->ii_dn(),
				    filter);
  }
  catch( ldif2classad::QueryException& e) {
    
    edglog( warning ) << e.what() << endl;
    throw matchmaking::ISQueryError( 
				    NSconf->ii_contact(),
				    NSconf->ii_port(),
				    NSconf->ii_dn(),
				    filter);
  }
  catch( ldif2classad::ConnectionException& e) {
    
    edglog( warning ) << e.what() << endl;
    throw matchmaking::ISConnectionError(
					 NSconf->ii_contact(),
					 NSconf->ii_port(),
					 NSconf->ii_dn());
  }
  m_CE_info_prefetched = true;
}
      
/**
 * Check requirements.
 * This method fills suitableCEs vector with CEs satisfying requirements as expressed in the requestAd.
 * @param requestAd
 * @param suitableCEs
 */
void matchmakerGlueImpl::checkRequirement(classad::ClassAd& requestAd_, match_table_t& suitableCEs, bool use_prefetched_ces)
{
  edglog_fn(checkRequirement);     	

  classad::ClassAd const* requestAd = &requestAd_;

  if (use_prefetched_ces && (!m_CE_info_prefetched)) {
    edglog( warning ) << "checkRequirement was flagged to use prefetched values, but prefetchCEInfo was not called. Ignoring flag." << endl;
    use_prefetched_ces = false;
  }

  const configuration::NSConfiguration* NSconf = configuration::Configuration::instance() -> ns();
  
  classad::ExprTree* requirements_expr = requestAd -> Lookup("requirements");
  
  boost::scoped_ptr<ldif2classad::LDAPConnection> IIconnection;
  
  IIconnection.reset( new ldif2classad::LDAPSynchConnection( NSconf -> ii_dn(),
							     NSconf -> ii_contact(),
							     NSconf -> ii_port(),
							     NSconf -> ii_timeout()) );

  string filter;
  vector<string> reqAttributes;
  vector<string> clsAttributes;
  bool start_from_given_CEs_set  = !suitableCEs.empty();

  if (!use_prefetched_ces) {

    utilities::insertAttributeInVector(&reqAttributes, requirements_expr,
				       utilities::is_reference_to("other"));
  
    reqAttributes.push_back(GS_GLUECE_ACCESSCONTROL_BASERULE);
    reqAttributes.push_back(GS_GLUECE_UNIQUEID);
    reqAttributes.push_back(GS_GLUECE_INFO_LRMS_TYPE);
    reqAttributes.push_back(GS_GLUECE_HOSTINGCLUSTER);
    reqAttributes.push_back(GS_GLUECE_FOREIGNKEY);
    reqAttributes.push_back("GlueInformationServiceURL");
  
    // checks if the query for gaining clusters information is needed or not	
    std::accumulate(reqAttributes.begin(), reqAttributes.end(), 
		    &clsAttributes, insertMatchingRegEx("^Glue[(Host)|(Sub)].*"));
  
    // if context.suitableCEs is not empty the match-making algorithms
    // should use these CEs to match requirements for...
    // moreover it should be noted that in this case there is no need 
    // to check if the user is authorised at a give CE...since is has    
    // been alreadry checked during the data matching phase, which
    // populated the suitableCEs  
  
    if(  start_from_given_CEs_set ) {
    
      //
      // construct the or part of the LDAP filter so that 
      // only the CEs in suitableCEs will be contacted...
      //
      string ORprefix;
      string ORexpr;
      match_table_t::const_iterator or_it = suitableCEs.begin();
    
      ORexpr = "(" + string(GS_GLUECE_UNIQUEID) + "=" + (*or_it).first + ")";
    
      while( ++or_it != suitableCEs.end() ) {
      
        ORprefix.append("(|");
        ORexpr.append("(" +  string(GS_GLUECE_UNIQUEID) + "=" + (*or_it).first + "))");
    }
      filter = "(&(objectclass=GlueCE)";
      if( suitableCEs.size() > 1) filter += ORprefix;     
      filter += ORexpr + ")";
    }
    else {
    
      string ucs( utilities::evaluate_attribute(*requestAd,DGS_CERTIFICATESUBJECT) );
      string vo ( utilities::evaluate_attribute(*requestAd, "VirtualOrganisation") );
      filter = "(&(objectclass=GLUECE)(|(" + string(GS_GLUECE_ACCESSCONTROL_BASERULE) + "=VO:" + vo + ")(" +
        string(GS_GLUECE_ACCESSCONTROL_BASERULE) + "=" + ucs + ")))";
    }
  } // if not using prefetched CEs
  
  ldif2classad::LDAPQuery query(IIconnection.get(), filter, reqAttributes);
  
  if (use_prefetched_ces) {
    edglog( debug ) << "Analysing CE info cache." << endl;
  }
  else {
    edglog( debug ) << "Filtering Information Index: " << filter << endl;
  }
  
  try { 
    
    IIconnection -> open();
    if (!use_prefetched_ces) {
      query.execute();
    }

    if( use_prefetched_ces || (!query.tuples() -> empty()) ) {
      
      ldif2classad::LDAPForwardIterator ldap_it(query.tuples());   

      if (!use_prefetched_ces) {
        ldap_it.first();
      }

      bool all_cluster_query_fail = true;
      
      // { fix for bug #2493
      bool need_output_se_check = false;
      bool all_output_se_check_fail = true;
      
      // } fix for bug #2493	
      
      ldif2classad::LDIFObject ldif_CE;
      std::vector<ldif2classad::LDIFObject>::const_iterator lobj_it;

      if (use_prefetched_ces) lobj_it = m_CE_info_cache.begin();

      while (use_prefetched_ces ? (lobj_it != m_CE_info_cache.end()) :
                                  (ldap_it.current()!=0) ) {

	if (use_prefetched_ces) {
		ldif_CE = (ldif2classad::LDIFObject)(*lobj_it);
	} else {
		ldif_CE = (ldif2classad::LDIFObject)(*ldap_it);
	}

	// { fix for bug #2493       
	need_output_se_check = requestAd -> Lookup("OutputSE");
        bool output_se_check_good = true;
	
	if( need_output_se_check ) {
      		
		output_se_check_good = false;
		
		string OutputSE( utilities::evaluate_attribute(*requestAd,"OutputSE"));
		string oSE_filter;
		string ce_str;
		vector<string> oSE_attributes;
		
		ldif_CE.EvaluateAttribute(GS_GLUECE_UNIQUEID, ce_str);
	
		oSE_filter = "(&(&(objectclass=GlueCESEBind)(" +
			string("GlueCESEBindSEUniqueID") + "=" +
			OutputSE + "))(" +
			string("GlueCESEBindCEUniqueID") + "=" +
			ce_str + "))";

		edglog(debug) << "Checking OutputSE (" + OutputSE + ") dependencies..." << endl;
		
		ldif2classad::LDAPQuery oSE_query(IIconnection.get(), oSE_filter, oSE_attributes);
		
		try {
			oSE_query.execute(); 
			
			if( oSE_query.tuples() -> empty() ) {
			 	
				string msg( "OutputSE check failed:" + OutputSE + " is not close to " + ce_str );
				edglog(error) << msg  << endl;
			}
			else {
			
				output_se_check_good = true;
				all_output_se_check_fail = false;
			}
		}
  		catch( ldif2classad::QueryException& e) {

	      		edglog( warning ) << e.what() << endl;
	          	throw matchmaking::ISQueryError( NSconf->ii_contact(),
							 NSconf->ii_port(),
							 NSconf->ii_dn(),
							 oSE_filter);
		}			  
	}
	if(output_se_check_good) 
	// fix for bug #2493 }	
	try {
		    
	  if( !clsAttributes.empty() && (!use_prefetched_ces)) {
	    string cluster(getClusterName(ldif_CE));
	    if( mergeClustersInfo(cluster, clsAttributes, *IIconnection, ldif_CE) ) {
	      
	      all_cluster_query_fail = false;
	    }
	  }
	  utilities::ii_attributes::const_iterator attrs_begin, attrs_end;
	  boost::tie(attrs_begin,attrs_end) = utilities::ii_attributes::multiValued();
	  boost::shared_ptr<classad::ClassAd> ceAd( ldif_CE.asClassAd(attrs_begin, attrs_end) );
	  
	  string ce_str;
	  
	  ce_str.assign( utilities::evaluate_attribute(*ceAd, GS_GLUECE_UNIQUEID) );
	  static boost::regex  expression_ceid( "(.+/[^\\-]+-([^\\-]+))-(.+)" );
          boost::smatch        pieces_ceid;
	  string  	       gcrs, type, name;
	  
	  if( boost::regex_match(ce_str, pieces_ceid, expression_ceid) ) {
	    
	    gcrs.assign     (pieces_ceid[1].first, pieces_ceid[1].second);
	    /** This addresses bug #2455. */
	    // The LRMS type in the GRAM contact can be modified for
	    // specific installations, so we have to rely on the LRMS type.
	    // type.assign     (pieces_ceid[2].first, pieces_ceid[2].second);
	    try {
	      type.assign( utilities::evaluate_attribute(*ceAd, GS_GLUECE_INFO_LRMS_TYPE) );
	    } catch( utilities::InvalidValue& e ) {
	      // Try to fall softly in case the attribute is missing...
	      type.assign     (pieces_ceid[2].first, pieces_ceid[2].second);
	      edglog( warning ) << "Cannot evaluate " << GS_GLUECE_INFO_LRMS_TYPE << " using value from contact string: " << type << endl;
	    }

	    // ... or in case the attribute is empty.
	    if (type.length() == 0) type.assign(pieces_ceid[2].first, pieces_ceid[2].second);
	    name.assign     (pieces_ceid[3].first, pieces_ceid[3].second);
	  }
	  else throw BadCEIdFormatEx();
	  
	  ceAd -> InsertAttr( "GlobusResourceContactString", gcrs   );
	  ceAd -> InsertAttr( "LRMSType",		     type   );
	  ceAd -> InsertAttr( "QueueName",		     name   );
	  
	  // Required by other component than MatchMaker... 
	  // induce bug #2470  
	  ceAd -> InsertAttr( "CEid",			     ce_str );
	  
	  // 
	  // if the requirements expression of the request ad contains
	  // gang-match functions we have to load the plugin library
	  // it could be possible to check whether the requirements 
	  // contains a function call to such function and then load
	  // the plugin library... in order to avoid the overhead due 
	  // to such a search we prefer to load the library always.
	  
	  // gang-match requires a nested classad within the CEad containing
	  // call to extended functions which allow to acquire all the required
	  // information...since this classad does not change it is possible
	  // we will parse it just once...
	  
	  if( !gang_match_storageAd ) {
	   
	   string adstr("[CEid = parent.GlueCEUniqueID; \
				VO = parent.other.VirtualOrganisation; \
				additionalSESAInfo = listAttrRegEx(\"^GlueS[EA].*\", parent.other.requirements); \
				CloseSEs = retrieveCloseSEsInfo( CEid, VO, additionalSESAInfo ); ]");
	    gang_match_storageAd.reset( utilities::parse_classad(adstr) );			      
	  } 
	  
          ceAd->Insert("storage", gang_match_storageAd->Copy());	
	  
	  //
	  // Construct the CE's requirement expression as follows:
	  // requirements = !member(GS_GLUECE_UNIQUEID, other.edg_wm_ces_to_exclude  
	  //
	  bool CEad_matches_requestAd = false;
	  
	  edglog( debug ) << "-[ CEAd ]-----------------------------------------------------------------------------------" << endl;
	  edglog( debug ) << *ceAd << endl;
	  edglog( debug ) << "-[ Request Ad ]-----------------------------------------------------------------------------------" << endl;
	  edglog( debug ) << *requestAd << endl;
	  edglog( debug ) << "--------------------------------------------------------------------------------------------------" << endl;
	  
	  if( requestAd -> Lookup("edg_previous_matches") ) {
	    
	    classad::ExprTree *expr;
	    string CErequirement( string("member(") + 
				  string(GS_GLUECE_UNIQUEID) + 
				  string(", other.edg_previous_matches) == false") );
	    
	    classad::ClassAdParser parser;
	    parser.ParseExpression(CErequirement, expr);
	    ceAd -> Insert("requirements", expr);
	    CEad_matches_requestAd = utilities::left_matches_right(*ceAd, *requestAd);

	    if (!CEad_matches_requestAd) {
	      // The match with the requirement to remove previous matches
	      // failed, but if we get here, we still haven't exhausted
	      // the job RetryCount, so let's ignore the previous matches,
	      // and apply the normal ranking.
	      ceAd -> Delete("requirements");
	      CEad_matches_requestAd = utilities::left_matches_right(*ceAd,
								     *requestAd);
	    }          
	  }	
	  else {
	    
	    CEad_matches_requestAd = utilities::left_matches_right(*ceAd, *requestAd);
	  }
	  
	  //	for( std::vector<std::string>::const_iterator it = reqAttributes.begin();
	  //			it != reqAttributes.end(); it++) ceAd -> Delete(*it);
	  
	  if( CEad_matches_requestAd ) {
	    
	    string isURL;
	    try {

	      isURL.assign( utilities::evaluate_attribute(*ceAd, "GlueInformationServiceURL") );
	      static boost::regex  expression_gisu( "\\S.*://(.*):([0-9]+)/(.*)" );
	      boost::smatch        pieces_gisu;
	      string  		  isbasedn, isport, ishost;
	      
	      if( boost::regex_match(isURL, pieces_gisu, expression_gisu) ) {
		
		ishost.assign  (pieces_gisu[1].first, pieces_gisu[1].second);
		isport.assign  (pieces_gisu[2].first, pieces_gisu[2].second);
		isbasedn.assign(pieces_gisu[3].first, pieces_gisu[3].second);
		
		ceAd -> InsertAttr( "InformationServiceDN",   isbasedn);
		ceAd -> InsertAttr( "InformationServiceHost", ishost);
		ceAd -> InsertAttr( "InformationServicePort", std::atoi(isport.c_str()) );
	      }
	      
	      edglog( info ) << ce_str << ", Ok!" << endl << flush;
	      suitableCEs[ ce_str ] = match_info(ceAd);
	    } 
	    catch( utilities::InvalidValue& e ) {
	      
	      edglog( warning ) << "Cannot evaluate GlueInformationServiceURL..." << endl;
	    }
	  }  
	  else if( start_from_given_CEs_set ) suitableCEs.erase(ce_str);
	}
	catch( ldif2classad::LDAPNoEntryEx& ) {
	  
	  edglog( warning ) << "Cannot retrieve subclusters..." << endl;
	}
	catch( ldif2classad::QueryException& e) {
	  
	  edglog( warning ) << "Cannot retrieve subclusters: " << e.what() << endl;
	}
	catch( utilities::InvalidValue& e ) {
	  
	  edglog( error ) << "Cannot evaluate GlueCEUniqueId..." << endl;
	}
	catch( boost::bad_expression& e ) {
	  
	  edglog( error ) << e.what() << endl;
	}
	catch( BadCEIdFormatEx& ) {
	  
	  edglog( error ) << "Bad CEid format..." << endl;
	}

	// Increment iterators
	if (use_prefetched_ces) {
		lobj_it++;
	} else {
		ldap_it.next();
	}
      } // while over CE objects.
      
      // { fix for bug #2493
      if( need_output_se_check && all_output_se_check_fail ) return;		      
      // fix for bug #2493 }
      
      if( !clsAttributes.empty() && all_cluster_query_fail ) {
	
	throw matchmaking::ISClusterQueryError(
					       NSconf->ii_contact(),
					       NSconf->ii_port(),
					       NSconf->ii_dn());
      }
    }
    else {
      throw matchmaking::ISNoResultError(
					 NSconf->ii_contact(),
					 NSconf->ii_port(),
					 NSconf->ii_dn(),
					 filter);
    }
  }
  catch( ldif2classad::LDAPNoEntryEx& ) {
    
    edglog( severe ) << "Unexpected result while searching the IS..." << endl;
    throw matchmaking::ISQueryError(
				    NSconf->ii_contact(),
				    NSconf->ii_port(),
				    NSconf->ii_dn(),
				    filter);
  }
  catch( ldif2classad::QueryException& e) {
    
    edglog( warning ) << e.what() << endl;
    throw matchmaking::ISQueryError( 
				    NSconf->ii_contact(),
				    NSconf->ii_port(),
				    NSconf->ii_dn(),
				    filter);
  }
  catch( ldif2classad::ConnectionException& e) {
    
    edglog( warning ) << e.what() << endl;
    throw matchmaking::ISConnectionError(
					 NSconf->ii_contact(),
					 NSconf->ii_port(),
					 NSconf->ii_dn());
  }
}

/**
 * Checks the rank of CE in suitableCEs vector.
 * @param context a pointer to the matchmaking context.
 */
void matchmakerGlueImpl::checkRank(classad::ClassAd& requestAd_, match_table_t& suitableCEs, bool use_prefetched_ces)
{
  edglog_fn(checkRank);	

  classad::ClassAd const* requestAd = &requestAd_;

  if (use_prefetched_ces && (!m_CE_info_prefetched)) {
    edglog( warning ) << "checkRank was flagged to use prefetched values, but prefetchCEInfo was not called. Ignoring flag." << endl;
    use_prefetched_ces = false;
  }

  if( !suitableCEs.empty() ) {
    
    const configuration::NSConfiguration* NSconf = configuration::Configuration::instance() -> ns();
    
    // theJob.HideRequirementsExpression();
    classad::ExprTree* rank_expr = requestAd -> Lookup("rank");
    
    if( rank_expr == NULL ) return;
    
    vector<string> rankAttributes;
    utilities::insertAttributeInVector(&rankAttributes, rank_expr,
				       utilities::is_reference_to("other"));
    
    rankAttributes.push_back(GS_GLUECE_INFO_LRMS_TYPE);    
    rankAttributes.push_back(GS_GLUECE_STATE_ESTIMATEDRESPONSETIME);
    rankAttributes.push_back(GS_GLUECE_HOSTINGCLUSTER);     
    rankAttributes.push_back(GS_GLUECE_FOREIGNKEY);
    
    // checks if the query for gaining clusters information is needed or not
    vector<string> clsAttributes;
    std::accumulate(rankAttributes.begin(), rankAttributes.end(),
		    &clsAttributes, insertMatchingRegEx("^Glue[(Host)|(Sub)].*"));
    
    bool unable_to_rank_all = true;		
    list<string> invalidCEs;
    boost::scoped_ptr<ldif2classad::LDAPConnection> IIconnection;
    bool all_cluster_query_fail = true;	
    std::vector<ldif2classad::LDIFObject>::const_iterator lobj_it;
    if (use_prefetched_ces) lobj_it = m_CE_info_cache.begin();

    for(match_table_t::iterator mit = suitableCEs.begin(); 
	mit != suitableCEs.end(); mit++) {
      
      string CEid( mit -> first );	
      ldif2classad::LDIFObject ldif_CE;
      boost::shared_ptr<classad::ClassAd> ceAd;

      bool prefetched_ce_found = false;
      bool ldif_ce_is_valid = false;

      if (use_prefetched_ces) {
        // Prefetched CE info case. Look up the relevant one
        edglog( debug ) << "Accessing prefetched CE info." << endl;
        std::vector<ldif2classad::LDIFObject>::const_iterator start_it = lobj_it;
	std::string ce_str;
	while(!prefetched_ce_found &&
	      lobj_it->EvaluateAttribute(GS_GLUECE_UNIQUEID, ce_str)) {
	  if (ce_str.compare(CEid) == 0) {
	    // Found it.
	    ldif_CE = *lobj_it;
	    prefetched_ce_found = true;
	    ldif_ce_is_valid = true;
	    break;
	  }
	  lobj_it++;
	  if (lobj_it == m_CE_info_cache.end())
	    lobj_it = m_CE_info_cache.begin();
	  if (lobj_it == start_it) break;
        }
      }
      if (!prefetched_ce_found) {
        string filter( "(&(objectclass=GlueCE)(" + string(GS_GLUECE_UNIQUEID) + "=" + CEid + "))");
        string CEhn( CEid.substr(0, CEid.find(":")) );
        string is_dn, is_host;
        int is_port;	      
        try {	
	  is_dn.assign( utilities::evaluate_attribute(*(mit->second).getAd(), "InformationServiceDN") );
        } 
        catch( utilities::InvalidValue& e ) {
	
	  is_dn.assign( NSconf->gris_dn() );      
	  edglog( warning ) << "Cannot evaluate InformationServiceDN, defaults to '" << is_dn << "'" << endl;
        }
        try {
	  is_host.assign( utilities::evaluate_attribute(*(mit->second).getAd(), "InformationServiceHost") );
        }
        catch( utilities::InvalidValue& e ) {
	
	  is_host.assign( CEhn.c_str() );
	  edglog( warning ) << "Cannot evaluate InformationServiceHost, defaults to '" << is_host << "'" << endl;
        }
        try {
	  is_port = utilities::evaluate_attribute(*(mit->second).getAd(), "InformationServicePort");
        }
        catch( utilities::InvalidValue& e ) {
	
	  is_port = NSconf -> gris_port();
	  edglog( warning ) << "Cannot evaluate InformationServicePort, defaults to '" << is_port << "'" << endl;
        }
      
        IIconnection.reset( new ldif2classad::LDAPSynchConnection( is_dn,
								 is_host,
								 is_port,
								 NSconf -> gris_timeout() ));
      
        ldif2classad::LDAPQuery query( IIconnection.get(), filter, rankAttributes);
      
        edglog( debug ) << "Filtering InformationIndex: " << filter << endl;
        try {
	
	  IIconnection -> open();
	  query.execute();
	  if( (!query.tuples() -> empty()) )  {
	  
	    try {
	    
	      ldif2classad::LDAPForwardIterator ldap_it(query.tuples());
	      ldap_it.first();
	    
	      ldif_CE = (*ldap_it);
	      ldif_ce_is_valid = true;
	    
	      if( !clsAttributes.empty() ) {
	      
	        string cluster(getClusterName(ldif_CE));
	        if (mergeClustersInfo(cluster, clsAttributes, *IIconnection, ldif_CE)) {
		
		  all_cluster_query_fail = false;
	        }
	      }
	    } 
	    catch( ldif2classad::LDAPNoEntryEx& ) {
	  
	      edglog( warning ) << "Cannot retrieve subclusters..." << endl;
	    }
	    catch( ldif2classad::QueryException& e) {
	  
	      edglog( warning ) << "Cannot retrieve subclusters: " << e.what() << endl;
	    }
	  } // if GRIS query not empty
	}
        catch( ldif2classad::ConnectionException& e) {
	  edglog( warning ) << e.what() << endl;
        }
        catch( ldif2classad::QueryException& e) {
	  edglog( warning ) << e.what() << endl;
        }
	catch( ldif2classad::LDAPNoEntryEx& ) {
		  
	  edglog( severe ) << "Unexpected result while searching the II: " << mit -> first << endl;
	}
      } // if no prefetched CE ldif object were found.
	    
      if (ldif_ce_is_valid) {
	try {
	    utilities::ii_attributes::const_iterator attrs_begin, attrs_end;
	    boost::tie(attrs_begin,attrs_end) = utilities::ii_attributes::multiValued();
	    ceAd.reset( ldif_CE.asClassAd( attrs_begin, attrs_end ));
	    
	    mit -> second.setRank( utilities::right_rank(*ceAd, *requestAd) );
	    unable_to_rank_all = false;	
	    edglog( info ) << mit -> first << ", " << mit -> second.getRank() << endl;
	} 
	catch( utilities::UndefinedRank& ) {

	  edglog( severe ) << "Unexpected result while ranking " << mit -> first << " rank does not evaluate to number..." << endl;
	}
      } // ldif_CE is valid
    } // Main for loop on suitableCEs

    if( !clsAttributes.empty() && all_cluster_query_fail ) {
      
      throw matchmaking::ISClusterQueryError(
					     NSconf->ii_contact(),
					     NSconf->ii_port(),
					     NSconf->ii_dn());
    }
    if(unable_to_rank_all) throw matchmaking::RankingError();
  }
}



} // namespace matchmaking
} // namespace wms
} // namespace glite
