/*
 * File: NetworkServerReal.h
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 * Author: Marco Pappalardo <Marco.Pappalardo@ct.infn.it>
 * Copyright (c) 2001 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */
 
#ifndef _GLITE_WMS_MANAGER_NS_DAEMON_NETWORKSERVERREAL_H_
#define _GLITE_WMS_MANAGER_NS_DAEMON_NETWORKSERVERREAL_H_


#include <boost/shared_ptr.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/mutex.hpp>
#include "NetworkServerImpl.h"

namespace glite {

namespace wmsutils {
namespace tls {
namespace socket_pp {
class GSISocketServer;
}
}
}

namespace wms {
namespace common {
namespace configuration {
  class Configuration;
  class NSConfiguration;
} 
}


namespace manager {
namespace ns {
namespace daemon {

namespace socket_pp     = wmsutils::tls::socket_pp;
namespace configuration = wms::common::configuration;

typedef boost::shared_ptr<socket_pp::GSISocketServer> GSISocketServerPtr;
 
class NetworkServerReal : public NetworkServerImpl  
{
  // non-copyable
  NetworkServerReal(const NetworkServerReal& nsi);
  NetworkServerReal& operator=(const NetworkServerReal& nsi);
  
public:
  NetworkServerReal();
  virtual ~NetworkServerReal();
  
  void run();
  void init();
  void shutdown();
  void drop_privileges();
  const configuration::NSConfiguration& configuration();
  void do_daemonize();
  void set_daemonize(bool flag);
  void set_privileges(bool flag);
  bool do_clearpipes();

public:
  GSISocketServerPtr UIserver;
  const configuration::NSConfiguration* NSconf;

private:
  boost::condition  do_shutdown;
  boost::mutex     mtx_shutdown;
  std::auto_ptr<configuration::Configuration>    conf;
  bool           daemonize_flag;
  bool          privileges_flag;
};

} // namespace daemon
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite

#endif

