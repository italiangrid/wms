









/* LICENSE:
Copyright (c) Members of the EGEE Collaboration. 2010. 
See http://www.eu-egee.org/partners/ for details on the copyright
holders.  

Licensed under the Apache License, Version 2.0 (the "License"); 
you may not use this file except in compliance with the License. 
You may obtain a copy of the License at 

   http://www.apache.org/licenses/LICENSE-2.0 

Unless required by applicable law or agreed to in writing, software 
distributed under the License is distributed on an "AS IS" BASIS, 
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or
implied. 
See the License for the specific language governing permissions and 
limitations under the License.

END LICENSE */

#ifndef GLITE_WMS_ICE_UTIL_CREAMJOB_H
#define GLITE_WMS_ICE_UTIL_CREAMJOB_H

/**
 *
 * ICE and WMS Headers
 *
 */
#include "main/Main.h"
#include "IceUtils.h"
#include "Url.h"
#include "IceConfManager.h"
#include "ClassadSyntax_ex.h"
#include "classad_distribution.h"
#include "common/src/configuration/ICEConfiguration.h"

/**
 *
 * Cream Client API C++ Headers
 *
 */
#include "gssapi.h"

#include "glite/ce/cream-client-api-c/creamApiLogger.h"
#include "glite/ce/cream-client-api-c/job_statuses.h"
#include "glite/ce/cream-client-api-c/VOMSWrapper.h"
#include "glite/ce/cream-client-api-c/certUtil.h"
#include "glite/ce/cream-client-api-c/CEUrl.h"

/**
 *
 * Boost's headers
 *
 */
#include <boost/thread/recursive_mutex.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/regex.hpp>

/**
 *
 * System and STL C++ Headers
 *
 */
#include <exception>
#include <vector>
#include <string>
#include <ctime>
#include <set>

namespace api_util = glite::ce::cream_client_api::util;
namespace api      = glite::ce::cream_client_api;
namespace api_util = glite::ce::cream_client_api::util;
namespace fs       = boost::filesystem;

using namespace std;
using namespace glite::wms::ice::util;



namespace glite {
  namespace wms {
    namespace ice {
      namespace util {
        
		class CreamJob {

		protected:
		                     std::string  m_grid_jobid;
		                            bool  m_changed_grid_jobid;
		                     std::string  m_cream_jobid;
		                            bool  m_changed_cream_jobid;
		                     std::string  m_complete_cream_jobid;
		                            bool  m_changed_complete_cream_jobid;
		                     std::string  m_jdl;
		                            bool  m_changed_jdl;
		                     std::string  m_ceid;
		                            bool  m_changed_ceid;
		                     std::string  m_endpoint;
		                            bool  m_changed_endpoint;
		                     std::string  m_cream_address;
		                            bool  m_changed_cream_address;
		                     std::string  m_cream_deleg_address;
		                            bool  m_changed_cream_deleg_address;
		                     std::string  m_user_proxyfile;
		                            bool  m_changed_user_proxyfile;
		                     std::string  m_user_dn;
		                            bool  m_changed_user_dn;
		                     std::string  m_sequence_code;
		                            bool  m_changed_sequence_code;
		                     std::string  m_delegation_id;
		                            bool  m_changed_delegation_id;
		                     std::string  m_wn_sequence_code;
		                            bool  m_changed_wn_sequence_code;
		              unsigned short int  m_prev_status;
		                            bool  m_changed_prev_status;
		              unsigned short int  m_status;
		                            bool  m_changed_status;
		              unsigned short int  m_num_logged_status_changes;
		                            bool  m_changed_num_logged_status_changes;
		                          time_t  m_last_seen;
		                            bool  m_changed_last_seen;
		                     std::string  m_lease_id;
		                            bool  m_changed_lease_id;
		              unsigned short int  m_status_poll_retry_count;
		                            bool  m_changed_status_poll_retry_count;
		              unsigned short int  m_exit_code;
		                            bool  m_changed_exit_code;
		                     std::string  m_failure_reason;
		                            bool  m_changed_failure_reason;
		                     std::string  m_worker_node;
		                            bool  m_changed_worker_node;
		                            bool  m_killed_byice;
		                            bool  m_changed_killed_byice;
		                          time_t  m_last_empty_notification_time;
		                            bool  m_changed_last_empty_notification_time;
		                            bool  m_proxy_renewable;
		                            bool  m_changed_proxy_renewable;
		                     std::string  m_myproxy_address;
		                            bool  m_changed_myproxy_address;
		                          time_t  m_isbproxy_time_end;
		                            bool  m_changed_isbproxy_time_end;
		                     std::string  m_modified_jdl;
		                            bool  m_changed_modified_jdl;
		                          time_t  m_last_poller_visited;
		                            bool  m_changed_last_poller_visited;
		          unsigned long long int  m_cream_dbid;
		                            bool  m_changed_cream_dbid;
		                     std::string  m_token_file;
		                            bool  m_changed_token_file;
		                     std::string  m_cancel_sequence_code;
		                            bool  m_changed_cancel_sequence_code;

