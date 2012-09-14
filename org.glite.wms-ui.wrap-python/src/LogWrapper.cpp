#include "LogWrapper.h"
#include "glite/wmsutils/exception/Exception.h"
#include "glite/wmsutils/jobid/JobId.h"
#include "glite/lb/consumer.h"
//#include "glite/lb/producer.h"
#include <stdio.h>
#include <iostream>
#include <string>

#define USERINTERFACE_SEED "Userinterface"
// IMPORTANT: These values must match the glite-job-submit values
#define UI_DAGTYPE 1
#define UI_JOBTYPE 0


using namespace std ;

LOG::LOG(){};
LOG::~LOG() {  } ;

// Compare two edg_wll_Event using the timestamp member
// This Method is used by std::string LOG::retrieveState
int cmp_by_timestamp(const void* first, const void* second) {
  timeval f = ((edg_wll_Event *) first)->any.timestamp;
  timeval s = ((edg_wll_Event *) second)->any.timestamp;
  if (( f.tv_sec > s.tv_sec ) ||
      (( f.tv_sec == s.tv_sec ) && ( f.tv_usec > s.tv_usec )))
    return 1;
  if (( f.tv_sec < s.tv_sec ) ||
      (( f.tv_sec == s.tv_sec ) && ( f.tv_usec < s.tv_usec )))
    return -1;
  return 0;
}

// void LOG::log_error (const std::string& err , edg_wll_Context *ctx){
void LOG::log_error (const std::string& err ){
    error = err ;
    if (error_code==0)   error_code = 1 ;
} ;
std::vector<std::string>  LOG::get_error () {

  std::vector<std::string> result ;
  
  // BUG 25250: PATCH TO AVOID COMPATIBILITY PROBLEMS WITH PYTHN CLI 
  result.push_back(error);
  result.push_back(error);
  // BUG 25250: PATCH TO AVOID COMPATIBILITY PROBLEMS WITH PYTHN CLI 
  
  error = "" ;

  return result;
};


/** Retrieve the representing state string of the n-step job for the specified jobid*/
std::string LOG::retrieveState ( const std::string& jobid_str , int step ){
  int                 i, cnt, error;
  edg_wlc_JobId       jobid;
  edg_wll_Event       *events = NULL;
  edg_wll_QueryRec    jc[2];
  edg_wll_QueryRec    ec[2];
  // parse the jobID string
  error_code= 0 ;
  error = edg_wlc_JobIdParse(jobid_str.c_str(), &jobid);
  if ( error ){
      log_error ( "JobState::getStateFromLB error from edg_wlc_JobIdParse");
      return "" ;
  }

  memset(jc,0,sizeof jc);
  memset(ec,0,sizeof ec);
  // job condition: JOBID = jobid
  jc[0].attr = EDG_WLL_QUERY_ATTR_JOBID;
  jc[0].op = EDG_WLL_QUERY_OP_EQUAL;
  jc[0].value.j =jobid ;
  // event condition: Event type = CHKPT
  ec[0].attr = EDG_WLL_QUERY_ATTR_EVENT_TYPE;
  ec[0].op = EDG_WLL_QUERY_OP_EQUAL;
  ec[0].value.i = EDG_WLL_EVENT_CHKPT;
  error = edg_wll_QueryEvents( ctx, jc, ec, &events );
  if ( error == ENOENT ){  // no events found
   log_error   ( "No events found: ENOENT");
   return "" ;
  }if ( error ) {
     log_error( "Query failed");
    return "";
  }
  for ( cnt=0; events[cnt].type; cnt++ ); // counts the number of events
  if ( !cnt ){
    log_error ("Empty Events vector returned");
    return "";
  }
  // sort the events vector using the timestamp
  qsort(events, cnt, sizeof(edg_wll_Event), &cmp_by_timestamp);
  // the last state in the array is the most recent
  // cout << "Job::getState > Retrieving state and free events...   " << endl << flush ;
  if ((int)step>= cnt){
    log_error ( "Number of step bigger then chkpt logged events");
    return "" ;
  }
  string state( events[cnt-1-step].chkpt.classad );
  for ( i = 0; i < cnt; i++)
    edg_wll_FreeEvent( events+i );
  // free(events);   //  TBD is to be done?
  return state ;

} ;

