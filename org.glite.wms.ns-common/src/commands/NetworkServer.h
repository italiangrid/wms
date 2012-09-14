/*
 * File: NetworkServer.h
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 * Author: Marco Pappalardo <Marco.Pappalardo@ct.infn.it>
 * Copyright (c) 2001 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */
 
#ifndef _GLITE_WMS_MANAGER_NS_DAEMON_NETWORKSERVER_H_
#define _GLITE_WMS_MANAGER_NS_DAEMON_NETWORKSERVER_H_

#include <boost/utility.hpp>
#include <boost/pool/detail/singleton.hpp>
#include "glite/wms/common/configuration/NSConfiguration.h"


using namespace boost::details::pool;

namespace glite {
namespace wms {
namespace common {
namespace configuration {
class NSConfiguration;
}
}

namespace manager {
namespace ns {
namespace daemon {

class NetworkServerImpl;

class NetworkServer : boost::noncopyable
{ 
 public:
  NetworkServer();
  ~NetworkServer();
  
  void run();
  void init();
  void shutdown();
  void drop_privileges();
  void do_daemonize();
  void set_daemonize(bool flag);
  void set_privileges(bool flag);
  const common::configuration::NSConfiguration& configuration();
 
 private:
  NetworkServerImpl *m_impl;
};

} // daemon
} // ns
} // manager 
} // wms
} // glite

#endif
