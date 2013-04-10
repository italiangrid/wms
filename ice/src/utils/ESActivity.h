
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

#ifndef GLITE_WMS_ICE_UTIL_ESACTIVITY_H
#define GLITE_WMS_ICE_UTIL_ESACTIVITY_H

/**
 *
 * ICE and WMS Headers
 *
 */


#include "ice/IceCore.h"
#include "IceUtils.h"
#include "Url.h"
#include "IceConfManager.h"
#include "ClassadSyntax_ex.h"
#include "classad_distribution.h"
#include "glite/wms/common/configuration/ICEConfiguration.h"

/**
 *
 * Cream Client API C++ Headers
 *
 */
#include "gssapi.h"

//#include "glite/ce/cream-client-api-c/creamApiLogger.h"
//#include "glite/ce/cream-client-api-c/job_statuses.h"
#include "glite/ce/cream-client-api-c/VOMSWrapper.h"
#include "glite/ce/cream-client-api-c/certUtil.h"
#include "glite/ce/es-client-api-c/ActivityStatus.h"
//#include "glite/ce/cream-client-api-c/CEUrl.h"

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

//namespace api_util = glite::ce::cream_client_api::util;
//namespace api      = glite::ce::cream_client_api;
//namespace api_util = glite::ce::cream_client_api::util;
namespace fs       = boost::filesystem;

using namespace std;
using namespace glite::wms::ice::util;



namespace glite {
  namespace wms {
    namespace ice {
      namespace util {
        
		class ESActivity {

		protected:
		                     std::string  m_grid_jobid;
		                            bool  m_changed_grid_jobid;
		                     std::string  m_es_jobid;
		                            bool  m_changed_es_jobid;
		                     std::string  m_adl;
		                            bool  m_changed_adl;
		                     std::string  m_endpoint;
		                            bool  m_changed_endpoint;
		                     std::string  m_user_proxyfile;
		                            bool  m_changed_user_proxyfile;
		                            bool  m_proxy_renewable;
		                            bool  m_changed_proxy_renewable;
		                          time_t  m_isbproxy_time_end;
		                            bool  m_changed_isbproxy_time_end;
		                     std::string  m_user_dn;
		                            bool  m_changed_user_dn;
		                     std::string  m_sequence_code;
		                            bool  m_changed_sequence_code;
		                     std::string  m_delegation_id;
		                            bool  m_changed_delegation_id;
		                     std::string  m_wn_sequence_code;
		                            bool  m_changed_wn_sequence_code;
		                       short int  m_prev_status;
		                            bool  m_changed_prev_status;
		                       short int  m_status;
		                            bool  m_changed_status;
		                          time_t  m_last_seen;
		                            bool  m_changed_last_seen;
		              unsigned short int  m_exit_code;
		                            bool  m_changed_exit_code;
		                     std::string  m_failure_reason;
		                            bool  m_changed_failure_reason;
		                     std::string  m_worker_node;
		                            bool  m_changed_worker_node;
		                     std::string  m_myproxy_address;
		                            bool  m_changed_myproxy_address;

		                            bool  m_new;

		 public:
		  static boost::recursive_mutex s_classad_mutex;
		  static boost::recursive_mutex s_reschedule_mutex;
		  static int num_of_members( void ) { return 18; }

		public:

		 /**
		  *
		  * SETTER methods: set the Job's data members
		  *
		  */
		  virtual void  set_grid_jobid( const std::string& _grid_jobid) { m_grid_jobid = _grid_jobid; m_changed_grid_jobid=true; }
		  virtual void  set_es_jobid( const std::string& _es_jobid) { m_es_jobid = _es_jobid; m_changed_es_jobid=true; }
		  virtual void  set_adl( const std::string& _adl) { m_adl = _adl; m_changed_adl=true; }
		  virtual void  set_endpoint( const std::string& _endpoint) { m_endpoint = _endpoint; m_changed_endpoint=true; }
		  virtual void  set_user_proxyfile( const std::string& _user_proxyfile) { m_user_proxyfile = _user_proxyfile; m_changed_user_proxyfile=true; }
		  virtual void  set_proxy_renewable( const bool& _proxy_renewable) { m_proxy_renewable = _proxy_renewable; m_changed_proxy_renewable=true; }
		  virtual void  set_isbproxy_time_end( const time_t& _isbproxy_time_end) { m_isbproxy_time_end = _isbproxy_time_end; m_changed_isbproxy_time_end=true; }
		  virtual void  set_user_dn( const std::string& _user_dn) { m_user_dn = _user_dn; m_changed_user_dn=true; }
		  virtual void  set_sequence_code( const std::string& _sequence_code) { m_sequence_code = _sequence_code; m_changed_sequence_code=true; }
		  virtual void  set_delegation_id( const std::string& _delegation_id) { m_delegation_id = _delegation_id; m_changed_delegation_id=true; }
		  virtual void  set_wn_sequence_code( const std::string& _wn_sequence_code) { m_wn_sequence_code = _wn_sequence_code; m_changed_wn_sequence_code=true; }
		  virtual void  set_prev_status( const short int& _prev_status) { m_prev_status = _prev_status; m_changed_prev_status=true; }
		  virtual void  set_last_seen( const time_t& _last_seen) { m_last_seen = _last_seen; m_changed_last_seen=true; }
		  virtual void  set_exit_code( const unsigned short int& _exit_code) { m_exit_code = _exit_code; m_changed_exit_code=true; }
		  virtual void  set_failure_reason( const std::string& _failure_reason) { m_failure_reason = _failure_reason; m_changed_failure_reason=true; }
		  virtual void  set_worker_node( const std::string& _worker_node) { m_worker_node = _worker_node; m_changed_worker_node=true; }
		  virtual void  set_myproxy_address( const std::string& _myproxy_address) { m_myproxy_address = _myproxy_address; m_changed_myproxy_address=true; }

