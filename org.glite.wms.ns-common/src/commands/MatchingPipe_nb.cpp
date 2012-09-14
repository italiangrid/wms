/*
 * File: MatchingPipe_nb.cpp
 * Author: MarcoP <marco.pappalardo@ct.infn.it>
 * Copyright (c) 2005 EGEE
 */
 
// $Id

#include "MatchingPipe_nb.h"
#include <boost/lexical_cast.hpp>
#include <exception>
namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace commands {

/**
 * A Non-blocking PipeWrapper for listmatch results retrieval.
 *
 * @version
 * @date November 18 2004
 * @author Marco Pappalardo
 */

  /**
   * Constructor.
   */
  //  MatchingPipe_nb::MatchingPipe_nb(std::string &path) {
    // Remove any previous FIFO.
    //unlink(path.c_str());
    //pipepath = path;
  //}

  /**
   * Opens the pipe.
   * @return a boolean indicating whether the command was successful or not.
   */
  bool MatchingPipe_nb::open(){
    // Create the FIFO.
    ispipeopen = (mkfifo(pipepath.c_str(), 0666) < 0 ? false : true);
    if (ispipeopen) {
      /*
       * Open the FIFO for reading.
       */
      if ((pipefd = ::open(pipepath.c_str(), O_RDONLY | O_NONBLOCK )) < 0) {
	ispipeopen = false;
      }
    }
    return ispipeopen;
  }
  
  /**
   * Read the pipe containt. Non blocking.
   * @return a string representation of return values taken from the pipe.
   */
  std::string MatchingPipe_nb::read() {

    std::string buffer; // the string each loop appends to
    bool done = false;

    while (!done) {
 
      // wait for input to be available
      fd_set read_fds;
      FD_ZERO(&read_fds);
      FD_SET(pipefd, &read_fds);
      while (select(pipefd + 1, &read_fds, 0, 0, 0) < 0) {
	if (errno == EINTR) {
	  continue;
	} else {
	  throw std::string( "select failed with errno " +
			     boost::lexical_cast<std::string>(errno));
	}
      }
      // read available input
      char buf[5120];
      int nread;
      while ((nread = ::read(pipefd, buf, 5120)) < 0) {
	if (errno == EINTR) {
	  continue;
	} else {
	  throw std::string( "select failed with errno " +
			     boost::lexical_cast<std::string>(errno));
	}
      }
      if (nread == 0) {           // EOF
	done = true;
      } else {
	buffer.append(buf, buf + nread);
      }
    }
    // buf[0]='\0';
    return buffer;
  }

} // namespace commands
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite
