#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <iostream>
#include <math.h> // calculate mathematical power
#include <unistd.h> // getpid method
#include "org_glite_wmsui_apij_Api.h"
// Network Server:
#include "NSClient.h"
#include "glite/wms/common/logger/common.h"
#include "glite/wmsutils/exception/Exception.h"
// LB:
#include "glite/lb/consumer.h" // TBD is possible?
#include "glite/lb/producer.h"
#include "glite/lb/JobStatus.h"
#include "glite/lb/Job.h"
#include "glite/lb/Notification.h"
#include "glite/wmsutils/jobid/JobId.h"
#include "glite/wmsutils/jobid/manipulation.h"  //replace jobid name
#include "glite/wms/partitioner/Partitioner.h"  //Partitioner

// Ad
#include "glite/jdl/ExpDagAd.h"
#include "glite/wmsutils/classads/classad_utils.h"
// Dgas Authorisation include files
#include "glite/dgas/hlr-clients/job_auth/jobAuthClient.h"
#define EDG_WLU_SOURCE_NS "NetworkServer"
#define GLITE_LB_LOG_DESTINATION "EDG_WL_LOG_DESTINATION" //TBD change when LB people provide new naming

#define NS_CTX 0
#define LB_CTX 1
#define DAG_CTX 2

using namespace std ;


/*  static variables */
pthread_mutex_t  mutex;
vector <edg_wll_Context* > lbVect ;
// DagAd Wrapper
vector <glite::jdl::ExpDagAd* > dagVect ;
glite::lb::Job lbJob ;
string nsHost ;
int nsPort ;
int nsLevel ;

/*************************************************************************
non-native methods:
*************************************************************************/

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

	const char* error_message ( edg_wll_Context ctx , const char* api  ){
		char* error_message =(char*) malloc ( 1024 ) ;
		char *msg, *dsc ;
		edg_wll_Error(  ctx , &msg , &dsc ) ;
		sprintf ( error_message , "%s%s%s%s%s%s%s", api,
		getenv ( GLITE_LB_LOG_DESTINATION) , "\n" , msg , " (" , dsc , " )" )  ;
		return error_message ;
	} ;

	void lock() {     pthread_mutex_lock( &mutex);      /* LOCK RESOURCES    */ };
	void unlock(){   pthread_mutex_unlock( &mutex);  /*UNLOCK RESOURCES*/ };

	void log_error (JNIEnv *env , string error) {
		// jclass newExcCls = env->FindClass("glite/wmsui/api/ApiException");
		jclass newExcCls = env->FindClass("java/lang/RuntimeException");
		if (newExcCls == 0) /* Unable to find the new exception class, give up */ return ;
		if ( error.empty() )
			env->ThrowNew( newExcCls , "(No message available)" );
		else
			env->ThrowNew( newExcCls , error.c_str() );
	}

	int getCtx ( JNIEnv *env , jobject obj , int value) {
		// TBD can be done statically?
		jclass cls = env->GetObjectClass(obj);
		jmethodID getx = env->GetMethodID ( cls , "getCtx" , "(I)I" ) ;
		if (getx==0) /** then */ log_error (env , "Fatal Error: Unable to find Api.getCtx method\n" ) ;
		return env->CallIntMethod(obj, getx, value) ;
	};

	void logUserTags( classad::ClassAd* userTags , const edg_wll_Context &ctx ){
		vector< pair< string, classad::ExprTree *> > vect ;
		classad::Value val ;
		string attrValue ;
		userTags->GetComponents( vect ) ;
		for (unsigned int i = 0 ; i< vect.size() ; i++){
			if ( userTags->EvaluateExpr( vect[i].second, val ) ) if  ( val.IsStringValue (attrValue)  )
				if (      edg_wll_LogUserTag( ctx,  (vect[i].first).c_str() ,  attrValue.c_str()  )     )
					cerr << "JNI edg_wll_LogUserTag error TBD" << endl ;
		}
	}




	/**
	* Method : LOAD STATUS
	* This method Load the c++ JobStatus into a Java JobStatus instance **/
	void loadStatus(  JNIEnv *env, const jobject &obj, const glite::lb::JobStatus  &status ){
		//Defining non-native methods
		using namespace glite::lb ;
		jclass cls = env->GetObjectClass(obj);
		jmethodID appStr = env->GetMethodID ( cls , "appendString" ,  "(ILjava/lang/String;)V" ) ;
		if (appStr==0)   log_error (env , "Fatal Error while retrieving status information\nUnable to find Api.appStr method\n" ) ;
		jmethodID appInt = env->GetMethodID ( cls , "appendInt" ,  "(II)V" ) ;
		if (appInt==0)   log_error (env , "Fatal Error while retrieving status information.\nUnable to find Api.appInt method\n"   ) ;

		// Iterate over attributes
		std::vector<pair<JobStatus::Attr, JobStatus::AttrType> > attrList = status.getAttrs();
		int STATUS = 35;
		env->CallVoidMethod(obj, appStr, (jint) STATUS , env->NewStringUTF( status.name().c_str() ) );
		int STATUS_CODE = 36;
		env->CallVoidMethod(obj, appInt, (jint) STATUS_CODE , (jint)status.status );
		char time [1024];

		for (unsigned i=0; i < attrList.size(); i++ ) {
			switch (attrList[i].second) {
				case JobStatus::INT_T :{
					env->CallVoidMethod(obj, appInt,  attrList[i].first , (jint) status.getValInt(attrList[i].first) );
				}
				break;
				case JobStatus::BOOL_T    :{
					env->CallVoidMethod(obj, appInt,  attrList[i].first , (jint)  status.getValBool(attrList[i].first) );
				}
				break;
				case JobStatus::STRING_T  :{
					env->CallVoidMethod(obj, appStr,   attrList[i].first , env->NewStringUTF( status.getValString(attrList[i].first).c_str() ) );
				}
				break;
				case JobStatus::JOBID_T    :{
					if(((glite::wmsutils::jobid::JobId)status.getValJobId(attrList[i].first)).isSet())
						env->CallVoidMethod(obj, appStr,  attrList[i].first , env->NewStringUTF( status.getValJobId(attrList[i].first).toString().c_str() ) ) ;
				}
				break;
				case JobStatus::STSLIST_T    :{ }
				break;
				case JobStatus::STRLIST_T    :{
					std::vector<std::string> v = status.getValStringList(attrList[i].first);
					for(unsigned int j=0; j < v.size(); j++){
						env->CallVoidMethod(obj, appStr,attrList[i].first , env->NewStringUTF( v[j].c_str() ) );
					}
				}
				break;
				case JobStatus::TAGLIST_T    :{
					std::vector<std::pair<std::string,std::string> >  v = status.getValTagList(   attrList[i].first   ) ;
					string result = "";
					if (v.size() >0)  result+= "[  ";
					for (unsigned int j = 0 ; j < v.size() ; j++ ){
						result += v[j].first  +"=\"" + v[j].second +"\";"  ;
					}
					if (v.size() >0) {
						result += " ]" ;
						env->CallVoidMethod( obj, appStr,  attrList[i].first ,   env->NewStringUTF( result.c_str() )  );
					}
				break;
				}
				case JobStatus::INTLIST_T    :{
					std::vector<int> v = status.getValIntList(attrList[i].first);
					for(unsigned int j=0; j < v.size(); j++){
						sprintf (time , "%d%s" , v[j] , ".0 s") ;
						env->CallVoidMethod(obj, appStr,attrList[i].first , env->NewStringUTF( time ) );
					}
				}
				break;
				case JobStatus::TIMEVAL_T    :{
					timeval t = status.getValTime(attrList[i].first);
					sprintf (time , "%d%s%d%s" ,(int)t.tv_sec , ".", (int)t.tv_usec," s") ;
					env->CallVoidMethod(obj, appStr,  attrList[i].first , env->NewStringUTF( time ) );
				}
				break;
				default : /* something is wrong */
					cerr << "\n\nFATAL ERROR!!! Something has gone bad for " << status.getAttrName(attrList[i].first) << flush;
				break;
			}
		}  // end For
		// Sub Jobs Implementation recoursive CALL
		std::vector<JobStatus> v = status.getValJobStatusList(  JobStatus::CHILDREN_STATES );
		for(unsigned int i=0; i < v.size(); i++)  loadStatus (env, obj, v[i]  );
	}