void LOG::init ( const std::string& ns ) {
    error ="" ;
    error_code= 0 ;
    if ( edg_wll_InitContext( &ctx ) ) log_error ( "Unable to Initialise LB context") ;
    else if (edg_wll_SetParam( ctx, EDG_WLL_PARAM_SOURCE, EDG_WLL_SOURCE_USER_INTERFACE ) ) log_error ( "Unable to set LB source parameter" ) ;
    if ( ! getenv ( GLITE_LB_LOG_DESTINATION) ){
       setenv ( GLITE_LB_LOG_DESTINATION   ,  ns.c_str()  , 0 );
       if (edg_wll_SetParamString( ctx, EDG_WLL_PARAM_DESTINATION, ns.c_str() ) ) log_error ( "Unable to set LB destination parameter") ;
    }
};

void LOG::regist( const std::string& jobid , const std::string& jdl , const std::string& ns ) {
   error ="" ;
   error_code= 0 ;
   try{
     glite::wmsutils::jobid::JobId jid ( jobid );
     error_code =  edg_wll_RegisterJobSync ( ctx , jid.getId() ,  EDG_WLL_JOB_SIMPLE , jdl.c_str() , ns.c_str() , 0 ,  NULL, NULL)   ;
     if ( error_code !=0 ){
        char error_message [1024];
        char *msg, *dsc ;
        edg_wll_Error( ctx , &msg , &dsc ) ;
        sprintf ( error_message , "%s%s%s%s%s%s%s%s%s", "Unable to Register the Job:\n", jid.toString().c_str(),"\nto the LB logger at: ",
        getenv ( GLITE_LB_LOG_DESTINATION) , "\n" , msg , " (", dsc , ")" )  ;
        log_error ( error_message  ) ;
     }
   }catch (exception &exc){
      log_error ("Unable to create the JobId:" + string (exc.what() ) );
   }
   };

void LOG::logSync ( const std::string& state, const std::string& currentStep ) {
       error ="" ;
       error_code= 0 ;
       if ( edg_wll_LogEventSync( ctx , EDG_WLL_EVENT_CHKPT , EDG_WLL_FORMAT_CHKPT , currentStep.c_str() , state.c_str()  )  ){
         if (edg_wll_LogAbort ( ctx ,  state.c_str() )) cerr << "\n\n\nLB - Warning   edg_wll_LogTransferFAIL! ! ! "<<flush;
         char error_message [1024];
         char *msg, *dsc ;
         edg_wll_Error( ctx , &msg , &dsc ) ;
         sprintf ( error_message , "%s%s%s%s%s%s%s", "Unable to log the sync event to LB logger at: ",
         getenv ( GLITE_LB_LOG_DESTINATION) , "\n" , msg , " (", dsc , " )" )  ;
         log_error ( error_message ) ;
       }
};

void LOG::log_start (const std::string& host , int port , const std::string& jdl ){
   char str_addr [1024];
   error_code= 0 ;
   sprintf (str_addr , "%s%s%d" , host.c_str(),":", port );
   if (edg_wll_LogTransferSTART( ctx, EDG_WLL_SOURCE_NETWORK_SERVER , host.c_str(), str_addr , jdl.c_str(), "", "" )){
       char error_message [1024];
       char *msg, *dsc ;
       edg_wll_Error( ctx , &msg , &dsc ) ;
       sprintf ( error_message , "%s%s%s%s%s%s%s","Unable to perform edg_wll_LogTransferSTART at: ",
       getenv ( GLITE_LB_LOG_DESTINATION) , "\n" , msg , " (", dsc , " )" )  ;
       log_error ( error_message ) ;
   }
  }
