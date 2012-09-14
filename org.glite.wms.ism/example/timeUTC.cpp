/* Copyright (c) Members of the EGEE Collaboration. 2004.
See http://www.eu-egee.org/partners/ for details on the copyright
holders.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License. */
#include <boost/thread/xtime.hpp>
#include <boost/thread/thread.hpp>
#include <iostream>

struct thread_alarm 
{
  thread_alarm(int secs) : m_secs(secs) {}
  void operator()()
  {
    boost::xtime xt;
    boost::xtime_get(&xt, boost::TIME_UTC);
    std::cout << "sec..." << xt.sec << std::endl;
    std::cout << "nsec..." << xt.nsec << std::endl;
    std::cout << "UTC" << static_cast<int>(boost::TIME_UTC) << std::endl;

    xt.sec += m_secs;
    boost::thread::sleep(xt);

    std::cout << "alarm sounded..." << std::endl;
    std::cout << "sec..." << xt.sec << std::endl;
    std::cout << "nsec..." << xt.nsec << std::endl;
    std::cout << "UTC" << static_cast<int>(boost::TIME_UTC) << std::endl;
  }
  int m_secs;
};

int main(int argc, char* argb[])
{
  int secs = 5;
  std::cout << "setting alarm for 5 seconds..." << std::endl;
  thread_alarm alarm(secs);
  boost::thread thrd(alarm);
  thrd.join();
}