/*************************************************************************
STATIC  methods:
*************************************************************************/
	/*
	* Class:     edg_workload_userinterface_jclient_Api
	* Method:    getPid
	* Signature: ()I
	*/
	JNIEXPORT jint JNICALL Java_org_glite_wmsui_apij_Api_getPid
	(JNIEnv *env, jclass){  return getpid();    }

	/*
	* Class:     edg_workload_userinterface_jclient_Api
	* Method:    shadow
	* Signature: (Ljava/lang/String;)I
	*/
	JNIEXPORT jint JNICALL Java_org_glite_wmsui_apij_Api_shadow
	(JNIEnv *env, jclass, jstring unique){
		const char *nm = env->GetStringUTFChars(unique, 0);
		int result = system( nm  ) ;
		env->ReleaseStringUTFChars(unique, nm);
		return result ;
	}

	/*
	* Class:     edg_workload_userinterface_jclient_Api
	* Method:    getEnv
	* Signature: (Ljava/lang/String;)Ljava/lang/String;
	*/
	JNIEXPORT jstring JNICALL Java_org_glite_wmsui_apij_Api_getEnv
	(JNIEnv *env, jclass, jstring name ){
		const char *nm = env->GetStringUTFChars(name, 0);
		char* value = getenv (nm) ;
		env->ReleaseStringUTFChars(name, nm);
		return env->NewStringUTF( value  );
	}

	/*
	* Class:     edg_workload_userinterface_jclient_Api
	* Method:    getUid
	* Signature: ()I
	*/
	JNIEXPORT jint JNICALL Java_org_glite_wmsui_apij_Api_getUid
	(JNIEnv *env, jclass){   return  getuid() ;    }

	/*
	* Class:     edg_workload_userinterface_jclient_Api
	* Method:    setEnv
	* Signature: (Ljava/lang/String;Ljava/lang/String;)V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_setEnv
	(JNIEnv *env, jclass, jstring name, jstring value ){
		const char *nm = env->GetStringUTFChars(name, 0);
		const char *vl = env->GetStringUTFChars(value, 0);
		unsetenv(nm) ;
		setenv( nm , vl, 0);
		env->ReleaseStringUTFChars(name, nm);
		env->ReleaseStringUTFChars(value, vl);
	}

        /*
         * Class:     edg_workload_userinterface_jclient_Api
         * Method:    unsetEnv
         * Signature: (Ljava/lang/String;)V
         */
         JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_unsetEnv
         (JNIEnv *env, jclass, jstring name){
                 const char *nm = env->GetStringUTFChars(name, 0);
                 unsetenv(nm) ;
                 env->ReleaseStringUTFChars(name, nm);
         }

	/*
	* Class:     edg_workload_userinterface_jclient_Api
	* Method:    initialise
	* Signature: ()V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_initialise
	(JNIEnv *env, jclass ){
		/*
		edg_wlc_SSLInitialization();
		if ( globus_module_activate(GLOBUS_COMMON_MODULE) != GLOBUS_SUCCESS )
			log_error (env , "Unable to use safe multi threading for Open SSL" );
		else if (edg_wlc_SSLLockingInit() != 0)
			log_error (env , "Unable to use safe multi threading for Open SSL" );
		*/
	}

	/*
	* Class:     edg_workload_userinterface_jclient_Api
	* Method:    genetareId
	* Signature: (Ljava/lang/String;I)Ljava/lang/String;
	*/
	JNIEXPORT jstring JNICALL Java_org_glite_wmsui_apij_Api_generateId
	(JNIEnv *env, jclass , jstring lbHost, jint lbPort){
		const char *host = env->GetStringUTFChars( lbHost, 0);
		try{
			glite::wmsutils::jobid::JobId jobid ;
			jobid.setJobId( host,  lbPort );
			env->ReleaseStringUTFChars( lbHost, host);
			return env->NewStringUTF( jobid.getUnique().c_str() ) ;
		}catch(glite::wmsutils::exception::Exception &exc){  log_error (env , exc.what() ) ;
		}catch(exception &exc){  log_error (env , exc.what() ) ;
		}catch (...){ log_error (env, "Fatal Error: Unpredictalbe exception thrown by JNI wrapper");    }
	}

