/*
 * File: MatchingPipe.cpp
 * Author: MarcoP <marco.pappalardo@ct.infn.it>
 * Copyright (c) 2004 EGEE
 */
 
// $Id

#include "MatchingPipe.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h> /* for unlink, read, write, and close */
#include <stdio.h>  /* for perror */

namespace glite {
namespace wms {
namespace manager {
namespace ns {
namespace commands {

/**
 * A PipeWrapper for listmatch results retrieval.
 *
 * @version
 * @date November 18 2004
 * @author Marco Pappalardo
 */
  // int pipefd;
  // bool ispipeopen;
  // std::string pipepath;

  /**
   * Constructor.
   */
  MatchingPipe::MatchingPipe(std::string &path) {
    // Remove any previous FIFO.
    unlink(path.c_str());
    pipepath = path;
  }

  /**
   * Destructor
   */
  MatchingPipe::~MatchingPipe() {
    unlink(pipepath.c_str());
  }

  /**
   * Opens the pipe.
   * @return a boolean indicating whether the command was successful or not.
   */
  bool MatchingPipe::open(){
    // Create the FIFO.
    ispipeopen = (mkfifo(pipepath.c_str(), 0666) < 0 ? false : true);
    if (ispipeopen) {
      /*
       * Open the FIFO for reading.
       */
      if ((pipefd = ::open(pipepath.c_str(), O_RDONLY)) < 0) {
	ispipeopen = false;
      }
    }
    return ispipeopen;
  }
  
  /**
   * Closes the pipe.
   * @return a boolean indicating whether the command was successful or not.
   */
  bool MatchingPipe::close() {
    if (ispipeopen) {
      ispipeopen= !(::close(pipefd));
    }
    return !ispipeopen;
  }

  /**
   * Read the pipe containt.
   * @return a string representation of return values taken from the pipe.
   */
  std::string MatchingPipe::read() {
    char buf[5120];     // 5KB buffer
    std::string buffer; // the string each loop appends to
    int n=0;            // read size
    
    while ((n = ::read(pipefd, buf, sizeof(buf))) > 0 ) {
      buffer.append(std::string(buf));
    }
    // classad::ClassAdParser parser;
    // classad::ClassAd* jdlad = parser.ParseClassAd(buffer);
    buf[0]='\0';
    return buffer;
  }

} // namespace commands
} // namespace ns
} // namespace manager
} // namespace wms
} // namespace glite
