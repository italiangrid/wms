#include "glite/ce/cream-client-api-c/creamApiLogger.h"
#include <boost/algorithm/string.hpp>
#include "classad_distribution.h"
#include "eventStatusListener.h"
#include "subscriptionCache.h"
#include "ClassadSyntax_ex.h"
#include "iceConfManager.h"
#include "jobCache.h"
#include <unistd.h>
#include <string>
#include <iostream>
#include <cerrno>
#include <sstream>
#include <cstring> // for memset
#include <netdb.h>

extern int h_errno;
extern int errno;

using namespace std;

namespace iceUtil = glite::wms::ice::util;

void parseEventJobStatus(string&, string&, const string&)
  throw(iceUtil::ClassadSyntax_ex&);

//______________________________________________________________________________
iceUtil::eventStatusListener::eventStatusListener(int i,const string& hostcert)
  : CEConsumer(i),
    grid_JOBID(""),
    cream_JOBID(""),
    status(glite::ce::cream_client_api::job_statuses::UNKNOWN),
    endaccept(false),
    subscriber(),
    subManager(),
    T(iceUtil::iceConfManager::getInstance()->getICETopic()),
    P(5000),
    proxyfile(hostcert),
    tcpport(i), 
    myname(""),
    //cemon_subscribed_to(),
    conf(iceUtil::iceConfManager::getInstance()),
    log_dev( glite::ce::cream_client_api::util::creamApiLogger::instance()->getLogger() )
{
  char name[256];
  memset((void*)name, 0, 256);

  if(gethostname(name, 256) == -1) {
    log_dev->fatalStream() << "eventStatusListener::CTOR - Couldn't resolve local hostname: "
                           << strerror(errno)
                           << log4cpp::CategoryStream::ENDLINE;

    exit(1);
  }
  struct hostent *H=gethostbyname(name);
  if(!H) {
      log_dev->fatalStream() << "eventStatusListener::CTOR - Couldn't resolve local hostname: "
                             << strerror(h_errno)
                             << log4cpp::CategoryStream::ENDLINE;
    exit(1);
  }
  myname = H->h_name;
  log_dev->info( "eventStatusListener::CTOR - Listener created!" );
  T.addDialect(NULL);
  init();
}

//______________________________________________________________________________
void iceUtil::eventStatusListener::operator()()
{
  endaccept=false;
  while(!endaccept) {
      log_dev->info( "eventStatusListener::()() - Waiting for job status notification" );
    acceptJobStatus();
    sleep(1);
  }
  log_dev->info( "eventStatusListener::()() - ending..." );
}

//______________________________________________________________________________
void iceUtil::eventStatusListener::acceptJobStatus(void)
{
  
  /**
   * Waits for an incoming connection
   */
  if(!this->accept() && !endaccept) {
    if(endaccept) {
        log_dev->info( "eventStatusListener::acceptJobStatus() - eventStatusListener is ending" );
      return;
    } else
        log_dev->info( "eventStatusListener::acceptJobStatus() -"
                       " eventStatusListener::acceptJobStatus()"
                       " - CEConsumer::Accept() returned false." );
  }
  
  log_dev->infoStream() << "eventStatusListener::acceptJobStatus() - Connection accepted from ["
                        << this->getClientIP() << "]" 
                        << log4cpp::CategoryStream::ENDLINE;
  
  /**
   * acquires the event from the client
   * and deserializes the data structures
   */
  if(!this->serve()) {
      log_dev->errorStream() << "eventStatusListener::acceptJobStatus() - ErrorCode=["
                             << this->getErrorCode() << "]"
                             << log4cpp::CategoryStream::ENDLINE;

      log_dev->errorStream() << "eventStatusListener::acceptJobStatus() - ErrorMessage=["
                             << this->getErrorMessage() << "]" 
                             << log4cpp::CategoryStream::ENDLINE;
  }

  /**
   * Checks if the SOAP message was correct (i.e.
   * containing a non-null topic)
   */
  if(!this->getEventTopic()) {
      log_dev->fatal( "eventStatusListener::acceptJobStatus() - NULL Topic received. Stop!" );
    exit(1);
  }

  /**
   * Ignores notifications generated by non-ICE topics
   */
  if(this->getEventTopic()->Name != conf->getICETopic())
    {
        log_dev->warnStream() << "eventStatusListener::acceptJobStatus() - "
                              << "Received a notification with TopicName="
                              << this->getEventTopic()->Name
                              << " that differs from the ICE's topic official name. Ignoring"
                              << log4cpp::CategoryStream::ENDLINE;

      this->reset();
      return;
    }
  const vector<Event>& evts = this->getEvents();

  /**
   * Loops over all events (event <-> job)
   * For each event updates the status of the related
   * job in the jobCache.
   */
  for(unsigned j=0; j<evts.size(); j++) {

    cout << "evt #"<<j<<".Message[0]"<<evts[j].Messages[0]<<endl;
    cout << "evt #"<<j<<".Message[1]"<<evts[j].Messages[1]<<endl;
    cout << "evt #"<<j<<".Message[2]"<<evts[j].Messages[2]<<endl;

    string Ad = evts[j].Messages[0];
    string cream_job_id(""), status("");
    parseEventJobStatus(cream_job_id, status, Ad);
    jobCache::iterator it;
    try {
      it = jobCache::getInstance()->lookupByCreamJobID(cream_job_id);
      (*it).setLastUpdate(time(NULL));
      (*it).setStatus(glite::ce::cream_client_api::job_statuses::getStatusNum(status));
      jobCache::getInstance()->put( *it );
    } catch(exception& ex) {
	log_dev->fatal( ex.what() );
	exit(1);
    }

    log_dev->infoStream() << "eventStatusListener::acceptJobStatus() - "
                          << "Successfully updated JobID="
                          << cream_job_id <<" - STATUS=" << status
                          << log4cpp::CategoryStream::ENDLINE;
  }
  /**
   * See CEConsumer's class for the purpose of this method
   */
  this->reset();
}

//______________________________________________________________________________
void iceUtil::eventStatusListener::init(void)
{
  /**
   * This method is executed ONLY at startup of the listener
   * it collects all jobids, extracts the CEUrl they belong to,
   * checks for each CEUrl if the current ICE is subscribed to it.
   */
  map< string , int > tmpMap;
  string ceurl;
  ostringstream hostport;
  for(jobCache::iterator it=jobCache::getInstance()->begin();
      it != jobCache::getInstance()->end();
      it++)
    {
      ceurl = it->getCreamURL();
      boost::replace_first(ceurl,
                           conf->getCreamUrlPostfix(),
                           conf->getCEMonUrlPostfix());
      tmpMap[ceurl] = 1;
    }

  /**
   * Now we've got a collection of CEMon urls (without duplicates,
   * thanks to the map's property) we've to check for subscription
   */
  vector<Subscription> vec;
  vec.reserve(100);

  for(map<string, int>::iterator it = tmpMap.begin();
      it!=tmpMap.end();
      it++)
    {
      try {
          log_dev->infoStream() << "eventStatusListener::init() "
	                        << "- Authenticating with proxy ["
                                << proxyfile << "]"
                                << log4cpp::CategoryStream::ENDLINE;

	subManager.authenticate(proxyfile.c_str(), "/");
	vec.clear();
	log_dev->infoStream() << "eventStatusListener::init() - "
                              << "Getting list of subscriptions from ["
                              << it->first << "]"
                              << log4cpp::CategoryStream::ENDLINE;

	subManager.list(it->first, vec);
      } catch(AuthenticationInitException& ex) {
          log_dev->fatalStream() << "eventStatusListener::init() - "
	                         << ex.what()
				 << log4cpp::CategoryStream::ENDLINE;
	exit(1);
      } catch(exception& ex) {
          log_dev->fatalStream() << "eventStatusListener::init() - "
	                         << ex.what()
				 << log4cpp::CategoryStream::ENDLINE;
	exit(1);
      }
      if(!vec.size())
	{
	  /**
	   * there're no subs in that CEMon; so for sure we've to
	   * subscribe to it
	   */
	  ostringstream myname_url("");
	  myname_url << "http://" << myname << ":"
		     << conf->getListenerPort();
	  try {
	    subscriber.authenticate(proxyfile.c_str(), "/");
	    subscriber.setServiceURL(it->first);
	    subscriber.setSubscribeParam(myname_url.str().c_str(),
	                                 T,
					 P,
					 conf->getSubscriptionDuration()
					 );
            log_dev->infoStream() << "eventStatusListener::init() - Subscribing the consumer ["
                                  << myname_url.str() << "] to ["<<it->first
                                  << "] with duration="
                                  << conf->getSubscriptionDuration()
                                  << " secs"
                                  << log4cpp::CategoryStream::ENDLINE;
	    subscriber.subscribe();
	    log_dev->infoStream() << "eventStatusListener::init() - "
                                  << "Subscribed with ID ["
                                  << subscriber.getSubscriptionID() << "]"
                                  << log4cpp::CategoryStream::ENDLINE;

	    subscriptionCache::getInstance()->insert(it->first);
	  } catch(AuthenticationInitException& ex) {
              log_dev->fatalStream() << "eventStatusListener::init() - AuthN Error: "
	                             << ex.what() << log4cpp::CategoryStream::ENDLINE;
	    exit(1);
	  } catch(exception& ex) {
              log_dev->fatalStream() << "eventStatusListener::init() - Subscription Error: "
	                             << ex.what() << log4cpp::CategoryStream::ENDLINE;
	    exit(1);
	  }
	} else 
	{
	  vector<string> _tmp; _tmp.reserve(2);
	  char name[256];
	  bool subscribed = false;
	  string consumerUrl;
	  for(vector<Subscription>::iterator sit=vec.begin();
	      (sit != vec.end()) && (!subscribed);
	      sit++)
	    {
	      _tmp.clear();
	      consumerUrl = (*sit).getConsumerURL();
	      boost::replace_first(consumerUrl, "https://", "");
	      boost::replace_first(consumerUrl, "http://", "");
	      boost::split(_tmp, consumerUrl, boost::is_any_of(":"));
	      memset((void*)name, 0, 256);
	      if(gethostname(name, 256) == -1)
		{
                    log_dev->fatalStream() << "eventStatusListener::init() - "
                                           << "Couldn't resolve local hostname:"
                                           << strerror(errno)
                                           << log4cpp::CategoryStream::ENDLINE;
		  exit(1);
		}
	      struct hostent *H = gethostbyname(_tmp[0].c_str());
	      if(!H) {
                  log_dev->fatalStream() << "eventStatusListener::init() - "
                                         << "Couldn't resolve subscribed consumer's hostname: "
                                         << strerror(h_errno)
                                         << log4cpp::CategoryStream::ENDLINE;
		exit(1);
	      }
	      ostringstream consumerUrlOS("");
	      consumerUrlOS << H->h_name << ":" << _tmp[1];
	      consumerUrl = consumerUrlOS.str();
	      hostport.str("");
	      hostport << myname << ":" << conf->getListenerPort();
	      if( (consumerUrl==hostport.str()) &&
		  ((*sit).getTopicName()==conf->getICETopic()) )
		{
		  /**
		   * this ICE is subscribed to CEMon
		   */
		  subscribed = true;
		  log_dev->infoStream() << "eventStatusListener::init() - "
                                        << "Already subscribed to ["
					<< it->first << "]"
					<< log4cpp::CategoryStream::ENDLINE;
		  //cemon_subscribed_to[it->first] = true;
		  continue;
		}

	    }
	  if(!subscribed) {
              log_dev->infoStream() << "eventStatusListener::init() - "
                                    << "Must subscribe to [" << it->first
                                    << "]" << log4cpp::CategoryStream::ENDLINE;
	    subscriber.setServiceURL(it->first);
	    subscriber.setSubscribeParam(hostport.str().c_str(),
	                                 T,
					 P,
					 conf->getSubscriptionDuration());
	    log_dev->infoStream() << "eventStatusListener::init() - "
                                  << "Subscribing the consumer ["
                                  << hostport.str() << "] to ["<<it->first
                                  << "] with duration="
                                  << conf->getSubscriptionDuration()
                                  << " secs" 
                                  << log4cpp::CategoryStream::ENDLINE;
	    try {
	      subscriber.subscribe();
	      log_dev->infoStream() << "eventStatusListener::init() - "
                                    << "Subscription succesfull: ID="
                                    << subscriber.getSubscriptionID()<<"]"
                                    << log4cpp::CategoryStream::ENDLINE;
	      //cemon_subscribed_to[it->first] = true;
	      subscriptionCache::getInstance()->insert(it->first);
	    } catch(exception& ex) {
                log_dev->fatalStream() << "eventStatusListener::init() - "
		                       << ex.what()
				       << log4cpp::CategoryStream::ENDLINE;
	      exit(1);
	    }
	  }
	}
    }
}

//______________________________________________________________________________
void parseEventJobStatus(string& cid, string& st, const string& _classad)
  throw(iceUtil::ClassadSyntax_ex&)
{
  classad::ClassAdParser parser;
  classad::ClassAd *ad = parser.ParseClassAd( _classad );

  if (!ad)
    throw iceUtil::ClassadSyntax_ex("The classad describing the job status has syntax error");

  if ( !ad->EvaluateAttrString( "CREAM_JOB_ID", cid ) )
    throw iceUtil::ClassadSyntax_ex("CREAM_JOB_ID attribute not found, or is not a string");

  if ( !ad->EvaluateAttrString( "JOB_STATUS", st ) )
    throw iceUtil::ClassadSyntax_ex("JOB_STATUS attribute not found, or is not a string");
}