/*************************************************************************
NETWORK SERVER  methods:
*************************************************************************/


	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    ns_init
	* Signature: (Ljava/lang/String;II)V
	* This Metdhos is synchronized. no more lock unlock are needed
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_ns_1init
	(JNIEnv *env, jobject obj, jstring host , jint port , jint  logLevel){
		const char *ns = env->GetStringUTFChars( host, 0);
		nsHost = string (ns) ;   nsPort = port ;     nsLevel = logLevel ;
		env->ReleaseStringUTFChars( host , ns);
	}

	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    ns_submit
	* Signature: (Ljava/lang/String;Ljava/lang/String;I)V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_ns_1submit
	(JNIEnv *env, jobject obj, jstring  jdl , jstring jnsHost, jint jnsPort){
		const char *jd = env->GetStringUTFChars(jdl, 0);
		const char *ct = env->GetStringUTFChars(jnsHost, 0);
		char str_port [1024];
		bool dagB = true ;
		/**
			Workaround: NS should provide a single submission method
		*/
		if ( jnsPort >0) dagB=false ;
		else jnsPort = -( jnsPort ) ;
		// sprintf (str_port , "%d" , jnsPort );
		sprintf (str_port , "%s%s%d" , nsHost.c_str(),":",nsPort );
		edg_wll_Context ctx = *lbVect[getCtx(env, obj,LB_CTX)]  ;
		try{
lock() ;
		glite::wms::manager::ns::client::NSClient nsClient ( ct , jnsPort , ( glite::wms::common::logger::level_t)nsLevel ) ;
		/**
			Workaround: NS should provide a single submission method
			UI currently switches between DAG and JOB
		*/
		if (dagB) {
			nsClient.dagSubmit( jd  );
		}
		else nsClient.jobSubmit( jd  );
		if (edg_wll_LogTransferOK(ctx, EDG_WLL_SOURCE_NETWORK_SERVER , ct, str_port , jd, "","") )
			cerr << error_message (ctx , "edg_wll_LogTransferOK Failed") << endl ;
unlock();
		}catch (exception &exc){    unlock();      /*Call the dgLog Transfer method  for failure (No Thread Safe Method)*/
lock() ;
		if (edg_wll_LogTransferFAIL(ctx ,  EDG_WLL_SOURCE_NETWORK_SERVER , ct, str_port , jd, exc.what(),""))
			cerr << error_message(ctx , "Wanrning: edg_wll_LogTransferFAIL Failed ") << endl ;
		if (edg_wll_LogAbort ( ctx  , exc.what() ))
			cerr <<  error_message(ctx, "Warning: edg_wll_LogAbort Failed") << endl ;
		// edg_wll_FreeContext(*lbVect[getCtx(env, obj,LB_CTX)] ) ; // needed before throwing the exception sure? TBD
unlock() ;
		log_error (env , "Unable to submit the job: " + string (exc.what() )  ) ;
		}catch (...){ unlock();   log_error (env, "Fatal Error: Unpredictalbe exception thrown by JNI wrapper");    }
		env->ReleaseStringUTFChars( jdl, jd);
		env->ReleaseStringUTFChars( jnsHost, ct);
	}

	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    ns_purge
	* Signature: (Ljava/lang/String;)V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_ns_1purge
	(JNIEnv *env, jobject obj, jstring jobid){
	const char *id = env->GetStringUTFChars(jobid, 0);
		try{
			glite::wmsutils::jobid::JobId jid ( id )  ;
			glite::wms::manager::ns::client::NSClient nsClient ( nsHost , nsPort,  ( glite::wms::common::logger::level_t)nsLevel ) ;
			nsClient.jobPurge( id )  ;
		}catch(glite::wmsutils::exception::Exception &exc){  log_error (env , exc.what() ) ;
		}catch(exception &exc){  log_error (env , exc.what() ) ;
		}catch (...){ log_error (env, "Fatal Error: Unpredictalbe exception thrown by JNI wrapper");    }
		env->ReleaseStringUTFChars( jobid , id);
	}

	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    ns_cancel
	* Signature: (Ljava/lang/String;)I
	*/
	JNIEXPORT jint JNICALL Java_org_glite_wmsui_apij_Api_ns_1cancel
	(JNIEnv *env, jobject obj, jstring jobid){
		const char *id = env->GetStringUTFChars(jobid, 0);
		try{
			glite::wmsutils::jobid::JobId jid ( id )  ;
			glite::wms::manager::ns::client::NSClient nsClient ( nsHost , nsPort,  ( glite::wms::common::logger::level_t)nsLevel ) ;
			nsClient.jobCancel( list<string> ( 1   ,  id) );
		}catch(exception &exc){  log_error (env , exc.what() ) ;
		}catch (...){ log_error (env, "Fatal Error: Unpredictalbe exception thrown by JNI wrapper");    }
		env->ReleaseStringUTFChars( jobid , id);
	}

	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    ns_get_root
	* Signature: (Ljava/lang/String;)Ljava/lang/String;
	*/
	JNIEXPORT jstring JNICALL Java_org_glite_wmsui_apij_Api_ns_1get_1root
	(JNIEnv *env, jobject obj, jstring jobId ){
	const char *id = env->GetStringUTFChars(jobId, 0);
		try{
			glite::wms::manager::ns::client::NSClient nsClient ( nsHost , nsPort,  ( glite::wms::common::logger::level_t)nsLevel  ) ;
			string path =  nsClient.getSandboxRootPath() ;
			string sep = "/" ;
			path += sep + glite::wmsutils::jobid::to_filename(glite::wmsutils::jobid::JobId(id) )  + sep + "output" + sep  ;
			return env->NewStringUTF( path.c_str() ) ;
		}catch (exception &exc){ log_error (env ,  exc.what() ) ;
		}catch (...){ log_error (env, "Fatal Error: Unpredictalbe exception thrown by JNI wrapper");    }
		env->ReleaseStringUTFChars( jobId, id);
		return env->NewStringUTF (""); //Some error occurred or the Api are faked
	}

	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    ns_multi
	* Signature: ()V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_ns_1multi
	(JNIEnv *env, jobject obj){
		jclass cls = env->GetObjectClass(obj);
		jmethodID appStr = env->GetMethodID ( cls , "appendString" ,  "(ILjava/lang/String;)V" ) ;
		try{
			if (appStr==0)    log_error (env , "Fatal Error while retrieving multi attribute info. Unable to find Api.appStr method\n" ) ;
			glite::wms::manager::ns::client::NSClient nsClient ( nsHost , nsPort ,  ( glite::wms::common::logger::level_t)nsLevel  ) ;
			vector<string> multi ;
			nsClient.getMultiattributeList(multi) ;
			for  (  vector<string>::iterator it  =  multi.begin() ; it !=multi.end() ; it++ )
				env->CallVoidMethod(obj, appStr, (jint) 0 , env->NewStringUTF( it->c_str() ) );
		}catch (exception &exc){ log_error (env ,  exc.what() ) ;
		}catch (...){ log_error (env, "Fatal Error: Unpredictalbe exception thrown by JNI wrapper");    }
	}

	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    ns_output
	* Signature: (Ljava/lang/String;)V
	*/

	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_ns_1output
	(JNIEnv *env, jobject obj, jstring   jobid){
		jclass cls = env->GetObjectClass(obj);
		jmethodID appStr = env->GetMethodID ( cls , "appendString" ,  "(ILjava/lang/String;)V" ) ;
		try{
			const char *jid = env->GetStringUTFChars(jobid, 0);
			if (appStr==0)    log_error (env , "Fatal Error while retrieving outputsandbox file. Unable to find Api.appStr method\n" ) ;
			glite::wms::manager::ns::client::NSClient nsClient ( nsHost , nsPort ,  ( glite::wms::common::logger::level_t)nsLevel  ) ;
			vector<string> outputfiles ;
			nsClient.getOutputFilesList ( jid ,outputfiles  ) ;
			for  (  vector<string>::iterator it  =   outputfiles.begin() ; it !=outputfiles.end() ; it++ )
				env->CallVoidMethod(obj, appStr, (jint) 0 , env->NewStringUTF( it->c_str() ) );
		}catch (exception &exc){ log_error (env ,  exc.what() ) ;
		}catch (...){ log_error (env, "Fatal Error: Unpredictalbe exception thrown by JNI wrapper");    }
	};


	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    ns_match
	* Signature: (Ljava/lang/String;I)V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_ns_1match
	(JNIEnv *env, jobject obj, jstring jobad){
		const char *id = env->GetStringUTFChars( jobad, 0);
		jclass cls = env->GetObjectClass(obj);
		jmethodID appStr = env->GetMethodID ( cls , "appendString" ,  "(ILjava/lang/String;)V" ) ;
		if (appStr==0)
		log_error (env , "Fatal Error while retrieving matching ce for the job: "+string(id) +".\nUnable to find Api.appStr method\n" ) ;
		try{
			// vector<string> resources ;
			std::vector<string> resources ;
			glite::wms::manager::ns::client::NSClient nsClient ( nsHost , nsPort,  ( glite::wms::common::logger::level_t)nsLevel  ) ;
			nsClient.listJobMatch( string ( id ) , resources )  ;
			for (unsigned int i  = 0 ; i< resources.size() ; i++){
				//env->CallVoidMethod(obj, appStr, (int)(resources[i].second)  , env->NewStringUTF( resources[i].first.c_str() ) );
                                env->CallVoidMethod(obj, appStr, i  , env->NewStringUTF( resources[i].c_str() ) );
				// env->CallVoidMethod(obj, appInt, i  , resources[i].second ) ;
			}
		}catch(exception &exc){  log_error (env , exc.what() ) ;
		}catch (...){ log_error (env, "Fatal Error: Unpredictalbe exception thrown by JNI wrapper");    }
		env->ReleaseStringUTFChars( jobad, id);
	}

	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    ns_free
	* Signature: ()V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_ns_1free
	(JNIEnv *env, jobject obj){     cerr << "JNI ns_free: do nothing (Deprecated)" << endl ; /*  Deprecated */              }




