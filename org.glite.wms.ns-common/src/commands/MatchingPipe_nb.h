/*
 * File: MatchingPipe_nb.h
 * Author: MarcoP <marco.pappalardo@ct.infn.it>
 * Copyright (c) 2005 EGEE.
 */
 
// $Id

#ifndef _GLITE_WMS_MANAGER_NS_COMMANDS_MATCHINGPIPE_NB_H_
#define _GLITE_WMS_MANAGER_NS_COMMANDS_MATCHINGPIPE_NB_H_

#include "MatchingPipe.h" 

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace commands {

/**
 * A Non blocking PipeWrapper for listmatch results retrieval.
 *
 * @version
 * @date April 28 2005
 * @author Marco Pappalardo
 */
  class MatchingPipe_nb : public commands::MatchingPipe
{

 public:
  /**
   * Constructor.
   */
  MatchingPipe_nb(std::string &path) : commands::MatchingPipe( path ) {}
 
  /**
   * Opens the pipe for non blocking read.
   * @return a boolean indicating whether the command was successful or not.
   */
  bool open();
  
  /**
   * Read the pipe containt. Non blocking.
   * @return a string representation of return values taken from the pipe.
   */
  std::string read();
};
  
} // namespace commands
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite

#endif
