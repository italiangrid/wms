/*
 * File: listfiles.h
 * Author: Marco Pappalardo <Marco.Pappalardo@ct.infn.it>
 * Copyright (c) 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// $Id$ 

#ifndef _GLITE_WMS_MANAGER_NS_COMMANDS_LOGGING_H_
#define _GLITE_WMS_MANAGER_NS_COMMANDS_LOGGING_H_

#define edglog(level) logger::threadsafe::edglog << logger::setlevel(logger::level) 
#define edglog_fn(name) logger::StatePusher    pusher(logger::threadsafe::edglog, #name); 

#endif