		 /**
		  *
		  * GETTER methods: return the value of the Job's members
		  *
		  */
		  virtual std::string  grid_jobid( void ) const { return m_grid_jobid; }
		  virtual std::string  es_jobid( void ) const { return m_es_jobid; }
		  virtual std::string  adl( void ) const { return m_adl; }
		  virtual std::string  endpoint( void ) const { return m_endpoint; }
		  virtual std::string  user_proxyfile( void ) const { return m_user_proxyfile; }
		  virtual bool  proxy_renewable( void ) const { return m_proxy_renewable; }
		  virtual time_t  isbproxy_time_end( void ) const { return m_isbproxy_time_end; }
		  virtual std::string  user_dn( void ) const { return m_user_dn; }
		  virtual std::string  sequence_code( void ) const { return m_sequence_code; }
		  virtual std::string  delegation_id( void ) const { return m_delegation_id; }
		  virtual std::string  wn_sequence_code( void ) const { return m_wn_sequence_code; }
		  virtual short int  prev_status( void ) const { return m_prev_status; }
		  virtual short int  status( void ) const { return m_status; }
		  virtual time_t  last_seen( void ) const { return m_last_seen; }
		  virtual unsigned short int  exit_code( void ) const { return m_exit_code; }
		  virtual std::string  failure_reason( void ) const { return m_failure_reason; }
		  virtual std::string  worker_node( void ) const { return m_worker_node; }
		  virtual std::string  myproxy_address( void ) const { return m_myproxy_address; }

		 /**
		  *
		  * Database field name GETTER methods: return the names of the database column
		  *
		  */
		  static std::string grid_jobid_field( void ) { return "grid_jobid"; }
		  static std::string es_jobid_field( void ) { return "es_jobid"; }
		  static std::string adl_field( void ) { return "adl"; }
		  static std::string endpoint_field( void ) { return "endpoint"; }
		  static std::string user_proxyfile_field( void ) { return "user_proxyfile"; }
		  static std::string proxy_renewable_field( void ) { return "proxy_renewable"; }
		  static std::string isbproxy_time_end_field( void ) { return "isbproxy_time_end"; }
		  static std::string user_dn_field( void ) { return "user_dn"; }
		  static std::string sequence_code_field( void ) { return "sequence_code"; }
		  static std::string delegation_id_field( void ) { return "delegation_id"; }
		  static std::string wn_sequence_code_field( void ) { return "wn_sequence_code"; }
		  static std::string prev_status_field( void ) { return "prev_status"; }
		  static std::string status_field( void ) { return "status"; }
		  static std::string last_seen_field( void ) { return "last_seen"; }
		  static std::string exit_code_field( void ) { return "exit_code"; }
		  static std::string failure_reason_field( void ) { return "failure_reason"; }
		  static std::string worker_node_field( void ) { return "worker_node"; }
		  static std::string myproxy_address_field( void ) { return "myproxy_address"; }

