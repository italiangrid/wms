
/*
 * Notification.h
 * 
 * Copyright (c) 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

#ifndef _GLITE_WMS_MANAGER_NS_CLIENT_NOTIFICATION_H_
#define _GLITE_WMS_MANAGER_NS_CLIENT_NOTIFICATION_H_


#include <string>
#include <vector>

#include <classad_distribution.h>

using namespace classad;

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace client {

  /**
   * An enumerative type representing the allowed notification types
   *
   * @version
   * @date September 16 2002 
   * @author Salvatore Monforte
   * @author Marco Pappalardo
   */
  enum notification_type_t 
  {
    SUBMIT_STATUS       = (1L<<5), /**< Requests notification about the status of a submission. */
    LIST_MATCH_STATUS   = (1L<<6), /**< Requests notification about the status of a list match command. */
    CANCEL_STATUS       = (1L<<7), /**< Requests notification about the cancellation of a job */
    GET_OUTPUT_STATUS   = (1L<<8), /**< Requests notification about the status of a output sandbox transfer. */
  };
  
  /**
   * An enumerative type representing the notification options.
   *
   * @version
   * @date September 16 2002 
   * @author Salvatore Monforte
   * @author Marco Pappalardo
   */
  enum notification_option_t 
  {
    ON_DONE        = (SUBMIT_STATUS | (1L<<1)),      /**< Requests notification about job done event. */
    ON_ABORTED     = (SUBMIT_STATUS | (1L<<2)),      /**< Requests notification about job aborted. */
    ON_READY       = (SUBMIT_STATUS | (1L<<3)),      /**< Requests notification about end match-making. */ 
    ON_RUNNING     = (SUBMIT_STATUS | (1L<<4)),      /**< Requests notification anout job running event. */
    WITH_RANK      = (LIST_MATCH_STATUS | (1L<<1)),  /**< Requests notification anout job list-match event. */
    FOR_EACH_FILE  = (GET_OUTPUT_STATUS | (1L<<1))   /**< Requests notification anout job sandbox stransfer event*/
  };

  /**
   * An enumerative indicating the status of cancel actions.
   *
   * @version
   * @date September 16 2002 
   * @author Salvatore Monforte
   * @author Marco Pappalardo
   */
  enum cancel_status_t { 
    OK,   
    MARKED_FOR_REMOVAL, 
    GENERIC_FAILURE, 
    CONDOR_FAILURE 
  };

  /**
   * The wrapper on notified classads.
   *
   * @version
   * @date September 16 2002 
   * @author Salvatore Monforte
   * @author Marco Pappalardo
   */
  class notifiedad_t
    {
    public:
      /**
       * Constructor
       * @param ad the notified classad.
       **/
      notifiedad_t(ClassAd* ad) { this -> ad = ad; }
      /**
       * Type extractor.
       * @return the notification type the ad refers to
       **/
      notification_type_t type() const 
	{ 
	  int t; 
	  assert( ad -> EvaluateAttrInt( "NotificationType", t ) == true );
	  return (notification_type_t)(t);
	}
 
      /**
       * JobStatus extractor.
       * Returns the value of the notified status as defined by relative 
       * notification_option_t enumerative type according with the set of values used,
       * in combination with SUBMIT_STATUS notification type, at call-back registration time.
       * @return the value of the notified status.
       */
      notification_option_t job_status() const
	{
	  int s;
	  assert( ad -> EvaluateAttrInt( "JobStatus", s ) == true );
	  return (notification_option_t)(s);
	}

      /**
       * JobId extractor.
       * @return the dg_jobId of the job the ad refers to
       */
      std::string dg_jobId() const 
	{
	  std::string id;
	  assert( ad -> EvaluateAttrString( "dg_jobId", id) == true );
	  return id;
	}

      /**
       * ExitCode extractor.
       * Returns the job exit code as reported by remote CEs where the job was running at.
       * This extractor returs an assertion fail if a notification option different from
       * ON_DONE and/or ON_ABORTED has/have been specified at call-back registration time.
       * @return the job exit code as reported by remote CEs.
       */
      int exit_code() const 
	{
	  int c;
	  assert( ad -> EvaluateAttrInt( "ExitCode", c ) == true );
	  return c;
	}

      /**
       * AbortReason extractor.
       * Returns a string describing the reason for the abnormal program termination 
       * at the remote CEs as reported by the JSS. This extractor should be used only if 
       * ON_ABORTED notification option has been used at call-back registration time.
       */
      std::string abort_reason() const 
	{ 
	  std::string r;
	  assert( ad -> EvaluateAttrString( "AbortReason", r) == true );
	  return r;
	}

      /** 
       * GlobusResourceContactString extractor.
       * Returns the GlobusResourceContactString of the notified JDL. 
       * This extractor should be used if and only if ON_READY notification option 
       * has been used at call-back registration time.
       * @returns the GlobusResourceContactString of the notified JDL.
       */
      std::string globus_resource_contact_string() const 
	{ 
	  std::string r;
	  assert( ad -> EvaluateAttrString( "GlobusResourceContactString", r) == true );
	  return r;
	}

      /** 
       * QueueName extractor.
       * Returns the QueueName of the notified JDL. 
       * This extractor should be used if and only if ON_READY notification option 
       * has been used at call-back registration time.
       * @return the QueueName of the notified JDL. 
       */
      std::string queue_name() const 
	{ 
	  std::string r;
	  assert( ad -> EvaluateAttrString( "QueueName", r) == true );
	  return r;
	}
  
      /////////////////////////////
      // MATCH STATUS extractors //
      /////////////////////////////
      /**
       * MatchStatus extractor.
       * Return whether the matching processed performed at the remote Resource Broker 
       * has been succesfully completed, or not. 
       * Return true on matching process success, false otherwise.
       */
      bool match_status() const
	{
	  bool s;
	  assert( ad -> EvaluateAttrBool( "MatchStatus", s ) == true );
	  return s;
	}

      /**
       * ce_match extractor.
       * @param l a reference to a vector which will contain matching CEs as reported
       * by the remote Resource Broker
       * @return whether an error as occurred, or not.
       */
      bool ce_match( std::vector<std::string>& l) const 
	{      
	  Value list_value;
	  const ExprList *expr_list;
	  if( ad -> EvaluateAttr("CE_Match", list_value) == true &&
	      list_value.IsListValue( expr_list )           == true ) {
	
	    ExprListIterator it( expr_list );
	    while( it.CurrentExpr() ) {
	      Value v;
	      std::string s;
	      if( it.CurrentValue(v) && v.IsStringValue(s) ) l.push_back( s );
	      else return false;
	      it.NextExpr();
	    }
	  }
	  else return false;
      
	  return true;
	}

      /**
       * Test if rank is reported.
       * @return return whether the rank is reported, that is WITH_RANK option has 
       * been used at call-back registration time, or not.
       */
      bool is_rank_reported() const
	{
	  return ( ad -> Lookup("CE_Rank") != NULL );
	}

      /**
       * ce_rank extractor.
       * @param l a reference to a vecotr which will contain the rank of matching CE.
       * @return whether an error as occurred, or not.
       */
      bool ce_rank( std::vector<double>& l) const 
	{      
	  Value list_value;
	  const ExprList *expr_list;
	  if( ad -> EvaluateAttr("CE_Rank", list_value) == true &&
	      list_value.IsListValue( expr_list )           == true ) {
	
	    ExprListIterator it( expr_list );
	    while( it.CurrentExpr() ) {
	      Value v;
	      double d;
	      if( it.CurrentValue(v) && v.IsRealValue(d) ) l.push_back( d );
	      else return false;
	      it.NextExpr();
	    }
	  }
	  else return false;
      
	  return true;
	}

      /**
       * Returns the list of errors occurred at the remote Resource Broker while
       * performing the match-making.
       * @param l a reference to a vector which will receive le error strings.
       * @return whether an error as occurred, or not.
       */
      bool match_errors(std::vector<string>& l) const 
	{      
	  Value list_value;
	  const ExprList *expr_list;
	  if( ad -> EvaluateAttr("MatchErrors", list_value) == true &&
	      list_value.IsListValue( expr_list )           == true ) {
	    
	    ExprListIterator it( expr_list );
	    while( it.CurrentExpr() ) {
	      Value v;
	      std::string s;
	      if( it.CurrentValue(v) && v.IsStringValue(s) ) l.push_back( s );
	      else return false;
	      it.NextExpr();
	    }
	  }
	  else return false;
	  
	  return true;
	}

      //////////////////////////////////
      // GET OUTPUT STATUS extractors //
      //////////////////////////////////  
      /**
       * Single sandboxfile check.
       * @return whether the notification refers to a single sandbox file, or not.
       */
      bool refers_to_sandboxfile() const
	{
	  return ( ad -> Lookup("SandboxFile") != NULL );
	}
 
      /**
       * Transfer status extractor.
       * @return whether the output sandbox retrieval has been entirely completed, or not. 
       */
      bool transfer_status() const 
	{    
	  bool s; 
	  assert( ad -> EvaluateAttrBool( "TransferStatus", s ) == true );
	  return s;
	}

      /**
       * Failure reason status extractor.
       * @return description of errors occurred during the output sandbox file(s) transfer.
       */
      std::string failure_reason() const 
	{    
	  std::string s;
	  ad -> EvaluateAttrString( "FailureReason", s );
	  return s;
	}

      //////////////////////////////
      // CANCEL STATUS extractors //
      //////////////////////////////
      /**
       * Returns the status of a cancel.
       * @return the cancel status.
       */
      cancel_status_t cancel_status() const
	{ 
	  int s;
	  assert( ad -> EvaluateAttrInt( "CancelStatus", s ) == true );
	  return cancel_status_t(s);
	}

    protected:
      /**
       * Constructor.
       */
      notifiedad_t() {};

    private:
      ClassAd *ad;
    };

} // namespace client
} // namespace ns
} // namespace manager  
} // namespace wms
} // namespace glite

#endif