/*************************************************************************
LOGGING methods:
*************************************************************************/


	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    lb_init
	* Signature: (Ljava/lang/String;)V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_lb_1init
	(JNIEnv *env, jobject obj, jstring  nsAddr ){
		const char *lb = env->GetStringUTFChars(nsAddr, 0);
		jclass cls = env->GetObjectClass(obj);
		jmethodID appInt = env->GetMethodID ( cls , "appendInt" ,  "(II)V" ) ;
		if (appInt==0)
			log_error (env , "Fatal Error: unable to find Api.appInt method\n" ) ;
		edg_wll_Context * ctx =  (edg_wll_Context*) malloc (sizeof(edg_wll_Context*) ) ;
		env->CallVoidMethod(obj, appInt, LB_CTX , (jint) lbVect.size() );
		lbVect.push_back( ctx) ;
		if ( edg_wll_InitContext( ctx ) ) log_error (env , "Unable to Initialise LB context") ;
		if (edg_wll_SetParam( *ctx, EDG_WLL_PARAM_SOURCE, EDG_WLL_SOURCE_USER_INTERFACE ) ) log_error (env , "Unable to set LB source parameter" ) ;
		if ( ! getenv ( GLITE_LB_LOG_DESTINATION) ){
			if (edg_wll_SetParamString( *ctx, EDG_WLL_PARAM_DESTINATION, lb ) ) log_error (env , "Unable to set LB destination parameter") ;
			char edg_log_dest [1024];
			int DEF_LOG_PORT = 9002 ;
			sprintf (  edg_log_dest , "%s%s%d", lb , ":" , DEF_LOG_PORT ) ;
			unsetenv( GLITE_LB_LOG_DESTINATION  ) ;
			setenv (  GLITE_LB_LOG_DESTINATION, edg_log_dest , 0 );
		}
		env->ReleaseStringUTFChars( nsAddr, lb);
	}

	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    lb_register
	* Signature: (Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_lb_1register
	(JNIEnv *env, jobject obj, jstring jobId, jstring jdl, jstring nsAddr){
		const char *jd = env->GetStringUTFChars(jdl, 0);
		const char *id = env->GetStringUTFChars(jobId, 0);
		const char *ns = env->GetStringUTFChars(nsAddr, 0);
		try{
			glite::wmsutils::jobid::JobId jid ( id );
lock();
			if (edg_wll_RegisterJobSync (*lbVect[getCtx(env, obj,LB_CTX)],
				jid.getId(),EDG_WLL_JOB_SIMPLE,jd,ns,0,NULL,NULL)){
				char error_message [1024];
				char *msg, *dsc ;
				edg_wll_Error(   *lbVect[getCtx(env, obj,LB_CTX)],&msg , &dsc ) ;
				sprintf ( error_message , "%s%s%s%s%s%s%s%s%s", "Unable to Register the Job:\n",
					jid.toString().c_str() ,"\nto the LB logger at: ",
				getenv(GLITE_LB_LOG_DESTINATION),"\n",msg," (",dsc," )");
				log_error (env , error_message) ;
			}
unlock() ;
		}catch(exception &exc){  log_error (env , exc.what() ) ;
		}catch (...){ log_error (env, "Fatal Error: Unpredictalbe exception thrown by JNI wrapper") ;   }
		env->ReleaseStringUTFChars( jdl, jd);
		env->ReleaseStringUTFChars( jobId, id);
		env->ReleaseStringUTFChars( nsAddr, ns);
	}



	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    lb_logSync
	* Signature: (Ljava/lang/String;)V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_lb_1logSync
	(JNIEnv *env, jobject obj, jstring jdl){
		const char *jd = env->GetStringUTFChars(jdl, 0);
		if ( edg_wll_LogEventSync(*lbVect[getCtx(env, obj,LB_CTX)] , EDG_WLL_EVENT_CHKPT , EDG_WLL_FORMAT_CHKPT , "1" , jd ) )
			log_error (env , "Unable to log the sync event to LB" ) ;
		env->ReleaseStringUTFChars( jdl, jd);
	}


	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    lb_log_output
	* Signature: (Ljava/lang/String;)V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_lb_1log_1output
	(JNIEnv *env, jobject obj, jstring jobId){
		const char *id = env->GetStringUTFChars(jobId, 0);
		try{
			glite::wmsutils::jobid::JobId jid ( id )  ;
		}catch(exception &exc){  log_error (env , exc.what() ) ;
		}catch (...){ log_error (env, "Fatal Error: Unpredictalbe exception thrown by JNI wrapper");    }
		env->ReleaseStringUTFChars( jobId, id);
	}

	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    lb_log_start
	* Signature: (Ljava/lang/String;Ljava/lang/String;I)V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_lb_1log_1start
	(JNIEnv *env, jobject obj, jstring jdl, jstring host , jint port ){
		const char *addr = env->GetStringUTFChars(host, 0);
		const char *jd = env->GetStringUTFChars(jdl, 0);
		char str_port [1024];
		sprintf (str_port , "%s%s%d" , nsHost.c_str(),":",nsPort );
		lock();
		if (edg_wll_LogTransferSTART(*lbVect[getCtx(env, obj,LB_CTX)] ,
		EDG_WLL_SOURCE_NETWORK_SERVER ,
		addr, str_port , jd, "", "" ))  cerr << "\n\n\nJNI::edg_wll_LogTransferSTART failed"<<flush;
		unlock();
		env->ReleaseStringUTFChars(host, addr);
		env->ReleaseStringUTFChars( jdl, jd);
	}

	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    lb_free
	* Signature: ()V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_lb_1free
	(JNIEnv *env, jobject obj){
		cerr << "JNI lb_free: do nothing (Deprecated)" << endl ;
	}



	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    lb_getSequence
	* Signature: ()Ljava/lang/String;
	*/
	JNIEXPORT jstring JNICALL Java_org_glite_wmsui_apij_Api_lb_1getSequence
	(JNIEnv *env, jobject obj){   return  env->NewStringUTF ( edg_wll_GetSequenceCode(*lbVect[getCtx(env, obj,LB_CTX)] )  );  }




	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    lb_log_listener
	* Signature: (Ljava/lang/String;Ljava/lang/String;I)V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_lb_1log_1listener
	(JNIEnv *env, jobject obj, jstring jobid , jstring host , jint port){
		const char *addr = env->GetStringUTFChars(host, 0);
		const char *id = env->GetStringUTFChars(jobid, 0);
		if ( string(id).length() >1 ){
			try{
				glite::wmsutils::jobid::JobId jid ( id )  ;
				if ( edg_wll_SetLoggingJob(*lbVect[getCtx(env, obj,LB_CTX)],jid.getId() , NULL , EDG_WLL_SEQ_DUPLICATE) )
					log_error (env , "Error found while performing edg_wll_SetLoggingJob LB method") ;
			}catch(exception &exc){   log_error (env , exc.what() ) ;  return ;
			}catch (...){ log_error (env, "Fatal Error: Unpredictalbe exception thrown by JNI wrapper\n");   return ; }
		}
		if ( edg_wll_LogListener(  *lbVect[getCtx(env, obj,LB_CTX)],"InteractiveListener" , string(addr).c_str() ,   (uint16_t) port) ){
			char error_message [1024];
			char *msg, *dsc ;
			edg_wll_Error(   *lbVect[getCtx(env, obj,LB_CTX)]   , &msg , &dsc ) ;
			sprintf ( error_message , "%s%s%s%s%s%s%s","Unable to perform edg_wll_LogListener  at: ",
			getenv ( GLITE_LB_LOG_DESTINATION) , "\n" , msg , " (" , dsc , " )" )  ;
			log_error (env ,   error_message ) ;
		}
		env->ReleaseStringUTFChars(host, addr);
		env->ReleaseStringUTFChars(jobid, id);
	}




	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    lb_log_query
	* Signature: (Ljava/lang/String;I)Ljava/lang/String;
	*/
	JNIEXPORT jstring JNICALL Java_org_glite_wmsui_apij_Api_lb_1log_1query
	(JNIEnv *env, jobject obj, jstring jobid_str, jint step){
		using namespace glite::lb ;
		const char *id = env->GetStringUTFChars(jobid_str, 0);
		int                 i, cnt, error;
		edg_wlc_JobId       jobid;
		edg_wll_Event       *events = NULL;
		edg_wll_QueryRec    jc[2];
		edg_wll_QueryRec    ec[2];
		// parse the jobID string
		error = edg_wlc_JobIdParse(id, &jobid);
		if ( error )   { log_error (env , "Unable to Initialise LB context") ; return  env->NewStringUTF( "" ) ; }
		memset(jc,0,sizeof jc);
		memset(ec,0,sizeof ec);
		// job condition: JOBID = jobid
		jc[0].attr = EDG_WLL_QUERY_ATTR_JOBID;
		jc[0].op = EDG_WLL_QUERY_OP_EQUAL;
		jc[0].value.j = jobid;
		// event condition: Event type = CHKPT
		ec[0].attr = EDG_WLL_QUERY_ATTR_EVENT_TYPE;
		ec[0].op = EDG_WLL_QUERY_OP_EQUAL;
		/******************************************************************
		* This is a workaround. The meaning of the step parameter depends on
		* its value: if greater than zero is a Checkpointing step,
		* otherwise retrieves the first Logging listener Event
		******************************************************************/
		if  (step<0)   ec[0].value.i = EDG_WLL_EVENT_LISTENER ; else
		//Normal behaviour: chkpt
		ec[0].value.i = EDG_WLL_EVENT_CHKPT;
		error = edg_wll_QueryEvents(  *lbVect[getCtx(env, obj,LB_CTX)] , jc, ec, &events );
		if ( error == ENOENT ) {  log_error (env , "No events found: ENOENT") ; return  env->NewStringUTF( "" ) ; }
		if ( error ) {  log_error (env , "Query failed") ;   return  env->NewStringUTF( "" ) ; }
		for ( cnt=0; events[cnt].type; cnt++ ); // counts the number of events
		if ( !cnt ) { log_error (env , "Empty Events vector returned") ;  return  env->NewStringUTF( "" ) ; }
		// sort the events vector using the timestamp
		qsort(events, cnt, sizeof(edg_wll_Event), &cmp_by_timestamp);
		// the last state in the array is the most recent
		if ((int) step>= (int)cnt ){ log_error (env , "Number of requested step bigger than chkpt logged events") ; 	return  env->NewStringUTF( "" ) ; }
		string state ;

		/******************************************************************
		* This is a workaround. The meaning of the step parameter depends on
		* its value: if greater than zero is a Checkpointing step,
		* otherwise retrieves the first Logging listener Event
		******************************************************************/
		if  (step<0){
			char
			str_port  [20] ;
			sprintf ( str_port  , "%d" ,  events[0].listener.svc_port ) ;
			state = string( events[0].listener.svc_host )  + ":"  + string( str_port  );  ;
		}else
			//Normal behaviour: chkpt
			state = string( events[cnt-1-step].chkpt.classad );
		for ( i = 0; i < cnt; i++)
			edg_wll_FreeEvent( events+i );
		free(events);
		env->ReleaseStringUTFChars(jobid_str, id);
		return  env->NewStringUTF(state.c_str()) ;
	}



	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    lb_log_tag
	* Signature: (Ljava/lang/String;Ljava/lang/String;)V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_lb_1log_1tag
	(JNIEnv *env, jobject obj, jstring name, jstring value){

		const char *tName = env->GetStringUTFChars(name, 0);
		const char *tValue = env->GetStringUTFChars(value, 0);

		if (  edg_wll_LogUserTag(*lbVect[getCtx(env, obj,LB_CTX)], tName , tValue   )      ) {
			char error_message [1024];
			char *msg, *dsc ;
			edg_wll_Error(   *lbVect[getCtx(env, obj,LB_CTX)]   , &msg , &dsc ) ;
			sprintf ( error_message , "%s%s%s%s%s%s%s","Unable to perform edg_wll_LogUserTag   at: ",
			getenv ( GLITE_LB_LOG_DESTINATION) , "\n" , msg , " (" , dsc , " )" )  ;
			log_error (env ,   error_message ) ;
		}
		env->ReleaseStringUTFChars(name, tName);
		env->ReleaseStringUTFChars(value, tValue);
	}

/*************************************************************************
BOOKKEEPING methods: JOBID info
*************************************************************************/

	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    lb_user_jobs
	* Signature: (Ljava/lang/String;I)V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_lb_1user_1jobs
	(JNIEnv *env, jobject obj, jstring lbHost, jint port){
		using namespace glite::lb ;
		const char *lb = env->GetStringUTFChars(lbHost, 0);
		//Defining non-native methods:
		jclass cls = env->GetObjectClass(obj);
		jmethodID appStr = env->GetMethodID ( cls , "appendString" ,  "(ILjava/lang/String;)V" ) ;
		if (appStr==0)
			log_error (env , "Fatal Error while retrieving user's jobs.\nUnable to find Api.appStr method\n" ) ;
		try{
			ServerConnection server ;
			vector<glite::wmsutils::jobid::JobId>  jobs ;
			server.setQueryServer( lb , port ) ;
			server.userJobs(jobs) ;
			for  (  vector<glite::wmsutils::jobid::JobId>::iterator it  =  jobs.begin() ; it !=jobs.end() ; it++ )
				env->CallVoidMethod(obj, appStr, (jint) 0 , env->NewStringUTF( it->toString().c_str() ) );
		}catch(exception &exc){     log_error (env , exc.what() ) ;
		}catch (...){ log_error (env, "Fatal Error: Unpredictalbe exception thrown by JNI wrapper\n");    }
		env->ReleaseStringUTFChars(lbHost, lb);
	}


	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    lb_jobs
	* Signature: (Ljava/lang/String;IIILjava/lang/String;Z)V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_lb_1jobs
	(JNIEnv *env, jobject obj, jstring lbHost, jint port, jint from, jint to, jstring utags,  jint inex, jstring issu){
		using namespace glite::lb ;
		using namespace glite::wmsutils::jobid ;
		vector<vector<QueryRecord> >cond  ;
		try{
		// std::auto_ptr<classad::ClassAd> ad( parse_classad(   utags   )  );
		const char *qw = env->GetStringUTFChars( utags , 0);
		string utString = string (qw) ;
		vector<string> attributes ;

		if ( utString.length() != 0 ){
			// USER TAGS QUERY
			// Parse
			glite::jdl::Ad userTags ;
			std::vector<std::string> tagValues ;
			userTags.fromString( utString ) ;
			env->ReleaseStringUTFChars(utags , qw ) ;
			attributes  = userTags.attributes() ;
			for (unsigned int i = 0 ; i < attributes.size() ; i++ ){
				cond.push_back(    vector<QueryRecord> (1,QueryRecord   ( attributes[i]  , QueryRecord::EQUAL,    userTags.getStringValue(  attributes[i] ) [0]  )  ) ) ;
			}
		}
		// STATUS Include-Exclude QUERY
		if (inex!=0){
			/** inex is a number representing all the required states summed, positive for equal, negative for unequal case 
			* the values are : 	
			* JobStatus.UNDEF = 1 , JobStatus.SUBMITTED =2 , JobStatus.WAITING = 4 , JobStatus.READY = 8 .. etc etc
			*/
			//  settint operation
			if (inex <0) {
				// UNEQUAL CASE: EXCLUDE states in boolean AND CONDITION
				// change back the sign
				inex *=(-1) ;
				for ( int i = JobStatus::CODE_MAX-1 ; i >=0  ; i-- ){
					if  ( inex>=  pow( 2.0, (double)i ) ){
						cond.push_back(  vector<QueryRecord> (1, QueryRecord   ( QueryRecord::STATUS   ,QueryRecord::UNEQUAL , i ) ) );
						inex -=(int) pow( 2.0, (double)i ) ;
					}
				}
			} else {
				// EQUAL CASE: INCLUDE states in boolean OR CONDITION
				vector<QueryRecord>  inclVect ;
				for ( int i = JobStatus::CODE_MAX-1 ; i >=0  ; i-- ){
					if  ( inex>=  pow( 2.0, (double)i ) ){
						inclVect.push_back(  QueryRecord   ( QueryRecord::STATUS ,  QueryRecord::EQUAL  ,i ) );
						inex -=(int) pow( 2.0, (double)i ) ;
					}
				}
				cond.push_back (inclVect  ) ;
			}
		}
		// Issuer initialisation
		const char *i = env->GetStringUTFChars( issu , 0);
		string issuer ( i ) ;
		env->ReleaseStringUTFChars(issu , i ) ;
		// LB server initialisation
		ServerConnection server ;
		const char *lb = env->GetStringUTFChars(lbHost, 0);
		server.setQueryServer( lb , port ) ;
		env->ReleaseStringUTFChars(lbHost, lb);

		// Retrieve query attributes
		std::vector<std::vector<std::pair<QueryRecord::Attr,std::string> > > ia = server.getIndexedAttrs();
		// indexed is used to check whether the user has performed a valid query (i.e. a query on the indexed attributes)
		bool indexed = false ;
		string queryErrMsg = "" ;
		for (unsigned int i = 0 ; i< ia.size() ;  i++ ){
			if (indexed) break ;
			for (unsigned int j = 0 ;  j < ia[i].size() ; j++ ) {
				if (indexed) break ;
				switch (   ia[i][j].first ){
					case QueryRecord::OWNER:
						if ( issuer!="") indexed = true ;
						queryErrMsg += "\n'all' condition" ;
						break;
					case QueryRecord::TIME:
						if(     ( from!=0 )  || ( to!=0 )   ) indexed = true ;
						queryErrMsg += "\n'from'/'to' condition" ;
						break;
					case QueryRecord::USERTAG:
						for ( unsigned int k = 0 ; k<  attributes.size()  ; k++ )
							if (   attributes[k] == ia[i][j].second  )  indexed = true ;
						queryErrMsg += "\n'user tag " + ia[i][j].second +"=<tag value>' condition"   ;
					default:
						break;
				}
			}
		}
		if ( !indexed){
			// query is useless, no indexed value found
			if ( queryErrMsg =="" )  queryErrMsg = " unable to find any indexed key";
			else queryErrMsg =  " try to use the following option(s):"   + queryErrMsg ;
			log_error (env , "No indexed attribute queried: " + queryErrMsg ) ;
			return ;
		}



		// --ALL OPTION
		if ( issuer.length() != 0 ) {
			cond.push_back(   vector<QueryRecord> (1,QueryRecord   ( QueryRecord::OWNER , QueryRecord::EQUAL,  issuer ) ) );
		}
		// FROM - TO OPTIONS
		if ( from!=0 ) {
			timeval   tvFrom=  { from   ,0 };
			cond.push_back(   vector<QueryRecord> (1,QueryRecord   ( QueryRecord::TIME , QueryRecord::GREATER, JobStatus::SUBMITTED, tvFrom ) ) );
		}
		if ( to!=0 ) {
			timeval   toStruct = {  to   ,0 };
			cond.push_back(   vector<QueryRecord> (1,QueryRecord   ( QueryRecord::TIME , QueryRecord::LESS, JobStatus::SUBMITTED, toStruct ) ) );
		}
		// Retrieving Jobs
		vector<JobId> jobs = server.queryJobs ( cond  ) ;
		//Defining non-native methods:
		jclass cls = env->GetObjectClass(obj);
		jmethodID appStr = env->GetMethodID ( cls , "appendString" ,  "(ILjava/lang/String;)V" ) ;
		if (appStr==0)
			log_error (env , "Fatal Error while retrieving user's jobs.\nUnable to find Api.appStr method\n" ) ;
		else for  (  vector<JobId>::iterator it  =  jobs.begin() ; it !=jobs.end() ; it++ ){
			env->CallVoidMethod(obj, appStr, (jint) 0 , env->NewStringUTF( it->toString().c_str() ) );
		}

	}catch( glite::wmsutils::exception::Exception &exc){
		log_error (env ,  exc.what() ) ;
	}catch(exception &exc){  log_error (env ,  exc.what() ) ;
	}catch (...){ log_error (env, "Fatal Error: Unpredictalbe exception thrown by JNI wrapper\n");    }
	};




/*************************************************************************
BOOKKEEPING methods: JOBSTATUS info
*************************************************************************/

	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    lb_user_status
	* Signature: (Ljava/lang/String;I)V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_lb_1user_1status
	(JNIEnv *env, jobject obj, jstring  lbHost, jint port){
	using namespace glite::lb ;
	const char *lb = env->GetStringUTFChars(lbHost, 0);
		try{
			ServerConnection server ;
			server.setQueryServer( lb , port ) ;
			vector<JobStatus> states = server.userJobStates()  ;
			for (unsigned int j = 0 ; j< states.size() ; j++) {
				loadStatus(  env, obj, states[j] ) ;
			}  // end for (unsigned i=0; i < attrList.size(); i++ )
		} catch (exception &exc){     log_error (env , exc.what() ) ;
		}catch (...){ log_error (env, "Fatal Error: Unpredictalbe exception thrown by JNI wrapper\n");    }
		env->ReleaseStringUTFChars(lbHost, lb);
	}


	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    lb_status
	* Signature: (Ljava/lang/String;)V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_lb_1status
	(JNIEnv *env, jobject obj, jstring jobId ){
		const char *id = env->GetStringUTFChars(jobId, 0);
		using namespace glite::lb ;
		string err ;
		glite::wmsutils::jobid::JobId jid (id)  ;
		env->ReleaseStringUTFChars(jobId, id);
		try{
			err = "- " + jid.toString()  +":\n";
			lock() ;
			lbJob = jid ;
			JobStatus  status =   lbJob.status( Job::STAT_CLASSADS | Job::STAT_CHILDSTAT      ) ;
			unlock() ;
			// Load the Status
			loadStatus(  env, obj, status );
		}catch( exception &exc){
			unlock() ;
			log_error (env , err +exc.what()  ) ;
		}catch (...){
			unlock() ;
			log_error (env, "Fatal Error: Unpredictalbe exception thrown by JNI wrapper");
		}
	}

	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    lb_notify
	* Signature: (Ljava/lang/String;I)V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_lb_1notify
	(JNIEnv *env, jobject obj, jstring jobad  , jint timeout ){
		using namespace glite::lb ;
		using namespace glite::wmsutils::jobid ;

		try{
			glite::jdl::Ad notifAd ;
			const char *ad = env->GetStringUTFChars(jobad, 0);
			notifAd.fromString( ad ) ;
			env->ReleaseStringUTFChars(jobad, ad);
			Notification n  ;
			// Fill the JobIds notifying for
			vector<string> jobids =  notifAd.getStringValue( "JOBIDS" ) ;
			for (unsigned int i = 0 ; i< jobids.size() ; i++)   n.addJob (JobId ( jobids[i]  )  ) ;
			// Fill the States vector
			vector<int> tmpstates = notifAd.getIntValue( "STATES" ) ;
			vector<JobStatus::Code> states ;
			for (unsigned int i = 0 ; i< tmpstates.size() ; i++) states.push_back(  ( JobStatus::Code) tmpstates[i]  ) ;
			n.setStates( states) ;
			n.Register() ;
			// Set the timeout value
			timeval to = { (int) timeout , 0 } ;
			JobStatus stat ;
			if (n.receive ( stat , to ) ) ; // return 1 ;
			else loadStatus ( env , obj , stat )  ;
			// return 0 ;
		}catch( glite::wmsutils::exception::Exception &exc){
			unlock() ;
			if (exc.getCode()==ETIMEDOUT) log_error (env , exc.what()  ) ;
			else log_error (env ,exc.what()  ) ;
		}catch( exception &exc){
			unlock() ;
			log_error (env ,exc.what()  ) ;
		}catch (...){
			unlock() ;
			log_error (env, "Fatal Error: Unpredictalbe exception thrown by JNI wrapper");
		}

	}

/*************************************************************************
BOOKKEEPING methods: LOGGING info
*************************************************************************/

	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    lb_log
	* Signature: (Ljava/lang/String;)V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_lb_1log
	(JNIEnv *env, jobject obj, jstring jobId){
		const char *id = env->GetStringUTFChars(jobId, 0);
		using namespace glite::lb ;
		//Defining non-native methods:
		jclass cls = env->GetObjectClass(obj);
		jmethodID appStr = env->GetMethodID ( cls , "appendString" ,  "(ILjava/lang/String;)V" ) ;
		if (appStr==0)
			log_error (env , "Fatal Error while retrieving logging information for the job: "+string(id) +".\nUnable to find Api.appStr method\n" ) ;
		jmethodID appInt = env->GetMethodID ( cls , "appendInt" ,  "(II)V" ) ;
		if (appInt==0)
			log_error (env , "Fatal Error while retrieving logging information for the job: "+string(id)+".\nUnable to find Api.appInt method\n"   ) ;
		try{
			glite::wmsutils::jobid::JobId jid (id)  ;
			lbJob = jid   ;
			vector <Event> events= lbJob.log();
			char time [1024] ;
			for (unsigned i=0; i < events.size(); i++ ) {
				std::vector<pair<Event::Attr,Event::AttrType> > attrList = events[i].getAttrs();
				const int ARRIVED_CODE = 54 ;  // Do not change this value unless change the Event.java code
				const int EVENT = 52 ;  // Do not change this value unless change the Event.java code
				env->CallVoidMethod(obj, appStr, (jint) EVENT , env->NewStringUTF( events[i].name().c_str() ) );
				const int EVENT_CODE = 53 ;  // Do not change this value unless change the Event.java code
				env->CallVoidMethod(obj, appInt, (jint) EVENT_CODE , (int)events[i].type ) ;
				for (unsigned j=0; j < attrList.size(); j++ ) {
					switch (attrList[j].first){
				/* Int attributes*/
					case Event::JOBTYPE:
					case Event::NSUBJOBS:
					case Event::EXIT_CODE:
					case Event::PRIORITY:
					case Event::SVC_PORT:
					case Event::DEST_PORT:
					// case Event::PERMISSION: // Not avaliable in DAG branch
					// case Event::PERMISSION_TYPE: // Not avaliable in DAG branch
					// case Event::USER_ID: // Not avaliable in DAG branch
					// case Event::USER_ID_TYPE: // Not avaliable in DAG branch
						env->CallVoidMethod(obj, appInt, (jint) attrList[j].first -1 , (jint) events[i].getValInt(attrList[j].first) );
						break;
					case Event::LEVEL:
						env->CallVoidMethod(obj, appStr, (jint) attrList[j].first -1 , env->NewStringUTF(  edg_wll_LevelToString ( (edg_wll_Level) events[i].getValInt(attrList[j].first) ) ) );
						break;
					case Event::STATUS_CODE:
						env->CallVoidMethod(obj, appStr, (jint) attrList[j].first -1 , env->NewStringUTF(   (  edg_wll_DoneStatus_codeToString ((edg_wll_DoneStatus_code) events[i].getValInt(attrList[j].first)     ) )  ) );
						break;
					case Event::SOURCE:
					case Event::DESTINATION:
					case Event::FROM:
						env->CallVoidMethod(obj, appStr, (jint) attrList[j].first -1 , env->NewStringUTF( edg_wll_SourceToString ( (edg_wll_Source )events[i].getValInt(attrList[j].first)    ) ) );
						break;
				/* String attributes*/
					case Event::SRC_ROLE:
					case Event::HOST:
					case Event::SEQCODE:
					case Event::USER:
					case Event::SRC_INSTANCE:
					case Event::VALUE:
					case Event::RESULT:
					case Event::NAME:
					case Event::REASON:
					case Event::DESCR:
					case Event::SVC_HOST:
					case Event::SVC_NAME:
					case Event::FROM_INSTANCE:
					case Event::FROM_HOST:
					case Event::LOCAL_JOBID:
					case Event::QUEUE:
					case Event::NODE:
					case Event::JDL:
					case Event::SEED:
					case Event::NS:
					case Event::JOB:
					case Event::RETVAL:
					case Event::CLASSAD:
					case Event::TAG:
					case Event::HELPER_PARAMS:
					case Event::HELPER_NAME:
					case Event::DEST_ID:
					case Event::DEST_HOST:
					case Event::DEST_JOBID:
					case Event::DEST_INSTANCE:
					case Event::JOBSTAT:
					case Event::OWNER:
						env->CallVoidMethod(obj, appStr, (jint) attrList[j].first -1 , env->NewStringUTF( events[i].getValString(attrList[j].first).c_str() ) );
						break;
				/* JobId attributes*/
					case Event::JOBID:
					case Event::PARENT:{
						if(   ((glite::wmsutils::jobid::JobId)events[i].getValJobId(attrList[j].first)).isSet()   )
						env->CallVoidMethod(obj, appStr, (jint) attrList[j].first -1, env->NewStringUTF( events[i].getValJobId(attrList[j].first).toString().c_str() ) ) ;
						}
						break;
				/* Time attributes*/
					case Event::TIMESTAMP:
						{
							timeval t = events[i].getValTime(attrList[j].first);
							sprintf (time , "%d%s%d%s" ,(int)t.tv_sec , ".", (int)t.tv_usec," s") ;
							env->CallVoidMethod(obj, appStr, (jint) attrList[j].first -1 , env->NewStringUTF( time ) );
						}
						break;
				/* Specials*/
					case Event::ARRIVED: //Only available in DAG branch , NOT in MAIN
						{
							timeval t = events[i].getValTime(attrList[j].first);
							sprintf (time , "%d%s%d%s" ,(int)t.tv_sec , ".", (int)t.tv_usec," s") ;
							env->CallVoidMethod(obj, appStr, (jint) ARRIVED_CODE , env->NewStringUTF( time ) );
						}
						break;
					case Event::NOTIFID: //Only available in DAG branch , NOT in MAIN
						cerr << "JNI:: Warning NOTIFID event found but nothing happens" << endl ;
						break;
					default:
						cerr << "\n\n\nJNI Something has gone bad.Please check field: " << j << ")"<< attrList[j].first << "]" <<events[i].getAttrName(attrList[j].first) << endl;
						break;
					}
				} // end of switch
			} //end for events
		}catch( glite::wmsutils::exception::Exception &exc){
		cerr  << "JNI::getLogInfo-> error caught" << exc.dbgMessage() << endl << flush ;
		log_error (env ,  exc.what() ) ;
		}catch(exception &exc){  log_error (env ,  exc.what() ) ;
		}catch (...){ log_error (env, "Fatal Error: Unpredictalbe exception thrown by JNI wrapper\n");    }
		env->ReleaseStringUTFChars(jobId, id);
	}
/*************************************************************************
DAGAD implementation methods:
*************************************************************************/
	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    dagFromFile
	* Signature: (Ljava/lang/String;)V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_dagFromFile
	(JNIEnv *env, jobject obj, jstring fileStr  ){
			jclass cls = env->GetObjectClass(obj);
			jmethodID appInt = env->GetMethodID ( cls , "appendInt" ,  "(II)V" ) ;
			if (appInt==0)
				log_error (env , "Fatal Error: unable to find Api.appInt method\n" ) ;
			// read file
			const char *file = env->GetStringUTFChars(fileStr, 0);
		try{
			ifstream jdl  ( file  ) ;
			// Create Dag
			glite::jdl::ExpDagAd* dagad= new glite::jdl::ExpDagAd ( jdl ) ;
		lock() ;
			env->CallVoidMethod(obj, appInt, DAG_CTX , (jint) dagVect.size() );
			dagVect.push_back(  dagad  ) ;
		unlock() ;
		}catch(exception &exc){   log_error (env , exc.what() ) ;  return ;
		}catch (...){ log_error (env, "Fatal Error: Unpredictalbe exception thrown by JNI wrapper\n");   return ; }
			env->ReleaseStringUTFChars( fileStr, file );
	};
	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    dagToString
	* Signature: (I)Ljava/lang/String;
	*/
	JNIEXPORT jstring JNICALL Java_org_glite_wmsui_apij_Api_dagToString
	(JNIEnv *env, jobject obj, jint level  ){
	lock() ;
		glite::jdl::ExpDagAd* dagad = dagVect[  getCtx( env, obj , DAG_CTX)]  ;
	unlock() ;
		return  env->NewStringUTF(  dagad->toString(  (glite::jdl::ExpDagAd::level) level ).c_str()   ) ;
	};


	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    dag_getSubmissionStrings
	* Signature: ()V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_dag_1getSubmissionStrings
	(JNIEnv *, jobject){
		// glite::jdl::ExpDagAd* dagad = dagVect[  getCtx( env, obj , DAG_CTX)]  ;
		// return  env->NewStringUTF(  dagad->toString(  level )   ) ;
	};

	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    dag_getSubAttributes
	* Signature: (Ljava/lang/String;)V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_dag_1logUserTags
	(JNIEnv *env, jobject obj, jstring   jobid  ){
	lock() ;
		glite::jdl::ExpDagAd* dagad = dagVect[  getCtx( env, obj , DAG_CTX)]  ;
	unlock() ;
		std::vector<  std::pair<  std::string  ,     classad::ExprTree* > > userTags = dagad->getSubAttributes ( "UserTags"  ) ;
		for (unsigned int i = 0 ; i < userTags.size() ; i++ ){
			if (  userTags[i].second->GetKind () != classad::ExprTree::CLASSAD_NODE )  cerr << "CLASSAD error" << endl ;
			glite::wmsutils::jobid::JobId subJobid(  userTags[i].first   ) ;
			edg_wll_SetLoggingJob(  *lbVect[getCtx(env, obj,LB_CTX)] , subJobid.getId() ,  NULL, EDG_WLL_SEQ_NORMAL) ;
			logUserTags (  (classad::ClassAd*)(userTags[i].second)   ,  *lbVect[getCtx(env, obj,LB_CTX)]   ) ;
		}
		// Ripristinate the old logging job Id
		const char *id = env->GetStringUTFChars(jobid, 0);
		try{
			glite::wmsutils::jobid::JobId jid ( id )  ;
			edg_wll_SetLoggingJob(  *lbVect[getCtx(env, obj,LB_CTX)] , jid.getId() ,  NULL, EDG_WLL_SEQ_NORMAL) ;
		}catch(exception &exc){   log_error (env , exc.what() ) ;  return ;
		}catch (...){ log_error (env, "Fatal Error: Unpredictalbe exception thrown by JNI wrapper\n");   return ; }
		env->ReleaseStringUTFChars( jobid ,  id ) ;
	};


	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    dagSetAttribute
	* Signature: (ILjava/lang/String;)V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_dagSetAttribute
	(JNIEnv *env, jobject obj, jint  name  , jstring  value ){
	lock() ;
		glite::jdl::ExpDagAd* dagad = dagVect[  getCtx( env, obj , DAG_CTX)]  ;
	unlock() ;
		const char *val = env->GetStringUTFChars(value, 0);
		dagad->setAttribute (   (glite::jdl::ExpDagAd::attribute) name , string(val) ) ;
		env->ReleaseStringUTFChars( value, val );
	};

	/*
	* Class:     edg_workload_userinterface_jclient_Api
	* Method:    registerDag
	* Signature: (ILjava/lang/String;)V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_registerDag
	(JNIEnv *env, jobject  obj ,  jstring jid , jstring nsAddress ){
	try{
		glite::wmsutils::jobid::JobId id(    string (  env->GetStringUTFChars( jid , 0)   )      ) ;
		// env->ReleaseStringUTFChars( jid , id );
		const char *str_addr = env->GetStringUTFChars( nsAddress , 0);
		// Retrieving the dagad
	lock();
		glite::jdl::ExpDagAd* dagad = dagVect[  getCtx( env, obj , DAG_CTX)]  ;
	unlock() ;
		// Retrieving the LB context
		edg_wll_Context ctx = *lbVect[getCtx(env, obj,LB_CTX)]  ;
		//  array of subjob ID's
		edg_wlc_JobId* subjobs = NULL ;
		// Register the job
	lock();
		if (      edg_wll_RegisterJobSync( ctx,   id.getId()  , EDG_WLL_REGJOB_DAG,  dagad->toString ( glite::jdl::ExpDagAd::NO_NODES ).c_str() ,
			str_addr,    dagad->size()  ,   NULL,   &subjobs  ) ){
			char error_message [1024];
			char *msg, *dsc ;
			edg_wll_Error(   *lbVect[getCtx(env, obj,LB_CTX)]   , &msg , &dsc ) ;
			sprintf ( error_message , "%s%s%s%s%s%s%s","Unable to perform  edg_wll_RegisterJobSync   at: ",
			getenv ( GLITE_LB_LOG_DESTINATION) , "\n" , msg , " (" , dsc , " )" )  ;
			log_error (env ,   error_message ) ;
		}
	unlock();
		// REGISTER SUB JOBS
		vector<string> jdls = dagad->getSubmissionStrings() ;
		// array of Jdls
		char **jdls_char , **zero_char ;
		vector<string>::iterator iter ;
		jdls_char = (char**) malloc(sizeof(char*) * (jdls.size()+1));
		zero_char =jdls_char ;
		jdls_char[jdls.size()] = NULL;
		int i = 0 ;
		for (  iter = jdls.begin() ; iter!=jdls.end() ; iter++, i++){
			*zero_char = (char*) malloc ( iter->size() + 1 ) ;
			sprintf ( *zero_char  , "%s" ,  iter->c_str() ) ;
			zero_char++;
		}
		if (      edg_wll_RegisterSubjobs (  ctx,   id.getId()   , jdls_char, str_addr, subjobs )     ){
			char error_message [1024];
			char *msg, *dsc ;
			edg_wll_Error(   *lbVect[getCtx(env, obj,LB_CTX)]   , &msg , &dsc ) ;
			sprintf ( error_message , "%s%s%s%s%s%s%s","Unable to perform edg_wll_LogUserTag   at: ",
			getenv ( GLITE_LB_LOG_DESTINATION) , "\n" , msg , " (" , dsc , " )" )  ;
			log_error (env ,   error_message ) ;
		}

		// Release Memory
		for ( unsigned int i = 0 ; i < jdls.size() ; i++ )  std::free(jdls_char[i]);
		std::free(jdls_char);
		vector<string> jobids ;
		for ( unsigned int i= 0 ; i< dagad->size() ; i++ ){
			jobids.push_back(  string (    edg_wlc_JobIdUnparse( subjobs[i]  )  )  ) ;
		}
		// Insert the sub-jobids into the DagAd
		dagad->setJobIds ( jobids );
	}catch(exception &exc){   log_error (env , exc.what() ) ;  return ;
	}catch (...){ log_error (env, "Fatal Error: Unpredictalbe exception thrown by JNI wrapper\n");   return ; }
	};


	/*
	* Class:     edg_workload_userinterface_jclient_Api
* Method:    logDefaultValues
	* Signature: (Z)V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_logDefaultValues
	(JNIEnv *env, jobject  obj, jboolean boom  ){
		lock() ;
		glite::jdl::ExpDagAd* dagad = dagVect[  getCtx( env, obj , DAG_CTX)]  ;
		unlock() ;
		dagad->setDefaultValues ( boom ) ;
	};

	/*
	* Class:     org_glite_wmsui_apij_Api
* Method:    registerPart
	* Signature: (Ljava/lang/String;Ljava/lang/String;I)V
	*/
	JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_registerPart
	(JNIEnv *env,  jobject obj, jstring jid, jstring original, jstring submission, jstring nsAddress, jint res_number){

	try{
		glite::wmsutils::jobid::JobId id(    string (  env->GetStringUTFChars(jid , 0)   )      ) ;
		const char *str_addr = env->GetStringUTFChars( nsAddress , 0);

		// Retrieving the LB context
		edg_wll_Context ctx = *lbVect[getCtx(env, obj,LB_CTX)]  ;
		//  array of subjob ID's
		edg_wlc_JobId* subjobs = NULL ;
		// Register the job
	lock();
		if (      edg_wll_RegisterJobSync( ctx,   id.getId()  , EDG_WLL_REGJOB_PARTITIONED, env->GetStringUTFChars( submission , 0) ,
			str_addr,   res_number,   NULL,   &subjobs  ) ){
			char error_message [1024];
			char *msg, *dsc ;
			edg_wll_Error( ctx   , &msg , &dsc ) ;
			sprintf ( error_message , "%s%s%s%s%s%s%s","Unable to perform  edg_wll_RegisterJobSync   at: ",
			getenv ( GLITE_LB_LOG_DESTINATION) , "\n" , msg , " (" , dsc , " )" )  ;
			log_error (env ,   error_message ) ;
		}
	unlock();


		// Create the DagAd Instance
		vector<string> jobids ;
		for ( unsigned int i= 0 ; i< res_number ; i++ ){
			jobids.push_back(  string (    edg_wlc_JobIdUnparse( subjobs[i]  )  )  ) ;
		}
		glite::jdl::ExpDagAd* dagad =NULL;
		 glite::wms::partitioner::Partitioner part (  glite::wmsutils::classads::parse_classad( env->GetStringUTFChars( submission , 0) )  , jobids );
		dagad= new glite::jdl::ExpDagAd ( part.createDag() ) ;


		// REGISTER SUB JOBS
		vector<string> jdls = dagad->getSubmissionStrings() ;
		// array of Jdls
		char **jdls_char , **zero_char ;
		vector<string>::iterator iter ;
		jdls_char = (char**) malloc(sizeof(char*) * (jdls.size()+1));
		zero_char =jdls_char ;
		jdls_char[jdls.size()] = NULL;
		int i = 0 ;
		for (  iter = jdls.begin() ; iter!=jdls.end() ; iter++, i++){
			*zero_char = (char*) malloc ( iter->size() + 1 ) ;
			sprintf ( *zero_char  , "%s" ,  iter->c_str() ) ;
			zero_char++;
		}
		if (      edg_wll_RegisterSubjobs (  ctx,   id.getId()   , jdls_char, str_addr, subjobs )     ){
			char error_message [1024];
			char *msg, *dsc ;
			edg_wll_Error(   *lbVect[getCtx(env, obj,LB_CTX)]   , &msg , &dsc ) ;
			sprintf ( error_message , "%s%s%s%s%s%s%s","Unable to perform edg_wll_LogUserTag   at: ",
			getenv ( GLITE_LB_LOG_DESTINATION) , "\n" , msg , " (" , dsc , " )" )  ;
			log_error (env ,   error_message ) ;
		}

		// RELEASE ALLOCATED MEMORY
		for ( unsigned int i = 0 ; i < jdls.size() ; i++ )  std::free(jdls_char[i]);
		std::free(jdls_char);
		env->ReleaseStringUTFChars( nsAddress, str_addr);

		// STORE DAGAD VALUE
		jclass cls = env->GetObjectClass(obj);
		jmethodID appInt = env->GetMethodID ( cls , "appendInt" ,  "(II)V" ) ;
		if (appInt==0)
			log_error (env , "Fatal Error: unable to find Api.appInt method\n" ) ;
	lock() ;
		env->CallVoidMethod(obj, appInt, DAG_CTX , (jint) dagVect.size() );
		dagVect.push_back(  dagad  ) ;
	unlock() ;
		}catch(exception &exc){   log_error (env , exc.what() ) ;  return ;
		}catch (...){ log_error (env, "Fatal Error: Unpredictalbe exception thrown by JNI wrapper\n");   return ; }
	}



/*************************************************************************
AUTHORISATION methods:
*************************************************************************/

  /*
 * Class:     edg_workload_userinterface_jclient_Api
 * Method:    ns_dgas
 * Signature: (Ljava/lang/String;Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL Java_org_glite_wmsui_apij_Api_ns_1dgas
  (JNIEnv *env, jobject obj, jstring  jid  , jstring  hlrj  ){
	//DGAS Job Authorisation manipulation
	jobAuth_data jobAuth ;
	jobAuth.dgJobId = string (env->GetStringUTFChars( jid , 0)  ) ;
	string ptr;
	string hlr =   string (env->GetStringUTFChars( hlrj , 0)  ) ;
	try{
		if (dgas_jobAuth_client ( hlr, jobAuth , &ptr )) cerr << "dgas_jobAuth_client failed" << endl ;
	}catch (exception &exc){ cerr << "dgas_jobAuth_client failed:" << exc.what() << endl;}
}

// EOF