		  ESActivity(
	  		  const std::string& grid_jobid,
			  const std::string& es_jobid,
			  const std::string& adl,
			  const std::string& endpoint,
			  const std::string& user_proxyfile,
			  const bool& proxy_renewable,
			  const time_t& isbproxy_time_end,
			  const std::string& user_dn,
			  const std::string& sequence_code,
			  const std::string& delegation_id,
			  const std::string& wn_sequence_code,
			  const short int& prev_status,
			  const short int& status,
			  const time_t& last_seen,
			  const unsigned short int& exit_code,
			  const std::string& failure_reason,
			  const std::string& worker_node,
			  const std::string& myproxy_address
			) : m_grid_jobid( grid_jobid ),m_changed_grid_jobid( true ),m_es_jobid( es_jobid ),m_changed_es_jobid( true ),m_adl( adl ),m_changed_adl( true ),m_endpoint( endpoint ),m_changed_endpoint( true ),m_user_proxyfile( user_proxyfile ),m_changed_user_proxyfile( true ),m_proxy_renewable( proxy_renewable ),m_changed_proxy_renewable( true ),m_isbproxy_time_end( isbproxy_time_end ),m_changed_isbproxy_time_end( true ),m_user_dn( user_dn ),m_changed_user_dn( true ),m_sequence_code( sequence_code ),m_changed_sequence_code( true ),m_delegation_id( delegation_id ),m_changed_delegation_id( true ),m_wn_sequence_code( wn_sequence_code ),m_changed_wn_sequence_code( true ),m_prev_status( prev_status ),m_changed_prev_status( true ),m_status( status ),m_changed_status( true ),m_last_seen( last_seen ),m_changed_last_seen( true ),m_exit_code( exit_code ),m_changed_exit_code( true ),m_failure_reason( failure_reason ),m_changed_failure_reason( true ),m_worker_node( worker_node ),m_changed_worker_node( true ),m_myproxy_address( myproxy_address ),m_changed_myproxy_address( true ) , m_new(true) { }

		  ESActivity() : m_grid_jobid( "" ),m_changed_grid_jobid( true ),m_es_jobid( "" ),m_changed_es_jobid( true ),m_adl( "" ),m_changed_adl( true ),m_endpoint( "" ),m_changed_endpoint( true ),m_user_proxyfile( "" ),m_changed_user_proxyfile( true ),m_proxy_renewable( 0 ),m_changed_proxy_renewable( true ),m_isbproxy_time_end( 0 ),m_changed_isbproxy_time_end( true ),m_user_dn( "" ),m_changed_user_dn( true ),m_sequence_code( "" ),m_changed_sequence_code( true ),m_delegation_id( "" ),m_changed_delegation_id( true ),m_wn_sequence_code( "" ),m_changed_wn_sequence_code( true ),m_prev_status( emi_es::client::wrapper::ActivityStatus::NA ),m_changed_prev_status( true ),m_status( emi_es::client::wrapper::ActivityStatus::NA ),m_changed_status( true ),m_last_seen( 0 ),m_changed_last_seen( true ),m_exit_code( 0 ),m_changed_exit_code( true ),m_failure_reason( "" ),m_changed_failure_reason( true ),m_worker_node( "" ),m_changed_worker_node( true ),m_myproxy_address( "" ),m_changed_myproxy_address( true ), m_new(true) {}

		  void reset_change_flags( void ) {
		    m_changed_grid_jobid = false; 
		    m_changed_es_jobid = false; 
		    m_changed_adl = false; 
		    m_changed_endpoint = false; 
		    m_changed_user_proxyfile = false; 
		    m_changed_proxy_renewable = false; 
		    m_changed_isbproxy_time_end = false; 
		    m_changed_user_dn = false; 
		    m_changed_sequence_code = false; 
		    m_changed_delegation_id = false; 
		    m_changed_wn_sequence_code = false; 
		    m_changed_prev_status = false; 
		    m_changed_status = false; 
		    m_changed_last_seen = false; 
		    m_changed_exit_code = false; 
		    m_changed_failure_reason = false; 
		    m_changed_worker_node = false; 
		    m_changed_myproxy_address = false;
		    m_new = false;
		  }

		  bool is_to_update( void ) const {
		    return m_changed_grid_jobid || m_changed_es_jobid || m_changed_adl || m_changed_endpoint || m_changed_user_proxyfile || m_changed_proxy_renewable || m_changed_isbproxy_time_end || m_changed_user_dn || m_changed_sequence_code || m_changed_delegation_id || m_changed_wn_sequence_code || m_changed_prev_status || m_changed_status || m_changed_last_seen || m_changed_exit_code || m_changed_failure_reason || m_changed_worker_node || m_changed_myproxy_address;
		  }

		  static std::string get_query_fields( void ) { 
		  			 return "grid_jobid,es_jobid,adl,endpoint,user_proxyfile,proxy_renewable,isbproxy_time_end,user_dn,sequence_code,delegation_id,wn_sequence_code,prev_status,status,last_seen,exit_code,failure_reason,worker_node,myproxy_address"; }