		                            bool  m_new;

		 public:
		  static boost::recursive_mutex s_classad_mutex;
		  static boost::recursive_mutex s_reschedule_mutex;
		  static int num_of_members( void ) { return 32; }

		public:

		 /**
		  *
		  * SETTER methods: set the Job's data members
		  *
		  */
		  virtual void  set_grid_jobid( const std::string& _grid_jobid) { m_grid_jobid = _grid_jobid; m_changed_grid_jobid=true; }
		  virtual void  set_complete_cream_jobid( const std::string& _complete_cream_jobid) { m_complete_cream_jobid = _complete_cream_jobid; m_changed_complete_cream_jobid=true; }
		  virtual void  set_ceid( const std::string& _ceid) { m_ceid = _ceid; m_changed_ceid=true; }
		  virtual void  set_endpoint( const std::string& _endpoint) { m_endpoint = _endpoint; m_changed_endpoint=true; }
		  virtual void  set_cream_address( const std::string& _cream_address) { m_cream_address = _cream_address; m_changed_cream_address=true; }
		  virtual void  set_cream_deleg_address( const std::string& _cream_deleg_address) { m_cream_deleg_address = _cream_deleg_address; m_changed_cream_deleg_address=true; }
		  virtual void  set_user_proxyfile( const std::string& _user_proxyfile) { m_user_proxyfile = _user_proxyfile; m_changed_user_proxyfile=true; }
		  virtual void  set_user_dn( const std::string& _user_dn) { m_user_dn = _user_dn; m_changed_user_dn=true; }
		  virtual void  set_delegation_id( const std::string& _delegation_id) { m_delegation_id = _delegation_id; m_changed_delegation_id=true; }
		  virtual void  set_wn_sequence_code( const std::string& _wn_sequence_code) { m_wn_sequence_code = _wn_sequence_code; m_changed_wn_sequence_code=true; }
		  virtual void  set_prev_status( const unsigned short int& _prev_status) { m_prev_status = _prev_status; m_changed_prev_status=true; }
		  virtual void  set_num_logged_status_changes( const unsigned short int& _num_logged_status_changes) { m_num_logged_status_changes = _num_logged_status_changes; m_changed_num_logged_status_changes=true; }
		  virtual void  set_last_seen( const time_t& _last_seen) { m_last_seen = _last_seen; m_changed_last_seen=true; }
		  virtual void  set_lease_id( const std::string& _lease_id) { m_lease_id = _lease_id; m_changed_lease_id=true; }
		  virtual void  set_status_poll_retry_count( const unsigned short int& _status_poll_retry_count) { m_status_poll_retry_count = _status_poll_retry_count; m_changed_status_poll_retry_count=true; }
		  virtual void  set_exit_code( const unsigned short int& _exit_code) { m_exit_code = _exit_code; m_changed_exit_code=true; }
		  virtual void  set_worker_node( const std::string& _worker_node) { m_worker_node = _worker_node; m_changed_worker_node=true; }
		  virtual void  set_killed_byice( const bool& _killed_byice) { m_killed_byice = _killed_byice; m_changed_killed_byice=true; }
		  virtual void  set_last_empty_notification_time( const time_t& _last_empty_notification_time) { m_last_empty_notification_time = _last_empty_notification_time; m_changed_last_empty_notification_time=true; }
		  virtual void  set_proxy_renewable( const bool& _proxy_renewable) { m_proxy_renewable = _proxy_renewable; m_changed_proxy_renewable=true; }
		  virtual void  set_myproxy_address( const std::string& _myproxy_address) { m_myproxy_address = _myproxy_address; m_changed_myproxy_address=true; }
		  virtual void  set_isbproxy_time_end( const time_t& _isbproxy_time_end) { m_isbproxy_time_end = _isbproxy_time_end; m_changed_isbproxy_time_end=true; }
		  virtual void  set_modified_jdl( const std::string& _modified_jdl) { m_modified_jdl = _modified_jdl; m_changed_modified_jdl=true; }
		  virtual void  set_last_poller_visited( const time_t& _last_poller_visited) { m_last_poller_visited = _last_poller_visited; m_changed_last_poller_visited=true; }
		  virtual void  set_cream_dbid( const unsigned long long int& _cream_dbid) { m_cream_dbid = _cream_dbid; m_changed_cream_dbid=true; }
		  virtual void  set_token_file( const std::string& _token_file) { m_token_file = _token_file; m_changed_token_file=true; }
		  virtual void  set_cancel_sequence_code( const std::string& _cancel_sequence_code) { m_cancel_sequence_code = _cancel_sequence_code; m_changed_cancel_sequence_code=true; }

		 /**
		  *
		  * GETTER methods: return the value of the Job's members
		  *
		  */
		  virtual std::string  grid_jobid( void ) const { return m_grid_jobid; }
		  virtual std::string  cream_jobid( void ) const { return m_cream_jobid; }
		  virtual std::string  complete_cream_jobid( void ) const { return m_complete_cream_jobid; }
		  virtual std::string  jdl( void ) const { return m_jdl; }
		  virtual std::string  ceid( void ) const { return m_ceid; }
		  virtual std::string  endpoint( void ) const { return m_endpoint; }
		  virtual std::string  cream_address( void ) const { return m_cream_address; }
		  virtual std::string  cream_deleg_address( void ) const { return m_cream_deleg_address; }
		  virtual std::string  user_proxyfile( void ) const { return m_user_proxyfile; }
		  virtual std::string  user_dn( void ) const { return m_user_dn; }
		  virtual std::string  sequence_code( void ) const { return m_sequence_code; }
		  virtual std::string  delegation_id( void ) const { return m_delegation_id; }
		  virtual std::string  wn_sequence_code( void ) const { return m_wn_sequence_code; }
		  virtual unsigned short int  prev_status( void ) const { return m_prev_status; }
		  virtual unsigned short int  status( void ) const { return m_status; }
		  virtual unsigned short int  num_logged_status_changes( void ) const { return m_num_logged_status_changes; }
		  virtual time_t  last_seen( void ) const { return m_last_seen; }
		  virtual std::string  lease_id( void ) const { return m_lease_id; }
		  virtual unsigned short int  status_poll_retry_count( void ) const { return m_status_poll_retry_count; }
		  virtual unsigned short int  exit_code( void ) const { return m_exit_code; }
		  virtual std::string  failure_reason( void ) const { return m_failure_reason; }
		  virtual std::string  worker_node( void ) const { return m_worker_node; }
		  virtual bool  killed_byice( void ) const { return m_killed_byice; }
		  virtual time_t  last_empty_notification_time( void ) const { return m_last_empty_notification_time; }
		  virtual bool  proxy_renewable( void ) const { return m_proxy_renewable; }
		  virtual std::string  myproxy_address( void ) const { return m_myproxy_address; }
		  virtual time_t  isbproxy_time_end( void ) const { return m_isbproxy_time_end; }
		  virtual std::string  modified_jdl( void ) const { return m_modified_jdl; }
		  virtual time_t  last_poller_visited( void ) const { return m_last_poller_visited; }
		  virtual unsigned long long int  cream_dbid( void ) const { return m_cream_dbid; }
		  virtual std::string  cancel_sequence_code( void ) const { return m_cancel_sequence_code; }

		 /**
		  *
		  * Database field name GETTER methods: return the names of the database column
		  *
		  */
		  static std::string grid_jobid_field( void ) { return "grid_jobid"; }
		  static std::string cream_jobid_field( void ) { return "cream_jobid"; }
		  static std::string complete_cream_jobid_field( void ) { return "complete_cream_jobid"; }
		  static std::string jdl_field( void ) { return "jdl"; }
		  static std::string ceid_field( void ) { return "ceid"; }
		  static std::string endpoint_field( void ) { return "endpoint"; }
		  static std::string cream_address_field( void ) { return "cream_address"; }
		  static std::string cream_deleg_address_field( void ) { return "cream_deleg_address"; }
		  static std::string user_proxyfile_field( void ) { return "user_proxyfile"; }
		  static std::string user_dn_field( void ) { return "user_dn"; }
		  static std::string sequence_code_field( void ) { return "sequence_code"; }
		  static std::string delegation_id_field( void ) { return "delegation_id"; }
		  static std::string wn_sequence_code_field( void ) { return "wn_sequence_code"; }
		  static std::string prev_status_field( void ) { return "prev_status"; }
		  static std::string status_field( void ) { return "status"; }
		  static std::string num_logged_status_changes_field( void ) { return "num_logged_status_changes"; }
		  static std::string last_seen_field( void ) { return "last_seen"; }
		  static std::string lease_id_field( void ) { return "lease_id"; }
		  static std::string status_poll_retry_count_field( void ) { return "status_poll_retry_count"; }
		  static std::string exit_code_field( void ) { return "exit_code"; }
		  static std::string failure_reason_field( void ) { return "failure_reason"; }
		  static std::string worker_node_field( void ) { return "worker_node"; }
		  static std::string killed_byice_field( void ) { return "killed_byice"; }
		  static std::string last_empty_notification_time_field( void ) { return "last_empty_notification_time"; }
		  static std::string proxy_renewable_field( void ) { return "proxy_renewable"; }
		  static std::string myproxy_address_field( void ) { return "myproxy_address"; }
		  static std::string isbproxy_time_end_field( void ) { return "isbproxy_time_end"; }
		  static std::string modified_jdl_field( void ) { return "modified_jdl"; }
		  static std::string last_poller_visited_field( void ) { return "last_poller_visited"; }
		  static std::string cream_dbid_field( void ) { return "cream_dbid"; }
		  static std::string token_file_field( void ) { return "token_file"; }
		  static std::string cancel_sequence_code_field( void ) { return "cancel_sequence_code"; }

