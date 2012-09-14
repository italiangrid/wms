/*
 * File: MatchingPipeFiller.cpp
 * Author: MarcoP <marco.pappalardo@ct.infn.it>
 * Copyright (c) 2004 EGEE.
 */

// $Id

#include "MatchingPipeFiller.h"
#include <classad_distribution.h>
#include <string>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> /* for read, write, and close */
#include <stdio.h> /* for perror */

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace commands {

MatchingPipeFiller::MatchingPipeFiller(std::string& path)
{
  pipepath = path;
}

MatchingPipeFiller::~MatchingPipeFiller()
{
}

bool MatchingPipeFiller::open()
{
  /*
   * Open the FIFO for writing.  It was
   * created by the NS FSM.
   */
  ispipeopen = !((pipefd = ::open(pipepath.c_str(), O_WRONLY)) < 0);
  return ispipeopen;
}

bool MatchingPipeFiller::close ()
{
  if (ispipeopen) {
    ispipeopen = !(::close(pipefd));
  }
  return !ispipeopen;
}

void MatchingPipeFiller::write (std::string message)
{
  if (ispipeopen) {
    ::write(pipefd, message.c_str(), sizeof(message.c_str()));
  }
}


} // namespace commands
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite


