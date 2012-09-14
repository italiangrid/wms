#ifndef   GLITE_WMSUTILS_EXCEPTION_CODES_H
#define  GLITE_WMSUTILS_EXCEPTION_CODES_H
// pure C-style code (needed by some libraries)
#define GLITE_WMS_COMMON_ERROR_BASE 900
#define GLITE_WMS_USERINTERFACE_ERROR_BASE 1000
#define GLITE_WMS_NETWORKSERVER_ERROR_BASE 1200
#define GLITE_WMS_SOCKET_ERROR_BASE 1300
#define GLITE_WMS_LDAP_ERROR_BASE 1350
#define GLITE_WMS_LOGGING_ERROR_BASE 1400
#define GLITE_WMS_REQUESTAD_ERROR_BASE 1500
#define GLITE_WMS_CHECKPOINT_ERROR_BASE 1600
#define GLITE_WMS_CONFIGURATION_ERROR_BASE 1800
#ifdef __cplusplus
namespace glite { 
namespace wmsutils { 
namespace exception {
	/**
	* The Error Code
	*/
	enum {
			WMS_COMMON_BASE = GLITE_WMS_COMMON_ERROR_BASE,
			THREAD_INIT  ,          // pthread_attr_init              method failed
			THREAD_DETACH ,         // pthread_attr_setdetachstate    method failed
			THREAD_CREATE ,         // pthread_create                 method failed
			THREAD_JOIN,
			THREAD_SSL,
			WMS_FATAL_ERROR,
			WMS_UI_ERROR_BASE            = GLITE_WMS_USERINTERFACE_ERROR_BASE,
			WMS_NS_ERROR_BASE            = GLITE_WMS_NETWORKSERVER_ERROR_BASE,
			WMS_SOCKET_ERROR_BASE        = GLITE_WMS_SOCKET_ERROR_BASE,
			WMS_LDAP_ERROR_BASE          = GLITE_WMS_LDAP_ERROR_BASE,
			WMS_LB_ERROR_BASE            = GLITE_WMS_LOGGING_ERROR_BASE ,
			WMS_REQUESTAD_ERROR_BASE     = GLITE_WMS_REQUESTAD_ERROR_BASE,
			WMS_CHKPT_ERROR_BASE         = GLITE_WMS_CHECKPOINT_ERROR_BASE,
			WMS_CONFIGURATION_ERROR_BASE = GLITE_WMS_CONFIGURATION_ERROR_BASE
	};
} // exception namespace
} // wmsutils namespace
} // glite namespace
#endif   //ifdef c++
#endif