		  std::string get_query_values( void ) const { 

		    string sql;
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_grid_jobid) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_es_jobid) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_adl) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_endpoint) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_user_proxyfile) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_proxy_renewable) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_isbproxy_time_end) );
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
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_last_seen) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_exit_code) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_failure_reason) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_worker_node) );
		    sql += ",";
		    sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(m_myproxy_address) );
		    sql += ",";
		    sql = sql.substr(0, sql.length() -1 );
		    return sql;
		  }


		  static std::string get_createdb_query( void ) { 
		  			 return "grid_jobid text primary key not null,es_jobid text  not null,adl blob  not null,endpoint text  not null,user_proxyfile text  not null,proxy_renewable integer(1)  not null,isbproxy_time_end integer(8)  not null,user_dn text  not null,sequence_code text  not null,delegation_id text  not null,wn_sequence_code text  not null,prev_status integer(2)  not null,status integer(2)  not null,last_seen integer(8)  not null,exit_code integer(2)  not null,failure_reason blob  not null,worker_node text  not null,myproxy_address text  not null"; }


		  void update_database( std::string& target ) const {
		    std::string _sql;
		    if(m_changed_grid_jobid) {
		      _sql += this->grid_jobid_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->grid_jobid( )) );
		      _sql += ",";

		    }
		    if(m_changed_es_jobid) {
		      _sql += this->es_jobid_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->es_jobid( )) );
		      _sql += ",";

		    }
		    if(m_changed_adl) {
		      _sql += this->adl_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->adl( )) );
		      _sql += ",";

		    }
		    if(m_changed_endpoint) {
		      _sql += this->endpoint_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->endpoint( )) );
		      _sql += ",";

		    }
		    if(m_changed_user_proxyfile) {
		      _sql += this->user_proxyfile_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->user_proxyfile( )) );
		      _sql += ",";

		    }
		    if(m_changed_proxy_renewable) {
		      _sql += this->proxy_renewable_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->proxy_renewable( )) );
		      _sql += ",";

		    }
		    if(m_changed_isbproxy_time_end) {
		      _sql += this->isbproxy_time_end_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->isbproxy_time_end( )) );
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
		    if(m_changed_last_seen) {
		      _sql += this->last_seen_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->last_seen( )) );
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
		    if(m_changed_myproxy_address) {
		      _sql += this->myproxy_address_field( ); 
		      _sql += "=";
		      _sql += IceUtils::withSQLDelimiters( boost::lexical_cast<string>(this->myproxy_address( )) );
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

		  void set_status ( const emi_es::client::wrapper::ActivityStatus::ACTIVITYSTATUS& st ) {
		    m_changed_status = true;
		    m_changed_prev_status  = true;
		    m_prev_status = m_status; m_status = st;
		  }

		  virtual ~ESActivity( void ) {
		  }

		  std::string toString( void ) {
		    std::string out = "";
		    out += "m_grid_jobid = " + boost::lexical_cast<string>(m_grid_jobid) + "\n";
		    out += "m_es_jobid = " + boost::lexical_cast<string>(m_es_jobid) + "\n";
		    out += "m_adl = " + boost::lexical_cast<string>(m_adl) + "\n";
		    out += "m_endpoint = " + boost::lexical_cast<string>(m_endpoint) + "\n";
		    out += "m_user_proxyfile = " + boost::lexical_cast<string>(m_user_proxyfile) + "\n";
		    out += "m_proxy_renewable = " + boost::lexical_cast<string>(m_proxy_renewable) + "\n";
		    out += "m_isbproxy_time_end = " + boost::lexical_cast<string>(m_isbproxy_time_end) + "\n";
		    out += "m_user_dn = " + boost::lexical_cast<string>(m_user_dn) + "\n";
		    out += "m_sequence_code = " + boost::lexical_cast<string>(m_sequence_code) + "\n";
		    out += "m_delegation_id = " + boost::lexical_cast<string>(m_delegation_id) + "\n";
		    out += "m_wn_sequence_code = " + boost::lexical_cast<string>(m_wn_sequence_code) + "\n";
		    out += "m_prev_status = " + boost::lexical_cast<string>(m_prev_status) + "\n";
		    out += "m_status = " + boost::lexical_cast<string>(m_status) + "\n";
		    out += "m_last_seen = " + boost::lexical_cast<string>(m_last_seen) + "\n";
		    out += "m_exit_code = " + boost::lexical_cast<string>(m_exit_code) + "\n";
		    out += "m_failure_reason = " + boost::lexical_cast<string>(m_failure_reason) + "\n";
		    out += "m_worker_node = " + boost::lexical_cast<string>(m_worker_node) + "\n";
		    out += "m_myproxy_address = " + boost::lexical_cast<string>(m_myproxy_address) + "\n";
		    return out;
		  };

		}; // class ESActivity
        } // namespace util
      } // namespace ice
    } // namespace wms
}; // namespace glite

#endif
