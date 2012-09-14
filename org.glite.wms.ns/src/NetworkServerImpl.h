/*
 * File: NetworkServerImpl.h
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 * Author: Marco Pappalardo <Marco.Pappalardo@ct.infn.it>
 * Copyright (c) 2001 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */
 
#ifndef _GLITE_WMS_MANAGER_NS_DAEMON_NETWORKSERVERIMPL_H_
#define _GLITE_WMS_MANAGER_NS_DAEMON_NETWORKSERVERIMPL_H_

#include <boost/utility.hpp>
 
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

class NetworkServerImpl : boost::noncopyable 
{
public:
  NetworkServerImpl();
  virtual ~NetworkServerImpl();

  virtual void run()                                                    = 0;
  virtual void init()                                                   = 0;
  virtual void shutdown()                                               = 0;
  virtual const common::configuration::NSConfiguration& configuration() = 0;
  virtual void drop_privileges()                                        = 0;
  virtual void do_daemonize()                                           = 0;
  virtual void set_daemonize(bool flag)                                 = 0;
  virtual void set_privileges(bool flag)                                = 0;
};
} // namespace daemon
} // namespace ns
} // namespace manager
} // namesapce wms
} // namespace glite

#endif