		  CreamJob(
	  		  const std::string& grid_jobid,
			  const std::string& cream_jobid,
			  const std::string& complete_cream_jobid,
			  const std::string& jdl,
			  const std::string& ceid,
			  const std::string& endpoint,
			  const std::string& cream_address,
			  const std::string& cream_deleg_address,
			  const std::string& user_proxyfile,
			  const std::string& user_dn,
			  const std::string& sequence_code,
			  const std::string& delegation_id,
			  const std::string& wn_sequence_code,
			  const unsigned short int& prev_status,
			  const unsigned short int& status,
			  const unsigned short int& num_logged_status_changes,
			  const time_t& last_seen,
			  const std::string& lease_id,
			  const unsigned short int& status_poll_retry_count,
			  const unsigned short int& exit_code,
			  const std::string& failure_reason,
			  const std::string& worker_node,
			  const bool& killed_byice,
			  const time_t& last_empty_notification_time,
			  const bool& proxy_renewable,
			  const std::string& myproxy_address,
			  const time_t& isbproxy_time_end,
			  const std::string& modified_jdl,
			  const time_t& last_poller_visited,
			  const unsigned long long int& cream_dbid,
			  const std::string& token_file,
			  const std::string& cancel_sequence_code
			) : m_grid_jobid( grid_jobid ),m_changed_grid_jobid( true ),m_cream_jobid( cream_jobid ),m_changed_cream_jobid( true ),m_complete_cream_jobid( complete_cream_jobid ),m_changed_complete_cream_jobid( true ),m_jdl( jdl ),m_changed_jdl( true ),m_ceid( ceid ),m_changed_ceid( true ),m_endpoint( endpoint ),m_changed_endpoint( true ),m_cream_address( cream_address ),m_changed_cream_address( true ),m_cream_deleg_address( cream_deleg_address ),m_changed_cream_deleg_address( true ),m_user_proxyfile( user_proxyfile ),m_changed_user_proxyfile( true ),m_user_dn( user_dn ),m_changed_user_dn( true ),m_sequence_code( sequence_code ),m_changed_sequence_code( true ),m_delegation_id( delegation_id ),m_changed_delegation_id( true ),m_wn_sequence_code( wn_sequence_code ),m_changed_wn_sequence_code( true ),m_prev_status( prev_status ),m_changed_prev_status( true ),m_status( status ),m_changed_status( true ),m_num_logged_status_changes( num_logged_status_changes ),m_changed_num_logged_status_changes( true ),m_last_seen( last_seen ),m_changed_last_seen( true ),m_lease_id( lease_id ),m_changed_lease_id( true ),m_status_poll_retry_count( status_poll_retry_count ),m_changed_status_poll_retry_count( true ),m_exit_code( exit_code ),m_changed_exit_code( true ),m_failure_reason( failure_reason ),m_changed_failure_reason( true ),m_worker_node( worker_node ),m_changed_worker_node( true ),m_killed_byice( killed_byice ),m_changed_killed_byice( true ),m_last_empty_notification_time( last_empty_notification_time ),m_changed_last_empty_notification_time( true ),m_proxy_renewable( proxy_renewable ),m_changed_proxy_renewable( true ),m_myproxy_address( myproxy_address ),m_changed_myproxy_address( true ),m_isbproxy_time_end( isbproxy_time_end ),m_changed_isbproxy_time_end( true ),m_modified_jdl( modified_jdl ),m_changed_modified_jdl( true ),m_last_poller_visited( last_poller_visited ),m_changed_last_poller_visited( true ),m_cream_dbid( cream_dbid ),m_changed_cream_dbid( true ),m_token_file( token_file ),m_changed_token_file( true ),m_cancel_sequence_code( cancel_sequence_code ),m_changed_cancel_sequence_code( true ) , m_new(true) { }

		  CreamJob() : m_grid_jobid( "" ),m_changed_grid_jobid( true ),m_cream_jobid( "" ),m_changed_cream_jobid( true ),m_complete_cream_jobid( "" ),m_changed_complete_cream_jobid( true ),m_jdl( "" ),m_changed_jdl( true ),m_ceid( "" ),m_changed_ceid( true ),m_endpoint( "" ),m_changed_endpoint( true ),m_cream_address( "" ),m_changed_cream_address( true ),m_cream_deleg_address( "" ),m_changed_cream_deleg_address( true ),m_user_proxyfile( "" ),m_changed_user_proxyfile( true ),m_user_dn( "" ),m_changed_user_dn( true ),m_sequence_code( "" ),m_changed_sequence_code( true ),m_delegation_id( "" ),m_changed_delegation_id( true ),m_wn_sequence_code( "" ),m_changed_wn_sequence_code( true ),m_prev_status( 11 ),m_changed_prev_status( true ),m_status( 11 ),m_changed_status( true ),m_num_logged_status_changes( 0 ),m_changed_num_logged_status_changes( true ),m_last_seen( 0 ),m_changed_last_seen( true ),m_lease_id( "" ),m_changed_lease_id( true ),m_status_poll_retry_count( 0 ),m_changed_status_poll_retry_count( true ),m_exit_code( 0 ),m_changed_exit_code( true ),m_failure_reason( "" ),m_changed_failure_reason( true ),m_worker_node( "" ),m_changed_worker_node( true ),m_killed_byice( 0 ),m_changed_killed_byice( true ),m_last_empty_notification_time( 0 ),m_changed_last_empty_notification_time( true ),m_proxy_renewable( 0 ),m_changed_proxy_renewable( true ),m_myproxy_address( "" ),m_changed_myproxy_address( true ),m_isbproxy_time_end( 0 ),m_changed_isbproxy_time_end( true ),m_modified_jdl( "" ),m_changed_modified_jdl( true ),m_last_poller_visited( 0 ),m_changed_last_poller_visited( true ),m_cream_dbid( 0 ),m_changed_cream_dbid( true ),m_token_file( "" ),m_changed_token_file( true ),m_cancel_sequence_code( "" ),m_changed_cancel_sequence_code( true ), m_new(true) {}

		  void reset_change_flags( void ) {
		    m_changed_grid_jobid = false; 
		    m_changed_cream_jobid = false; 
		    m_changed_complete_cream_jobid = false; 
		    m_changed_jdl = false; 
		    m_changed_ceid = false; 
		    m_changed_endpoint = false; 
		    m_changed_cream_address = false; 
		    m_changed_cream_deleg_address = false; 
		    m_changed_user_proxyfile = false; 
		    m_changed_user_dn = false; 
		    m_changed_sequence_code = false; 
		    m_changed_delegation_id = false; 
		    m_changed_wn_sequence_code = false; 
		    m_changed_prev_status = false; 
		    m_changed_status = false; 
		    m_changed_num_logged_status_changes = false; 
		    m_changed_last_seen = false; 
		    m_changed_lease_id = false; 
		    m_changed_status_poll_retry_count = false; 
		    m_changed_exit_code = false; 
		    m_changed_failure_reason = false; 
		    m_changed_worker_node = false; 
		    m_changed_killed_byice = false; 
		    m_changed_last_empty_notification_time = false; 
		    m_changed_proxy_renewable = false; 
		    m_changed_myproxy_address = false; 
		    m_changed_isbproxy_time_end = false; 
		    m_changed_modified_jdl = false; 
		    m_changed_last_poller_visited = false; 
		    m_changed_cream_dbid = false; 
		    m_changed_token_file = false; 
		    m_changed_cancel_sequence_code = false;
		    m_new = false;
		  }

		  bool is_to_update( void ) const {
		    return m_changed_grid_jobid || m_changed_cream_jobid || m_changed_complete_cream_jobid || m_changed_jdl || m_changed_ceid || m_changed_endpoint || m_changed_cream_address || m_changed_cream_deleg_address || m_changed_user_proxyfile || m_changed_user_dn || m_changed_sequence_code || m_changed_delegation_id || m_changed_wn_sequence_code || m_changed_prev_status || m_changed_status || m_changed_num_logged_status_changes || m_changed_last_seen || m_changed_lease_id || m_changed_status_poll_retry_count || m_changed_exit_code || m_changed_failure_reason || m_changed_worker_node || m_changed_killed_byice || m_changed_last_empty_notification_time || m_changed_proxy_renewable || m_changed_myproxy_address || m_changed_isbproxy_time_end || m_changed_modified_jdl || m_changed_last_poller_visited || m_changed_cream_dbid || m_changed_token_file || m_changed_cancel_sequence_code;
		  }

		  static std::string get_query_fields( void ) { 
		  			 return "grid_jobid,cream_jobid,complete_cream_jobid,jdl,ceid,endpoint,cream_address,cream_deleg_address,user_proxyfile,user_dn,sequence_code,delegation_id,wn_sequence_code,prev_status,status,num_logged_status_changes,last_seen,lease_id,status_poll_retry_count,exit_code,failure_reason,worker_node,killed_byice,last_empty_notification_time,proxy_renewable,myproxy_address,isbproxy_time_end,modified_jdl,last_poller_visited,cream_dbid,token_file,cancel_sequence_code"; }