void LOG::log_tag (const std::string& attrName  , const std::string& attrValue ){
	error_code= 0 ;
	if (      edg_wll_LogUserTag( ctx,  attrName.c_str() ,  attrValue.c_str()  )   ){
		char error_message [1024];
		char *msg, *dsc ;
		edg_wll_Error( ctx , &msg , &dsc ) ;
		sprintf ( error_message , "%s%s%s%s%s%s%s","Unable to perform   edg_wll_LogUserTag  at: ",
		getenv ( GLITE_LB_LOG_DESTINATION) , "\n" , msg , " (", dsc , " )" )  ;
		log_error ( error_message ) ;
	}
}
void LOG::log_jobid(const std::string& j ){
	glite::wmsutils::jobid::JobId jobid (j) ;
	edg_wll_SetLoggingJob(  ctx , jobid.getId() ,NULL, EDG_WLL_SEQ_NORMAL) ;
}
void LOG::free (){
	error_code= 0 ; 
	edg_wll_FreeContext( ctx ) ;
}
std::string LOG::getSequence(){    
	error_code= 0 ; return edg_wll_GetSequenceCode( ctx ) ; 
}
void LOG::log_listener( const std::string& jobid, const std::string& host , int port  ) {
   error_code= 0 ;
   if (jobid!= "")
     try{
        glite::wmsutils::jobid::JobId jid ( jobid );
        if ( edg_wll_SetLoggingJob(ctx , jid.getId() , NULL , EDG_WLL_SEQ_DUPLICATE) )
               log_error ( "Unable to perform edg_wll_SetLoggingJob LB api to " + string ( getenv ( GLITE_LB_LOG_DESTINATION) )  ) ;
        return ;
     } catch (exception &exc) {    log_error ( "Unable parse JobId: " + jobid ) ;     return ;     }
   if (edg_wll_LogListener(  ctx , "InteractiveListener", host.c_str(),   (uint16_t) port)){
         if (edg_wll_LogAbort ( ctx , "edg_wll_LogListener method failed"   )) cerr << "\n\n\nLB - Warning  edg_wll_LogAbort Failed  ! ! ! "<<flush;
         log_error ( "Unable to perform edg_wll_LogListener LB api to " + string ( getenv ( GLITE_LB_LOG_DESTINATION) )  ) ;
   }
}
void LOG::log_tr_ok ( const std::string& jdl , const std::string&  host , int port ){
	char str_addr [1024];
	error_code= 0 ;
	sprintf (str_addr , "%s%s%d" , host.c_str(),":", port );
	if ( edg_wll_LogTransferOK( ctx , EDG_WLL_SOURCE_NETWORK_SERVER , host.c_str(), str_addr , jdl.c_str(), "","") ){
	   char error_message [1024];
	   char *msg, *dsc ;
	   edg_wll_Error( ctx , &msg , &dsc ) ;
	   sprintf ( error_message , "%s%s%s%s%s%s%s","Unable to perform   edg_wll_LogTransferOK at:",
	   getenv ( GLITE_LB_LOG_DESTINATION) , "\n" , msg , " (", dsc , " )" )  ;
	   log_error (  error_message ) ;
	}
}
void LOG::log_tr_fail ( const std::string& jdl , const std::string&  host , int port, const char* exc ){
	char str_addr [1024];
	error_code= 0 ;
	sprintf (str_addr , "%s%s%d" , host.c_str(),":", port );
	if (edg_wll_LogTransferFAIL(ctx,
		EDG_WLL_SOURCE_NETWORK_SERVER ,
		host.c_str(),
		str_addr ,
		jdl.c_str(), exc,"")) cerr << "\n\n\nLB - Warning edg_wll_LogTransferFAIL ! ! ! "<<flush;
	if (edg_wll_LogAbort ( ctx ,  exc )) cerr << "\n\n\nLB - Warning   edg_wll_LogTransferFAIL! ! ! "<<flush;
}
// DagAd New feature implementation:
std::vector<std::string>  LOG::regist_dag ( const std::vector<std::string>& jdls, const std::string& jobid ,const std::string& jdl , int length , const std::string& ns, int regType ){
	vector <string> jobids ;
	error_code= false ;
	//  array of subjob ID's
	edg_wlc_JobId* subjobs = NULL ;
	// Register The Dag
	edg_wlc_JobId id = NULL;
	try{
		glite::wmsutils::jobid::JobId jid (jobid );
		id = jid.getId();
	} catch (exception &exc) {  log_error ( "Unable parse JobId: " + jobid ) ;       return jobids ;  }
	if (edg_wll_RegisterJobSync(ctx,id,(regType==UI_DAGTYPE)?EDG_WLL_REGJOB_DAG:EDG_WLL_REGJOB_PARTITIONED, jdl.c_str(),
		ns.c_str() ,    length,   USERINTERFACE_SEED,   &subjobs  ) ){
		char error_message [1024];
		char *msg, *dsc ;
		edg_wll_Error( ctx , &msg , &dsc ) ;
		sprintf ( error_message , "%s%s%s%s%s%s%s","Unable to perform  edg_wll_RegisterJobSync at: ",
		getenv ( GLITE_LB_LOG_DESTINATION) , "\n" , msg , " (", dsc , " )" )  ;
		log_error ( error_message ) ;
		return jobids ;
	}
	// Fill the jobids value
	for ( unsigned int i = 0 ; i < length ; i++ ){
		// Fill vector of jobids strings
		jobids.push_back(  string (    edg_wlc_JobIdUnparse( subjobs[i]  )  )  ) ;
	}
	// Register the Sub Jobs (needed when it's a pure dag (not a partitioner)
	if (jdls.size()>0){
		// array of Jdls
		char **jdls_char , **zero_char ;
		jdls_char = (char**) malloc(sizeof(char*) * (jdls.size()+1));
		zero_char = jdls_char ;
		jdls_char[jdls.size()] = NULL;
		vector<string>::const_iterator iter ;
		int i = 0 ;
		for (  iter = jdls.begin() ; iter!=jdls.end() ; iter++, i++){
			*jdls_char = (char*) malloc ( iter->size() + 1 ) ;
			sprintf (*jdls_char,"%s",iter->c_str());
			jdls_char++;
		}
		if ( edg_wll_RegisterSubjobs (ctx, id, zero_char,ns.c_str(),subjobs)){
			char error_message [1024];
			char *msg, *dsc ;
			edg_wll_Error( ctx , &msg , &dsc ) ;
			sprintf ( error_message , "%s%s%s%s%s%s%s","Unable to perform   edg_wll_RegisterSubjobs  at: ",
			getenv ( GLITE_LB_LOG_DESTINATION) , "\n" , msg , " (", dsc , " )" )  ;
			log_error ( error_message ) ;
			return jobids ;
		}
		// cout << "register Sub Jobs done properly" << endl ;
		std::free(zero_char);
	}
	log_jobid(jobid);
	return jobids ;
}


std::vector<std::string>  LOG::generate_sub_jobs( const std::string& jobid, int res_num){
	vector <string> jobids ;
	error_code= false ;
	edg_wlc_JobId id = NULL;
	edg_wlc_JobId* subjobs = NULL;
	try{
		id = glite::wmsutils::jobid::JobId (jobid ).getId();
	}catch (exception &exc) {
		log_error("Unable parse JobId: "+jobid);return jobids;
	}
	edg_wll_GenerateSubjobIds(ctx, id, res_num, USERINTERFACE_SEED, &subjobs);
	for (int i = 0; i < res_num; i++) {
		jobids.push_back(string(edg_wlc_JobIdUnparse(subjobs[i])));
	}
	return jobids;
}






