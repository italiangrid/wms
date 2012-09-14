// File: broker_helper.h
// Author: Francesco Giacomini

// $Id$

#ifndef GLITE_WMS_BROKER_HELPER_H
#define GLITE_WMS_BROKER_HELPER_H

namespace glite {
namespace wms {
namespace broker {

boost::shared_ptr<classad::ClassAd>
broker_helper_resolve(boost::shared_ptr<classad::ClassAd> ad);

}}}

#endif
