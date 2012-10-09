##############################################################################
#
# NAME:        scheduler.py
#
# FACILITY:    SAM (Service Availability Monitoring)
#
# DESCRIPTION:
#
#         Threaded scheduler.
#
# AUTHORS:     Konstantin Skaburskas, CERN
#
# CREATED:     Mar 7, 2010
#
# NOTES:
#
# MODIFIED:
#
##############################################################################

"""
threaded scheduler.

Threaded scheduler.

Konstantin Skaburskas <konstantin.skaburskas@cern.ch>, CERN
SAM (Service Availability Monitoring)
"""

import time

import thread
mutex = thread.allocate_lock()
threads_pool = {}

def de_queue(h):
    mutex.acquire()
    del threads_pool[h]
    mutex.release()

class SchedulerThread:
    def __init__(self, max_threads=10, default_timeout=300):
        self.max_threads = max_threads
        self.default_timeout = default_timeout

    def check_threads(self):
        global threads_pool
        t = time.time()

        for h, v in threads_pool.items():
            if t > v['t']:
                try:
                    # kill processes spawned by the thread
                    # we need a global object holding PIDs
                    print "Thread timeout. Killing processes spawned by : ",h
#                    os.kill(-pid, signal.SIGTERM)
#                    os.waitpid(pid, 0)
                except StandardError:
                    pass
                del threads_pool[h]
#            else:
#                if os.waitpid(pid,os.WNOHANG)[0]:
#                    del self.threads_pool[h]

    def wait(self):
        global threads_pool
        self.check_threads()
        while threads_pool:
            time.sleep(1)
            self.check_threads()

    def run(self, func, jd, host, timeout=None):
        global threads_pool
        self.check_threads()
        while len(threads_pool) >= self.max_threads:
            time.sleep(1)
            self.check_threads()

        tm = timeout or self.default_timeout
        threads_pool[host] = {'jd' : jd,
                              't'  : time.time()+tm}
        thread.start_new_thread(func, (jd,))
