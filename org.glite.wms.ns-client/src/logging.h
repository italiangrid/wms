/*
 * File: logging.h
 * Author: Marco Pappalardo <Marco.Pappalardo@ct.infn.it>
 * Copyright (c) 2002 EU DataGrid.
 * For license conditions see http://www.eu-datagrid.org/license.html
 */

// $Id$ 

#define edglog(level) logger::threadsafe::edglog << logger::setlevel(logger::level) 
#define edglog_fn(name) logger::StatePusher    pusher(logger::threadsafe::edglog, #name); 

