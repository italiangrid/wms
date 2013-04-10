#include <boost/lexical_cast.hpp>

#define edglog(level) glite::wms::common::logger::threadsafe::edglog<<glite::wms::common::logger::setlevel(glite::wms::common::logger::level)
#define edglog_fn(name) glite::wms::common::logger::StatePusher pusher(glite::wms::common::logger::threadsafe::edglog, "PID: " + boost::lexical_cast<std::string>(getpid()) + " - " + #name)
#define glitelogTag(level) glite::wms::common::logger::threadsafe::edglog<<glite::wms::common::logger::setlevel(glite::wms::common::logger::level)<<"*********"
#define glitelogHead(level) glite::wms::common::logger::threadsafe::edglog<<glite::wms::common::logger::setlevel(glite::wms::common::logger::level)<<"* Error *"
#define glitelogBody(level) glite::wms::common::logger::threadsafe::edglog<<glite::wms::common::logger::setlevel(glite::wms::common::logger::level)<<"*       *"