		  std::string get_query_values( void ) const { 

		    string sql;
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_grid_jobid) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_cream_jobid) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_complete_cream_jobid) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_jdl) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_ceid) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_endpoint) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_cream_address) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_cream_deleg_address) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_user_proxyfile) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_user_dn) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_sequence_code) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_delegation_id) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_wn_sequence_code) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_prev_status) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_status) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_num_logged_status_changes) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_last_seen) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_lease_id) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_status_poll_retry_count) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_exit_code) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_failure_reason) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_worker_node) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_killed_byice) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_last_empty_notification_time) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_proxy_renewable) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_myproxy_address) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_isbproxy_time_end) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_modified_jdl) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_last_poller_visited) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_cream_dbid) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_token_file) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_cancel_sequence_code) );
		    sql += ",";
		    sql = sql.substr(0, sql.length() -1 );
		    return sql;
		  }


		  static std::string get_createdb_query( void ) { 
		  			 return "grid_jobid text primary key not null,cream_jobid text  not null,complete_cream_jobid text  not null,jdl blob  not null,ceid text  not null,endpoint text  not null,cream_address text  not null,cream_deleg_address text  not null,user_proxyfile text  not null,user_dn text  not null,sequence_code text  not null,delegation_id text  not null,wn_sequence_code text  not null,prev_status integer(2)  not null,status integer(2)  not null,num_logged_status_changes integer(2)  not null,last_seen integer(8)  not null,lease_id text  not null,status_poll_retry_count integer(2)  not null,exit_code integer(2)  not null,failure_reason blob  not null,worker_node text  not null,killed_byice integer(1)  not null,last_empty_notification_time integer(8)  not null,proxy_renewable integer(1)  not null,myproxy_address text  not null,isbproxy_time_end integer(8)  not null,modified_jdl blob  not null,last_poller_visited integer(8)  not null,cream_dbid integer(8)  not null,token_file text  not null,cancel_sequence_code text  not null"; }


		  void update_database( std::string& target ) const {
		    std::string _sql;
		    if(m_changed_grid_jobid) {
		      _sql += this->grid_jobid_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->grid_jobid( )) );
		      _sql += ",";

		    }
		    if(m_changed_cream_jobid) {
		      _sql += this->cream_jobid_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->cream_jobid( )) );
		      _sql += ",";

		    }
		    if(m_changed_complete_cream_jobid) {
		      _sql += this->complete_cream_jobid_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->complete_cream_jobid( )) );
		      _sql += ",";

		    }
		    if(m_changed_jdl) {
		      _sql += this->jdl_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->jdl( )) );
		      _sql += ",";

		    }
		    if(m_changed_ceid) {
		      _sql += this->ceid_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->ceid( )) );
		      _sql += ",";

		    }
		    if(m_changed_endpoint) {
		      _sql += this->endpoint_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->endpoint( )) );
		      _sql += ",";

		    }
		    if(m_changed_cream_address) {
		      _sql += this->cream_address_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->cream_address( )) );
		      _sql += ",";

		    }
		    if(m_changed_cream_deleg_address) {
		      _sql += this->cream_deleg_address_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->cream_deleg_address( )) );
		      _sql += ",";

		    }
		    if(m_changed_user_proxyfile) {
		      _sql += this->user_proxyfile_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->user_proxyfile( )) );
		      _sql += ",";

		    }
		    if(m_changed_user_dn) {
		      _sql += this->user_dn_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->user_dn( )) );
		      _sql += ",";

		    }
		    if(m_changed_sequence_code) {
		      _sql += this->sequence_code_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->sequence_code( )) );
		      _sql += ",";

		    }
		    if(m_changed_delegation_id) {
		      _sql += this->delegation_id_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->delegation_id( )) );
		      _sql += ",";

		    }
		    if(m_changed_wn_sequence_code) {
		      _sql += this->wn_sequence_code_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->wn_sequence_code( )) );
		      _sql += ",";

		    }
		    if(m_changed_prev_status) {
		      _sql += this->prev_status_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->prev_status( )) );
		      _sql += ",";

		    }
		    if(m_changed_status) {
		      _sql += this->status_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->status( )) );
		      _sql += ",";

		    }
		    if(m_changed_num_logged_status_changes) {
		      _sql += this->num_logged_status_changes_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->num_logged_status_changes( )) );
		      _sql += ",";

		    }
		    if(m_changed_last_seen) {
		      _sql += this->last_seen_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->last_seen( )) );
		      _sql += ",";

		    }
		    if(m_changed_lease_id) {
		      _sql += this->lease_id_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->lease_id( )) );
		      _sql += ",";

		    }
		    if(m_changed_status_poll_retry_count) {
		      _sql += this->status_poll_retry_count_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->status_poll_retry_count( )) );
		      _sql += ",";

		    }
		    if(m_changed_exit_code) {
		      _sql += this->exit_code_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->exit_code( )) );
		      _sql += ",";

		    }
		    if(m_changed_failure_reason) {
		      _sql += this->failure_reason_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->failure_reason( )) );
		      _sql += ",";

		    }
		    if(m_changed_worker_node) {
		      _sql += this->worker_node_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->worker_node( )) );
		      _sql += ",";

		    }
		    if(m_changed_killed_byice) {
		      _sql += this->killed_byice_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->killed_byice( )) );
		      _sql += ",";

		    }
		    if(m_changed_last_empty_notification_time) {
		      _sql += this->last_empty_notification_time_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->last_empty_notification_time( )) );
		      _sql += ",";

		    }
		    if(m_changed_proxy_renewable) {
		      _sql += this->proxy_renewable_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->proxy_renewable( )) );
		      _sql += ",";

		    }
		    if(m_changed_myproxy_address) {
		      _sql += this->myproxy_address_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->myproxy_address( )) );
		      _sql += ",";

		    }
		    if(m_changed_isbproxy_time_end) {
		      _sql += this->isbproxy_time_end_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->isbproxy_time_end( )) );
		      _sql += ",";

		    }
		    if(m_changed_modified_jdl) {
		      _sql += this->modified_jdl_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->modified_jdl( )) );
		      _sql += ",";

		    }
		    if(m_changed_last_poller_visited) {
		      _sql += this->last_poller_visited_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->last_poller_visited( )) );
		      _sql += ",";

		    }
		    if(m_changed_cream_dbid) {
		      _sql += this->cream_dbid_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->cream_dbid( )) );
		      _sql += ",";

		    }
		    if(m_changed_token_file) {
		      _sql += this->token_file_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->token_file( )) );
		      _sql += ",";

		    }
		    if(m_changed_cancel_sequence_code) {
		      _sql += this->cancel_sequence_code_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->cancel_sequence_code( )) );
		      _sql += ",";

		    }

		    if( _sql.empty() ) {return;}

		    if( boost::ends_with( _sql, ",") ) { _sql = _sql.substr(0, _sql.length() -1 ); }

		    _sql += " WHERE ";
		    _sql += this->grid_jobid_field();
		    _sql += "=";
		    _sql += IceUtils::withSQLDelimiters( this->grid_jobid( ) );
		    _sql += ";";
		    target = string("UPDATE jobs SET ") + _sql;
		  }

		  void set_status ( const glite::ce::cream_client_api::job_statuses::job_status& st ) {
		    m_changed_status = true;
		    m_changed_prev_status  = true;
		    m_prev_status = m_status; m_status = st;
		  }

		  void set_cream_jobid ( const std::string& cid ) { 
		    m_cream_jobid = cid; 
		    if(!m_cream_jobid.empty() && !m_cream_address.empty()) {
		      m_complete_cream_jobid = m_cream_address;
		      boost::replace_all( m_complete_cream_jobid, IceConfManager::instance()->getConfiguration()->ice()->cream_url_postfix(), "" );
		      m_complete_cream_jobid += "/" + m_cream_jobid;
		    }
		    m_changed_cream_jobid = true;
		    m_changed_complete_cream_jobid = true;
		  }

		  void set_jdl ( const std::string& j, const std::string& cmdtype ) 
		   throw( ClassadSyntax_ex& )
		  {
		    /**
		     * Classad-mutex protected region
		     */
		    boost::recursive_mutex::scoped_lock M_classad( this->s_classad_mutex );
		    classad::ClassAdParser parser;
		    classad::ClassAd *jdlAd = parser.ParseClassAd( j );
		    
		    if ( 0 == jdlAd ) {
		      throw ClassadSyntax_ex( string("unable to parse jdl=[") + j + "]" );
		    }
		    
		    boost::scoped_ptr< classad::ClassAd > classad_safe_ptr( jdlAd );
		    
		    m_jdl = j;
		    
		    // Look for the "ce_id" attribute
		    if ( !classad_safe_ptr->EvaluateAttrString( "ce_id", m_ceid ) ) {
		      throw ClassadSyntax_ex("ce_id attribute not found, or is not a string");
		    }
		    boost::trim_if(m_ceid, boost::is_any_of("\"") );
		    
		    string shallow;
		    m_token_file = "";
		    if ( !classad_safe_ptr->EvaluateAttrString( "ReallyRunningToken", m_token_file ) ) {
		      throw ClassadSyntax_ex("ReallyRunningToken attribute not found, or is not a string");
		    }
		    
		    if(m_token_file.empty())
		      m_token_file = "/nofile";
		    
		    if( classad_safe_ptr->EvaluateAttrString( "ShallowRetryCount", shallow ) ) {
		      if( atoi( shallow.c_str() ) == -1 )
		        m_token_file = "";
		    }
		    
		    boost::trim_if(m_token_file, boost::is_any_of("\"") );
		    
		    // Look for the "X509UserProxy" attribute
		    if ( !classad_safe_ptr->EvaluateAttrString( "X509UserProxy", m_user_proxyfile ) ) {
		      throw ClassadSyntax_ex("X509UserProxy attribute not found, or is not a string");
		    }
		    
		    string tmp;
		    
		    m_proxy_renewable = false;
		    
		    if ( classad_safe_ptr->EvaluateAttrString( "MYPROXYSERVER", tmp ) ) {
		    
		      boost::trim_if(tmp, boost::is_any_of(" ") );
		      
		      if( !tmp.empty() ) {
		        m_proxy_renewable = true;
		        m_myproxy_address = tmp;
		      }
		      
		    } 
		    
		    boost::trim_if(m_user_proxyfile, boost::is_any_of("\""));
		    
		    // Look for the "LBSequenceCode" attribute (if this attribute is not in the classad, the sequence code is set to the empty string
		    if ( classad_safe_ptr->EvaluateAttrString( "LB_sequence_code", m_sequence_code ) ) {
		      boost::trim_if(m_sequence_code, boost::is_any_of("\""));
		    }
		    
		    // Look for the "edg_jobid" attribute
		    if ( !classad_safe_ptr->EvaluateAttrString( "edg_jobid", m_grid_jobid ) ) {
		      throw ClassadSyntax_ex( "edg_jobid attribute not found, or is not a string" );
		    }
		    boost::trim_if(m_grid_jobid, boost::is_any_of("\"") );
		    
		    vector<string> pieces;
		    try{
		      api::util::CEUrl::parseCEID(m_ceid, pieces);
		    } catch(api::util::CEUrl::ceid_syntax_ex& ex) {
		      throw ClassadSyntax_ex(ex.what());
		    }
		    
		    m_endpoint = pieces[0] + ":" + pieces[1];
		    
		    m_cream_address = IceConfManager::instance()->getConfiguration()->ice()->cream_url_prefix() 
		      + m_endpoint + IceConfManager::instance()->getConfiguration()->ice()->cream_url_postfix();
		    
		    m_cream_deleg_address = IceConfManager::instance()->getConfiguration()->ice()->creamdelegation_url_prefix() 
		      + m_endpoint + IceConfManager::instance()->getConfiguration()->ice()->creamdelegation_url_postfix();
		    
		    // It is important to get the jdl from the job itself, rather
		    // than using the m_jdl attribute. This is because the
		    // sequence_code attribute inside the jdl classad has been
		    // modified by the L&B calls, and we have to pass to CREAM the
		    // "last" sequence code as the job wrapper will need to log
		    // the "really running" event.
		    //if( boost::algorithm::iequals( cmdtype, "submit" ) )
		    glite::wms::ice::util::IceUtils::cream_jdl_helper( (const string&)this->jdl(), m_modified_jdl );// can throw ClassadSyntax_ex
		    
		    // release of Classad-mutex
		    m_changed_jdl = true;
		    m_changed_modified_jdl = true;
		  }

		  bool is_active ( void ) const
		  {
		      if( this->killed_byice() ) return false;
		  
		      return ( ( m_status == api::job_statuses::REGISTERED ) ||
		               ( m_status == api::job_statuses::PENDING ) ||
		               ( m_status == api::job_statuses::IDLE ) ||
		               ( m_status == api::job_statuses::RUNNING ) ||
		  	     ( m_status == api::job_statuses::REALLY_RUNNING) ||
		               ( m_status == api::job_statuses::HELD ) );
		  }

		  bool can_be_purged ( void ) const
		  {
		      return ( ( m_status == api::job_statuses::DONE_OK ) ||
		               ( m_status == api::job_statuses::CANCELLED ) ||
		               ( m_status == api::job_statuses::DONE_FAILED ) ||
		               ( m_status == api::job_statuses::ABORTED ) );
		  }

		  bool can_be_resubmitted ( void ) const
		  { 
		      int threshold( IceConfManager::instance()->getConfiguration()->ice()->job_cancellation_threshold_time() );
		      api::soap_proxy::VOMSWrapper V( this->user_proxyfile( ) );
		      if ( !V.IsValid() || 
		           ( V.getProxyTimeEnd() < time(0) + threshold ) ) {
		          return false;
		      }
		      return ( ( m_status == api::job_statuses::DONE_FAILED ) ||
		               ( m_status == api::job_statuses::ABORTED ) );
		  }

		  string describe ( void ) const
		  {
		      string result;
		      result.append( "GRIDJobID=\"" );
		      result.append( m_grid_jobid );
		      result.append( "\" CREAMJobID=\"" );
		      result.append( m_complete_cream_jobid );
		      result.append( "\"" );
		      return result;
		  }

		  void set_sequence_code ( const std::string& seq )
		  {
		      string old_seq_code = m_sequence_code; 
		      m_sequence_code = seq;
		  
		      boost::replace_all( m_jdl, old_seq_code, seq );
		      boost::replace_all( m_modified_jdl, old_seq_code, seq );
		  
		      m_changed_sequence_code = true;
		  }

		  void set_failure_reason ( const std::string& f ) { 
		  if( f.empty() )
		    m_failure_reason = " "; 
		  else
		    m_failure_reason = f;
		  
		    m_changed_failure_reason = true;
		  }

		  string token_file ( void ) const {
		  
		    glite::wms::ice::util::Url url( m_token_file );
		    if(!url.is_valid() )
		      return url.get_error();
		  
		    if(url.get_path().at(0) == '/')
		      return url.get_path( );
		    else {
		      return string("/")+url.get_path( ); 
		    }
		  
		  }

		  virtual ~CreamJob( void ) {
		  }


		}; // class CreamJob
        } // namespace util
      } // namespace ice
    } // namespace wms
}; // namespace glite

#endif
