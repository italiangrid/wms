// File: checkpointing.h
// Author: Alessio Gianelle <gianelle@pd.infn.it>
// Copyright (c) 2002 EU DataGrid.
// For license conditions see http://www.eu-datagrid.org/license.html

// $Id$

/**
 * \file
 * \brief Provides an API to realize a trivial checkpointing service. 
 * 
 * With "trivial checkpoint" we mean the possibility for the user to 
 * save and to manage a well-defined State object which describes the 
 * job and what it has done until the moment in which the State object 
 * is saved. It is also possible to restart a job from an intermediate 
 * point using a previously saved State.
 * \see http://edms.cern.ch/document/347730/1 
 *
 * \version 0.2
 * \date 18 December 2002
 * \author Alessio Gianelle <gianelle@pd.infn.it>
*/

#ifndef GLITE_WMS_CHECKPOINTING_CHKPT_H
#define GLITE_WMS_CHECKPOINTING_CHKPT_H

#include <glite/wms/checkpointing/step.h>
#include <glite/wms/checkpointing/stepper.h>
#include <glite/wms/checkpointing/jobstate.h>
#include "glite/wms/checkpointing/ChkptException.h"

#endif // GLITE_WMS_CHECKPOINTING_CHKPT_H

//  Local Variables:
//  mode: c++
//  End:










