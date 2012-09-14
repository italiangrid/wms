/**
*        Copyright (c) Members of the EGEE Collaboration. 2004.
*        See http://public.eu-egee.org/partners/ for details on the copyright holders.
*        For license conditions see the license file or http://www.eu-egee.org/license.html
*
*       Authors:        Alessandro Maraschini <alessandro.maraschini@datamat.it>
*                       Marco Sottilaro <marco.sottilaro@datamat.it>
*/

//      $Id$

#ifndef GLITE_WMS_CLIENT_EXCMAN
#define GLITE_WMS_CLIENT_EXCMAN
/*
 * excman.h
 */
#include "glite/wmsutils/exception/Exception.h"
#include "glite/wms/wmproxyapi/wmproxy_api.h"

// std error codes
#include <errno.h>

namespace glite{
namespace wms{
namespace client{
namespace utilities{
#define WMS_EXCM_TRY()try{
#define WMS_EXCM_CATCH(sev)}catch (glite::wmsutils::exception::Exception &exc){cerr << exc.what();}
enum severity{
	WMS_UNDEF,
	WMS_DEBUG,
        WMS_INFO,
	WMS_WARNING,
	WMS_ERROR,
	WMS_FATAL
};

enum errCode{
	DEFAULT_ERR_CODE
};

class WmsClientException: public glite::wmsutils::exception::Exception {
public:
	   WmsClientException(const std::string& file, int line,const std::string& method,
	   		int code,const std::string& exception_name, const std::string& error);
};
/*
* gets a formatted error description of the input exception;
* it can be printed on the std-output/error (see "debug" input parameter) and/or into a file  (see "path" input parameter)
* Messages which severity are "none" and "info" are printed on the std-output; the other ones on the std-error (see "sev" input parameter)
* @param sev severity of the message	(WMS_NONE, WMS_INFO, WMS_WARNING, WMS_ERROR, WMS_FATAL)*
* @param header header text
* @param exc the exception
* @param debug flag indicating whether the formatted message have to be printed on the std-output
* @param path the pathname of the file where to write the formatted message
* @return the formatted message
*/
const std::string errMsg(severity sev, const std::string &header,glite::wmsutils::exception::Exception& exc, const bool &debug=false, std::string *path=NULL);

/*
* gets a formatted error message text according to the input information;
* it can be printed on the std-output/error (see "debug" input parameter) and/or into a file as well (see "path" input parameter)
* Messages which severity are "none" and "info" are printed on the std-output; the other ones on the std-error (see "sev" input parameter)
* @param sev severity of the message (WMS_NONE, WMS_INFO, WMS_WARNING, WMS_ERROR, WMS_FATAL)
* @param header header text
* @param err the error message
* @param exc the exception
* @param debug flag indicating whether the messages have to be printed on the std-output
* @param path the pathname of the file
* @return the formatted message
*/
const std::string errMsg(severity sev, const std::string &header,const std::string& err, const bool &debug=false, std::string *path=NULL);
/*
* gets a formatted error description of the input exception
* @param b_ex the exception
* @return the formatted message
*/
const std::string errMsg (const glite::wms::wmproxyapi::BaseException &b_ex );

}}}} // ending namespaces

#endif

