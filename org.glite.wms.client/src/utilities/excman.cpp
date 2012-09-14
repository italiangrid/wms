// HEADER
#include "excman.h"
#include "utils.h"
#include "fstream" // file streams
#include "sstream" // file streams
#include "time.h" // time fncts
#include <unistd.h> // getpid


using namespace std ;
namespace glite{
namespace wms{
namespace client{
namespace utilities{

 const string monthStr[]  = {"Jan", "Feb", "March", "Apr", "May", "June" ,"July", "Aug", "Sept", "Oct", "Nov", "Dec"};

 enum WmcStdStream {WMC_OUT, WMC_ERR};

//WmsClientException class
WmsClientException::WmsClientException (const string& file, int line, const string& method,
				int code,const string& exception_name,const string& error):
				Exception(file,line,method,code,exception_name){
	error_message=error;
};
// End Class WmsClientException

/*
* prints the error message
*/
const std::string errMsg(severity sev, const std::string &header, const std::string& err, const bool &debug, std::string* path){

	const string stripe= "-----------------------------------------";
        char ws=(char)32;
	ostringstream px ;
        string mglog = "";
        string mgout = "";
        WmcStdStream ss = (WmcStdStream)0;
	time_t now = time(NULL);
	// PREFIX MSG: timestamp
        struct tm *ns = localtime(&now);
        // PREFIX MSG: time stamp: day-month
        px << Utils::twoDigits(ns->tm_mday) << ws << monthStr[ns->tm_mon] << ws << (ns->tm_year+1900) <<"," << ws;
        // PREFIX MSG: time stamp: hh::mm:ss
 	px << Utils::twoDigits(ns->tm_hour) << ":" << Utils::twoDigits(ns->tm_min) << ":" << Utils::twoDigits(ns->tm_sec) << ws;
        // PREFIX MSG: pid
        px << "-I- PID:" << ws << getpid( ) << ws   ;
	switch (sev){
        	case WMS_DEBUG:{
                        // log message
			mglog = px.str( ) +  string("(Debug) -") + ws + header + ws + err + string("\n");
                        if (debug){
                                // std out message
                                mgout += stripe +"\n";
                                mgout += mglog ;
                                mgout += stripe +"\n";
                                ss = WMC_OUT;
                        }
			break;
		}
        	case WMS_INFO:{
                	mglog = px.str( ) + string("(Info) -") + ws + header + ws + err + string("\n");
			if (debug){
				mgout = string("\n") + header + ws + err + string("\n\n");
				ss = WMC_OUT;
    			}
			break;
		}
		case WMS_WARNING:{
			mglog += px.str( ) +  string("(Warning) -") + ws + header + ws + err +string("\n");
			if (debug){
				mgout = string("Warning -") +  ws + header  ;
				if (err.size()>0){ mgout += string("\n") + err + string("\n");}
				ss = WMC_ERR;
    			}
			break;
		}
		case WMS_ERROR:{
			mglog += px.str( ) +  string("(Error) -") + ws + header + ws + err + string("\n");
			if (debug){
				mgout  =  string("Error -") + ws +header + string("\n") + err +string("\n");
				ss = WMC_ERR;
    			}
                        break;
		}
		case WMS_FATAL:{
			mglog += px.str( ) + string( "(Fatal Error) -") + ws + header + ws + err + string("\n");
			if (debug){
				mgout =  string("Fatal Error -") + ws+ header +  string("\n") + err + string("\n");
				ss = WMC_ERR;
    			}
			break;
		}
		default:{
			mglog += header + ws + err + string("\n");
			break;
   		}
	}
	// prints the message into a file
 	if (path && mglog.size()>0) {
        	ofstream outstream(path->c_str(), ios::app);
        	if (outstream.is_open() ) {
                        // to file
			outstream << stripe << "\n";
			outstream << mglog ;
			// closes the file
                        outstream.close ( );
         	}
   	}
        // prints the message on the std-output/error
        if (mgout.size()>0){
                if (ss==WMC_OUT){
                	cout << mgout << flush ;
 		} else if (ss==WMC_ERR){
			cerr << "\n" << mgout << "\n" << flush ;
                }
        }
	return mglog;
};
/*
*
*/
const std::string errMsg(severity sev, const std::string &header, glite::wmsutils::exception::Exception& exc, const bool &debug, std::string* path){
	string h = header + exc.getExceptionName();
        return errMsg(sev, h,string(exc.what()),debug,path);
};

const std::string errMsg (const glite::wms::wmproxyapi::BaseException &b_ex ){
        string meth = b_ex.methodName.c_str();
        string *errcode =b_ex.ErrorCode ;
        string *description = b_ex.Description ;
        string excmsg = "";
        //description
        if (description){ if (description->size() > 0 ){
		excmsg += *description+"\n" ; }
	}
        if (b_ex.FaultCause) {
                int size = b_ex.FaultCause->size( );
                for (int i = 0; i < size; i++) {
                        excmsg += (*b_ex.FaultCause)[i]+"\n";
                }
        }
        //method
        if (meth.size() > 0){excmsg +="Method: "+meth+"\n";}
        //errorcode
        if (errcode){if (errcode->size()>0){excmsg += "Error code: " + *errcode+"\n";}}
        return excmsg;
}

}}}} // ending namespaces
