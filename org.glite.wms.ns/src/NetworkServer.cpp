/*
 * File: NetworkServer.cpp
 * Author: Monforte Salvatore <Salvatore.Monforte@ct.infn.it>
 * Copyright (c) 2001 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

#include "glite/wms/ns-common/NetworkServer.h"
#include "NetworkServerImpl.h"
#include "NetworkServerReal.h"

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace daemon {

namespace configuration = wms::common::configuration;
 
static NetworkServerImpl* create_ns_impl();

NetworkServer::NetworkServer() : 
  m_impl(create_ns_impl())
{
}
 
NetworkServer::~NetworkServer()
{
 delete m_impl;
}
 
void NetworkServer::run()
{
 m_impl -> run();
}

void NetworkServer::init()
{
 m_impl -> init();
}

void NetworkServer::shutdown()
{
 m_impl -> shutdown();
}

void NetworkServer::drop_privileges()
{
 m_impl -> drop_privileges();
}

void NetworkServer::do_daemonize()
{
 m_impl -> do_daemonize();
}

void NetworkServer::set_daemonize(bool flag)
{
 m_impl -> set_daemonize(flag);
}

void NetworkServer::set_privileges(bool flag) {
  m_impl -> set_privileges(flag);
}

const configuration::NSConfiguration& NetworkServer::configuration() 
{
 return m_impl -> configuration();
}

NetworkServerImpl* create_ns_impl()
{
 return new NetworkServerReal;
}

} // namespace daemon
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite
