/*
 * File: MatchingPipeFiller.h
 * Author: Marco Pappalardo
 * Copyright (c) 2004 EGEE.
 */

// $Id
#ifndef _GLITE_WMS_MANAGER_NS_COMMANDS_MATCHINGPIPEFILLER_H_
#define _GLITE_WMS_MANAGER_NS_COMMANDS_MATCHINGPIPEFILLER_H_

#include <classad_distribution.h>
#include <string>
#include <boost/utility.hpp>
#include <boost/scoped_ptr.hpp>

namespace classad {
  class ClassAd;
}

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace commands {

class MatchingPipeFiller
{
  int pipefd;
  bool ispipeopen;
  std::string pipepath;

public:
  /*
   * Contructor.
   */
  MatchingPipeFiller(std::string& path);
  /*
   * Destructor.
   */
  virtual ~MatchingPipeFiller();
  
  /*
   * Initializes the PipeWrapper filler opening the pipe.
   * @param path the pathname of the pipe.
   * @return true on success, false otherwise.
   */
  bool open();

  /*
   * Closes the PipeWrapper filler.
   * @return true on success, false otherwise.
   */
  bool close();

  /*
   * Writes a message onto the pipe.
   * @param message the message to be written.
   */
  void write(std::string message);

};

} // namespace commands
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite

#endif

// Local Variables:
// mode: c++
// End:






