##############################################################################
#
# NAME:        wmsmetrics.py
#
# FACILITY:    NAGIOS probes
#
# COPYRIGHT:
#         Copyright (c) 2009-2011, Members of the EGEE Collaboration.
#         http://www.eu-egee.org/partners/
#         Licensed under the Apache License, Version 2.0.
#         http://www.apache.org/licenses/LICENSE-2.0
#         This software is provided "as is", without warranties
#         or conditions of any kind, either express or implied.
#
# DESCRIPTION:
#
#         Module containing Nagios WMS metrics.
#         Multi-threaded version.
#
# AUTHORS:     Konstantin Skaburskas, CERN
#              Alessio Gianelle, INFN
#
# CREATED:     08-Oct-2009
#
# NOTES:
#
# MODIFIED:
#    2011-09-15 : Alessio Gianelle
#         - porting to EMI
# 
#
##############################################################################

"""
Nagios WMS metrics.

Module containing Nagios WMS metrics.
Multi-threaded version.

Konstantin Skaburskas <konstantin.skaburskas@cern.ch>, CERN
SAM (Service Availability Monitoring)
"""

import os
import sys
import time #@UnresolvedImport
import re
import shutil
import random
import urllib
import getopt

try:
    from gridmon import probe
    from gridmon import utils as samutils
    from gridmon.nagios.perfdata import PerfData
    from wmsmetrics import scheduler as sched
except ImportError,e:
    summary = "UNKNOWN: Error loading modules : %s" % (e)
    sys.stdout.write(summary+'\n')
    sys.stdout.write(summary+'\nsys.path: %s\n'% str(sys.path))
    sys.exit(3)

# for threaded metricJobMonit()
gridjobs_states = {}

class WMSGenericMetrics(probe.MetricGatherer) :
    """A Metric Gatherer generic for WMS."""

    # The probe's author name space
    ns = 'emi.wms'

    # Timeouts
    _timeouts = {#'JOB_BABYSITTING' : 30,
                 'JOB_WAITING'  : 60*45,
                 'JOB_SUBMTTED' : 60*45,
                 'JOB_READY'    : 60*45,
                 'JOB_GLOBAL'   : 60*60,
                 'JOB_DISCARD'  : 60*60*2
                 }

    # host to be tested
    hostName   = None
    portNumber = '7443'

    prev_status = ''

    # Probe info
    probeinfo = { 'probeName'      : ns+'.WMS-Probe',
                  'probeVersion'   : '1.0',
                  'serviceVersion' : '*'}
    # Metrics' info
    _metrics = {
           'JobState' : {
                      'metricDescription': "Submit a grid job to WMS.",
                      'cmdLineOptions'   : ['namespace=',
                                            'ces-file=',
                                            'jdl-templ=',
                                            'jdl-retrycount=',
                                            'jdl-shallowretrycount=',
                                            'no-submit',
                                            'prev-status='],
                      'cmdLineOptionsReq' : [],
                      'metricChildren'   : [],
                      'statusMsgs'       : {
                                    'OK'               : '',
                                    'WARNING'   : 'Problem with job submission.',
                                    'CRITICAL'     : 'Problem with job submission.',
                                    'UNKNOWN' : 'Problem with job submission.'},
                      'metricVersion'     : '0.1'
                      },

           'JobMonit' : {
                      'metricDescription': "Babysit submitted grid jobs.",
                      'cmdLineOptions'   : ['namespace=',
                                            'timeout-job-global=',
                                            'timeout-job-waiting=',
                                            'hosts='],
                      'cmdLineOptionsReq' : [],
                      'metricChildren'   : [],
                      'metricVersion'     : '0.1'
                      },
           'JobCancel' : {
                      'metricDescription': "Cancel grid jobs.",
                      'cmdLineOptions'   : ['namespace='],
                      'cmdLineOptionsReq' : [],
                      'metricChildren'   : [],
                      'metricVersion'     : '0.1'
                      }
           }

    # <submitTimeStamp>|<hostNameCE>|<serviceDesc>|<jobID>|<jobStatus>|<lastStatusTimeStamp>
    activejobattrs = ['submitTimeStamp',
                      'hostNameCE',
                      'serviceDesc',
                      'jobID',
                      'jobStatus',
                      'lastStatusTimeStamp']

    jobstates = {'all':['SUBMITTED',
                        'WAITING',
                        'READY',
                        'SCHEDULED',
                        'RUNNING',
                        'DONE',
                        'ABORTED',
                        'CLEARED'],
                 'terminal':['DONE',
                             'ABORTED',
                             'CLEARED'],
                 'non_terminal':['SUBMITTED',
                                 'WAITING',
                                 'READY',
                                 'SCHEDULED',
                                 'RUNNING']}

    gridjobcmd = {'SUBMIT'  : 'glite-wms-job-submit -a',
                  'STATUS'  : 'glite-wms-job-status',
                  'OUTPUT'  : 'glite-wms-job-output --noint --nosubdir',
                  'CANCEL'  : 'glite-wms-job-cancel --noint',
                  'LOGGING' : 'glite-wms-job-logging-info -v 2'}

    # Order of performance data for JobMonit metric.
    jm_perf_data_order = ('jobs_processed', 'DONE', 'RUNNING', 'SCHEDULED',
                          'SUBMITTED', 'READY', 'WAITING',
                          'ABORTED', 'CANCELLED', 'CLEARED',
                          'MISSED', 'UNDETERMINED', 'unknown')

    # JDL Template file
    _fileJDLTemplateFN = 'WMS-jdl.template'
    _fileJDLTemplate = '%s/%s' % (
            os.path.normpath(os.path.dirname(os.path.abspath(sys.argv[0]))),
            _fileJDLTemplateFN)

    # JDL ClassAdds
    _jdlRetryCount = '0'
    _jdlShallowRetryCount = '1'

    # Do not submit job to CE
    _nosubmit = None

    _hosts = []

    def __init__(self, tuples, wmstype):

        probe.MetricGatherer.__init__(self, tuples, wmstype)

        # File with a list of "good" CEs
        self._fileGoodCEs = '%s/%s/GoodCEs' % (self.workdir_run,
                                               self.fqan_norm or self.voName)

        self.usage="""    Metrics specific parameters:

--namespace <string>    Name-space for the probe. (Default: %s)

%s
--jdl-templ <file>    JDL template file (full path). Default:
                      <emi.wms.ProbesLocation>/%s
--jdl-retrycount <val>          JDL RetryCount (Default: %s).
--jdl-shallowretrycount <val>   JDL ShallowRetryCount (Default: %s).
--ces-file <file>     File with list of CEs. Two schemes [file:] or http:
                      (Default: %s)
--prev-status <0-3>         Previous Nagios status of the metric.

%s
--timeout-job-global <sec>  Global timeout for jobs. Job will be canceled
                            and dropped if it is not in terminal state by
                            that time. (Default: %i)
--timeout-job-waiting <sec> Time allowed for a job to stay in Waiting with
                            'no compatible resources'. (Default: %i)
--hosts <h1,h2,..>  Comma-separated list of CE hostnames to run monitor on.

"""%(self.ns,
     self.ns+'.'+self.serviceType+'-JobState',
     self._fileJDLTemplateFN,
     self._jdlRetryCount,
     self._jdlShallowRetryCount,
     self._fileGoodCEs,
     self.ns+'.'+self.serviceType+'-JobMonit',
     self._timeouts['JOB_GLOBAL'],
     self._timeouts['JOB_WAITING'],
     )

        # initiate metrics description
        self.set_metrics(self._metrics)

        # parse command line parameters
        self.parse_cmd_args(tuples)

        # working directory for metrics
        self.make_workdir()

        # File to track status of the submitted job. Structure:
        # <submitTimeStamp>|<hostNameCE>|<serviceDesc>|<jobID>|<jobStatus>|<lastStatusTimeStamp>
        self._fileActiveJobName = 'activejob.map'
        self._fileActiveJob = self.workdir_metric+'/'+self._fileActiveJobName

        # Lock file
        self._fileLock = self.workdir_metric+'/lock'

        # Job output parameters
        self._dirJobOutputName = 'jobOutput'
        self._dirJobOutput = self.workdir_metric+'/'+self._dirJobOutputName
        self._fileJobID    = self.workdir_metric+'/jobID'
        self._fileJobOutputName = 'gridjob.out'

        # JDL
        self._fileJDL  = self.workdir_metric+'/gridJob.jdl'
        self._jdlPatterns = {
                            'jdlExecutable': '/bin/hostname',
                            #'jdlArguments': None,
                            'jdlReqCEInfoHostName': None,
                            'jdlRetryCount': self._jdlRetryCount,
                            'jdlShallowRetryCount': self._jdlShallowRetryCount,
                            }
        self.metJobSubmission = '%s.%s-JobSubmit-%s' % (self.ns,
                                                        self.serviceType,
                                                        self.fqan or self.voName)
        self.metJobState = '%s.%s-JobState-%s' % (self.ns, self.serviceType,
                                                  self.fqan or self.voName)

        if not self.portNumber:
            self.portNumber = '7443'

    def parse_args(self, opts):
        """Parse command line arguments relevant to the probe and metrics.
        """

        for o,v in opts:
            if o == '--timeout-job-global':
                self._timeouts['JOB_GLOBAL'] = int(v)
            elif o == '--timeout-job-waiting':
                self._timeouts['JOB_WAITING'] = int(v)
            elif o == '--namespace':
                self.ns = v
            elif o == '--jdl-templ':
                self._fileJDLTemplate = v
            elif o == '--jdl-retrycount':
                self._jdlRetryCount = v
            elif o == '--jdl-shallowretrycount':
                self._jdlShallowRetryCount = v
            elif o == '--ces-file':
                self._fileGoodCEs = v
            elif o == '--no-submit':
                self._nosubmit = True
            elif o == '--hosts':
                self._hosts = [ x for x in v.split(',') if x ]
            elif o == '--prev-status':
                try:
                    self.prev_status = samutils.to_status(abs(int(v)))
                except ValueError:
                    raise getopt.GetoptError('--prev-status should be integer.')

    def _load_activejob(self, ajf=''):
        """Loads active job's description from formated file.
        """

        if not ajf:
            ajf = self._fileActiveJob

        self.printdvm("Loading active job's description ... ",
              cr=False)

        sched.mutex.acquire()
        vals = open(ajf).readline().strip().split('|')
        sched.mutex.release()
        if len(vals) == 1 and vals[0] == '':
            vals = []
        # we don't want neither less nor empty fields
        if len(self.activejobattrs) != len(vals):
            self.printdvm('failed.')
            output = 'ERROR: File format mismatch while reading '+ajf+'.\n'
            output += 'File should contain '+str(len(self.activejobattrs))+\
                ' elements, but '+str(len(vals))+' read.\n'
            return False, output
        elif [ True for v in vals if v == '' ]:
            self.printdvm('failed.')
            output = 'ERROR: File format mismatch while reading '+ajf+'.\n'
            output += 'One of the required fields is empty.\n'
            return False, output

        self.printdvm('done.')

        return True, dict(zip(self.activejobattrs, vals))


    def _save_activejob(self, data, ajf=''):
        """Saves active job's description to a file.
        - data - dictionary with self.activejobattrs fields
        """

        if not ajf:
            ajf = self._fileActiveJob

        self.printdvm("Saving active job's description ... ",
              cr=False)

        s = '|'.join([ data[k] for k in self.activejobattrs ])

        sched.mutex.acquire()
        open(ajf,'w').write(s)
        sched.mutex.release()

        self.printdvm('done.')

        return s

    def _js_do_cleanup(self, clean=True):
        if not clean:
            return
        for fn in [self._fileJobID,
                   self._fileJDL]:
            try:    os.unlink(fn)
            except StandardError: pass
        # keep last job's output sandbox
        try:
            if os.listdir(self._dirJobOutput):
                new_dir = self._dirJobOutput+'.LAST'
                shutil.rmtree(new_dir, ignore_errors=True)
                os.rename(self._dirJobOutput, new_dir)
        except StandardError: pass

    def _build_jdl(self):
        """
        Check if all JDL substitution patterns in the respective
        dictionary are set. Load JDL template file. Make required
        substitutions. Save JDL.
        """

        self.printdvm('>>> JDL patterns for template substitutions\n%s' %
            '\n'.join(['%s : %s'%(k,v) for k,v in self._jdlPatterns.items()]))

        patterns = {}
        for pattern,repl in self._jdlPatterns.items():
            if not repl:
                raise TypeError(
                        'JDL pattern <%s> not initialised while building JDL.' %
                        pattern)
            patterns['<%s>' % pattern] = repl

        try:
            _jdl_templ = open(self._fileJDLTemplate, 'r').read()
        except IOError, e:
            raise IOError('Unable to load JDL template %s. %s' %
                                        (self._fileJDLTemplate, str(e)))
        self.printdvm('>>> JDL template: \nfile: %s\n%s' %
                            (self._fileJDLTemplate, _jdl_templ))

        # do the needful
        for pattern,repl in patterns.items():
            _jdl_templ = re.sub(pattern, repl, _jdl_templ)

        self.printdvm('>>> Resulting JDL:\n%s' % _jdl_templ)

        self.printdvm('>>> Saving JDL to %s ... ' % self._fileJDL, cr=False)
        try:
            open(self._fileJDL, 'w').write(_jdl_templ)
        except IOError, e:
            raise IOError('Unable to save JDL %s. %s' %
                                        (self._fileJDL, str(e)))
        self.printdvm('done.')

    def metricJobCancel(self):
        """Cancel grid job and remove job bookkeeping files.
        """
        _cmd = self.gridjobcmd['CANCEL']+' %s'
        # job ID can be either in jobID or active job map files
        if os.path.exists(self._fileJobID):
            cmd = _cmd % (' -i %s' % self._fileJobID)
        elif os.path.exists(self._fileActiveJob):
            rc, jobdesc = self._load_activejob()
            if not rc:
                status = 'UNKNOWN'
                s = 'Problems loading active job file.'
                self.printd('%s\n%s' % (s, jobdesc))
                self.prints(s)
                return status
            cmd = _cmd % jobdesc['jobID']
        else:
            msg = 'UNKNOWN: no active job found'
            return 'UNKNOWN', msg, msg
        _rc, _stsmsg, _detmsg = self.run_cmd(cmd)
        self.printd('Job cancellation request sent:\n%s' % cmd)
        self.printdvm(_detmsg)
        for f in [self._fileJobID, self._fileActiveJob]:
            try: os.unlink(f)
            except Exception: pass
        self.printd('Job bookkeeping files deleted.')
        return 'OK', 'OK: job cancelled'

    def metricJobState(self):
        """Submit a grid job to CE.
        """

        # Clean up or exit gracefully if there is an active job.
        try:
            os.stat(self._fileActiveJob)
        except OSError:
            # Active job file was not found. Do cleanup.
            self._js_do_cleanup()
        else:
            rc, jobdesc = self._load_activejob()
            if not rc:
                status = 'UNKNOWN'
                s = 'Problems loading active job file.'
                self.printd('%s\n%s' % (s, jobdesc))
                self.prints(s)
                return status

            subtime     = int(jobdesc['submitTimeStamp'])
            jobsts      = jobdesc['jobStatus']
            lastststime = int(jobdesc['lastStatusTimeStamp'])

            # active job was found
            self.printdvm('>>> Active job found')
            self.printdvm('file: %s' % self._fileActiveJob)
            self.printdvm('\n'.join([ k+' : '+v for k,v in jobdesc.items()]))
            # Return if the job is not in terminal state, or else do cleanup and
            # proceed with job submission.
            if jobsts.upper() not in self.jobstates['terminal']:
                # Job is not in terminal state for more than 1h - discard the job.
                # Submit the job cancel request and remove active job file.
                if lastststime - subtime > 3600:
                    cmd = self.gridjobcmd['CANCEL']+' '+jobdesc['jobID']
                    self.printdvm('>>> Old job - cancel it\n%s' % cmd)
                    _rc, _stsmsg, _detmsg = self.run_cmd(cmd)
                    os.unlink(self._fileActiveJob)
                else:
                    status = self.prev_status or 'OK'
                    s = 'Active job - %s [%s]' % \
                        (jobsts, time.strftime("%Y-%m-%dT%H:%M:%SZ",
                                      time.gmtime(lastststime)))
                    self.prints(s)
                    return status
            else:
                try:
                    self.printdvm('>>> Deleting active job file.')
                    self.printdvm('The previously submitted job is in terminal state: %s\n' % \
                                jobdesc['jobStatus'])
                    self.printd('Deleting the file %s' % self._fileActiveJob, v=3)
                    os.unlink(self._fileActiveJob)
                except StandardError: pass
                self._js_do_cleanup()

        # TODO: global timeout on checking job's status is needed. 30 min?

        self.printdvm('>>> Preparing for job submission')

        # job's output directory
        if not os.path.isdir(self._dirJobOutput):
            try:
                os.makedirs(self._dirJobOutput)
            except OSError, e:
                status = 'UNKNOWN'
                stsmsg = detmsg = status+": OSError: %s" % e
                return (status, stsmsg, detmsg)

        wmsendpt = 'https://%s:%s/glite_wms_wmproxy_server'%(self.hostName,
                                                            self.portNumber)
        self.printdvm('>>> WMS endpoint to test')
        self.printdvm(wmsendpt)

        # build JDL
        class ErrNoGoodCEs(Exception):
            ''
        class ErrNoGoodCEsHTTP(Exception):
            ''
            def __init__(self, summary, detdata):
                self.summary = summary
                self.detdata = detdata
        def get_goodCEs():
            'Get all good CE from a given list.'
            ces = []
            try:
                ces = urllib.urlopen(self._fileGoodCEs).readlines()
            except IOError, e:
                raise ErrNoGoodCEs('Unable to load good CEs %s. %s' % \
                                            (self._fileGoodCEs, str(e)))
            except StandardError:
                raise
            else:
                if len(ces) == 0:
                    raise ErrNoGoodCEs('No good CEs were found in %s.' % \
                                                        self._fileGoodCEs)
                else:
                    if self._fileGoodCEs.startswith('http') and \
                        re.search('<.*>', ' '.join(ces), re.M):
                            raise ErrNoGoodCEsHTTP('Failed to get good CEs file %s' % \
                                                   self._fileGoodCEs,
                                                   'Response:\n%s' % ' '.join(ces))
                return [x.strip('\n') for x in ces]
        def get_goodCERand():
            'Randomly pick up a good CE from a given list.'
            ces = get_goodCEs()
            return ces[random.randint(0, len(ces)-1)]
        def get_goodCERandWithReq():
            'Return random good CE with JDL requirement.'
            return 'other.GlueCEInfoHostName == "%s"' % get_goodCERand()
        def get_goodCEsWithReq():
            'Return a list of good CEs with JDL requirements.'
            ces = get_goodCEs()
            req = ''
            for i in range(len(ces)-1, -1, -1):
                req += '(other.GlueCEInfoHostName == "%s") %s ' % (
                                        ces[i], i and '||' or '')
            return req
        try:
            self._jdlPatterns['jdlReqCEInfoHostName'] = get_goodCEsWithReq()
        except ErrNoGoodCEs, e:
            status = 'UNKNOWN'
            # no goodCe file, so use a generic requirement
            self._jdlPatterns['jdlReqCEInfoHostName'] = "true"
            #return (status, str(e), str(e))
        except ErrNoGoodCEsHTTP, e:
            status = 'UNKNOWN'
            return (status, e.summary, e.detdata)
        self._build_jdl()

        # Check if jobs ID file is there. If, yes:
        # - cancel all the jobs defined there
        # - delete the file
        if os.path.exists(self._fileJobID):
            cmd = self.gridjobcmd['CANCEL']+' -i %s' % self._fileJobID
            # TODO: use this returned info later for more verbose output
            _rc, _stsmsg, _detmsg = self.run_cmd(cmd)
            os.unlink(self._fileJobID)

        # submit job
        verb = ''
        if self.verbosity >= probe.VERBOSITY_MAX:
            verb = '--debug'
        cmd = self.gridjobcmd['SUBMIT']+' %(verb)s -e %(wms)s -o %(jobids)s %(jdl)s' % \
            {'verb'   : verb,
             'wms'    : wmsendpt,
             'jobids' : self._fileJobID,
             'jdl'    : self._fileJDL}
        self.printdvm('>>> Job submit command\n%s\n' % cmd)

        if self._nosubmit:
            self.printdvm('>>> Asked not to submit job. Bailing out ... ', cr=False)
            #self._js_do_cleanup()
            self.printd('Asked not to submit job. Bailing out')
            self.prints('Asked not to submit job. Bailing out')
            return 'OK'

        self.printdvm('>>> Submitting grid job ... ', cr=False)
        rc, stsmsg, detmsg = self.run_cmd(cmd, _verbosity=2)

        if rc != self.retCodes['OK']:
            self.printdvm('failed.')
            self._js_do_cleanup()
            self.printd(detmsg)
            self.prints(stsmsg)
            self._submit_results(self.hostName, self.metJobSubmission,
                    rc,
                    '%s: %s' %(samutils.to_status(rc), self.get_summary()),
                    self.get_detdata())
            return rc
        self.printdvm('done.')

        self.printd(detmsg)

        # obtain job's ID
        try:
            jobid = re.search('^http.*$', detmsg, re.M).group(0).rstrip()
            self.printdvm('>>> Job ID\n%s' % jobid)
        except (IndexError, AttributeError):
            status = 'UNKNOWN'
            s = "Job was successfully submitted, "+\
                "but we couldn't determine the job's ID."
            self.printd(s)
            self.prints(s)
            return status

        # save active job's description for jobs' monitor
        stime = str(int(time.time()))
        jd = {}
        jd['submitTimeStamp'] = stime
        jd['hostNameCE']      = self.hostName
        jd['serviceDesc']     = self.metJobSubmission
        jd['jobID']           = jobid
        jd['jobStatus']       = 'Submitted'
        jd['lastStatusTimeStamp'] = stime
        self._save_activejob(jd)

        self.prints(stsmsg)

        return rc

    def _submit_results(self, host, mname, rc, stsmsg, detmsg):
        """Submit check results.
        """
        metric_res = [{'host'    : host,
                       'service' : mname,
                       'status'  : str(rc),
                       'summary' : stsmsg,
                       'details' : detmsg}]
        self._submit_service_checks(metric_res)

    def _thr_ajworker(self, jd):
        """Check status of submitted grid job and udate the grid
        *job status* (CE-JobSubmit) and/or *CE state* (CE-JobState).
        (The "update" is perfomed in the sense of Nagios passive
        checks.)

        ###- h  - hostname (add if needed)
        - jd - job description

Depeneding on the status of the grid job:
- SUBMITTED, READY, SCHEDULED, RUNNING
  - update active job file
  - update job status: "OK: [Submitted/Ready/Scheduled/Running] jobID"
- WAITING
  - if in WAITING due to 'no compatible resources' >N min
    - cancel the job
    - if cancelled
      - update job status: "WARNING: [Canceled] after >N min in Waiting
                                             'no compatible resources'"
      - update CE state: "WARNING: job was cancelled after >N min in
                                   Waiting 'no compatible resources'"
    - else
      - update job status: "WARNING: [Waiting] unable to cancel the job
                      after >N min in Waiting 'no compatible resources'"
      - update CE state: "WARNING: unable to cancel the job after >N min
                                   in Waiting 'no compatible resources'"
    - delete active job file
  - else
    - if in WAITING >N sec
      - cancel the job
        - if cancelled
          - update job status: "OK: [Cancelled]"
        - else
          - update job status: "WARNING: [Waiting] unable to cancel the
                                            job after >N min in Waiting"
      - delete active job file
    - else
      - update job status: "OK: [Waiting]"
      - update active job file
- DONE
  - if DONE (Success|Exit Code.*)
    - update job status: "OK: ..."
    - update CE state: "OK: ..."
    - delete active job file
  - else if DONE (???)
    - ?
- ABORTED
    - update job status: "WARNING: [Aborted] Reason:  ..."
    - update CE state: "WARNING: [Aborted] Reason: ..."
      - TODO: check reason of job failure
    - delete active job file
- CANCELLED, CLEARED
  - this shouldn't happen
  - remove active job file

NB! Failed operations on WMS do NOT update CE state. Exceptional case is
    when job was in DONE (Success|Exit Code.*) state. CE state is set to OK
    in such case.

Timeouts:
TODO:
 - total timeout for grid job:
   * job has to be cancelled after that period and depending on the job status
     it was in when it was cancelled CE state should (not) be updated. Eg., if
     cancelled from Running - UNKNOWN should be set + adding
     glite-wms-job-logging-info -v 3 to details output.
"""

        stsmsg = ''
        detmsg = ''

        def _unlink(fn):
            try:
                os.unlink(fn)
            except StandardError:
                pass

        def update_non_terminal(state, jd, ajf, _detmsg=''):
            """Update active job file and job status passive check.
            """
            jd['jobStatus'] = state
            jd['lastStatusTimeStamp'] = str(int(time.time()))
            s = self._save_activejob(jd, ajf)
            self.printdvm('>>> %s\nUpdated active job file\n%s\n' % (host, s))

            # job status
            status = 'OK'
            stsmsg = status+': ['+state+'] '+jd['jobID']
            detmsg = status+': ['+state+'] '+jd['jobID']
            detmsg += _detmsg or ''
            self._submit_results(host, self.metJobState, str(self.retCodes[status]),
                           stsmsg, detmsg)

        def job_cancelTimeoutGlobal():
            """Cancel job because of job's global timeout.
            """
            #status = 'WARNING'
            status = 'OK'
            # cancel the job
            _detmsg = detmsg+\
                        "\n\n%i min global timeout for the job exceeded. Cancelling the job." % \
                        (allowMaxTimeout/60)
            cmd = self.gridjobcmd['CANCEL']+' %s' % jobid
            rc, _, detmsgC = self.thr_run_cmd(cmd, _verbosity=2)
            if rc != self.retCodes['OK']:
                jobsts = '[%s->Cancel failed [timeout/dropped]]' % jobstate
                _stsmsg = jobsts+" "+stsmsg+" Problem cancelling job."
                _detmsg = jobsts+" "+_detmsg+" Problem cancelling job.\n"+detmsgC
            else:
                jobsts = '[%s->Cancelled [timeout/dropped]]' % jobstate
                _stsmsg = jobsts+" "+stsmsg
                _detmsg = jobsts+" "+_detmsg+' '+" "+detmsgC

            # WMS State [A/P]
            self._submit_results(host, self.metJobState, str(self.retCodes[status]),
                           status+': '+_stsmsg, status+': '+_detmsg)
            # WMS Submit [P]
            self._submit_results(host, srvdesc, str(self.retCodes[status]),
                           _stsmsg, _detmsg)
            _unlink(ajf)
            gridjobs_states[host] = jobsts
            sched.de_queue(host)
            return (status, _stsmsg, _detmsg)

        def job_cancelTimeout(timeout, state, status='WARNING'):
            """Cancel job because of job's global timeout.
            """
            # cancel the job
            _detmsg = detmsg+\
                        "\n\n%i min timeout in '%s' exceeded. Cancelling the job." % \
                        (timeout/60, state)
            rc, msg = __job_logging_info(jobid)
            if samutils.to_status(rc) == 'CRITICAL':
                status = 'CRITICAL'
            _detmsg += msg
            rc, detmsgC = __job_cancel(jobid)
            if rc != self.retCodes['OK']:
                jobsts = '[%s->Cancel failed [timeout/dropped]]' % jobstate
                _stsmsg = jobsts+" "+stsmsg+" Problem cancelling job."
                _detmsg = jobsts+" "+_detmsg+" Problem cancelling job.\n"+detmsgC
            else:
                jobsts = '[%s->Cancelled [timeout/dropped]]' % jobstate
                _stsmsg = jobsts+" "+stsmsg
                _detmsg = jobsts+" "+_detmsg+' '+" "+detmsgC

            # WMS State [A/P]
            self._submit_results(host, self.metJobState, str(self.retCodes[status]),
                           status+': '+_stsmsg, status+': '+_detmsg)
            # WMS Submit [P]
            self._submit_results(host, srvdesc, str(self.retCodes[status]),
                           _stsmsg, _detmsg)
            _unlink(ajf)
            gridjobs_states[host] = jobsts
            sched.de_queue(host)
            return (status, _stsmsg, _detmsg)

        def __removeStatusStanza(str):
            return re.sub('(%s).*\n' % '|'.join(samutils.retCodes.keys()),
                          '\n', str, 1)

        def __failedGetJobStatus(detmsg):
            gridjobs_states[host] = 'Failure [get job status] %s: %s' % (host,detmsg)
            if re.search('command not found', detmsg, re.M):
                    # JobState
                    status = 'WARNING'
                    stsmsg = 'unable to get job status. Command not found.'
                    detmsg = '%s\n%s' % (stsmsg, detmsg)
                    self._submit_results(host, self.metJobState, str(self.retCodes[status]),
                                   status+': '+stsmsg, status+': '+detmsg)
            else:
                _, _detmsg = __job_logging_info(jobid)
                detmsg += '\n\nLogging info:\n%s' % _detmsg
                if int(time.time()) - subtime >= self._timeouts['JOB_DISCARD']:
                    # JobState
                    status = 'WARNING'
                    stsmsg = 'unable to get job status. Job discarded.'
                    detmsg = '%s\n%s' % (stsmsg, detmsg)
                    self._submit_results(host, self.metJobState, str(self.retCodes[status]),
                                   status+': '+stsmsg, status+': '+detmsg)
                    # JobSubmit
                    status = 'UNKNOWN'
                    self._submit_results(host, self.metJobSubmission, str(self.retCodes[status]),
                                   status+': '+stsmsg, status+': '+detmsg)
                    # remove the job
                    shutil.move(ajf, ajf+'.FAILURE_GETSTATUS')
                else:
                    # JobState
                    status = 'WARNING'
                    stsmsg = 'unable to get job status. Job will be deleted in %i min' % \
                                ((self._timeouts['JOB_DISCARD'] - (int(time.time()) - subtime)) / 60)
                    detmsg = '%s\n%s' % (stsmsg, detmsg)
                    self._submit_results(host, self.metJobState, str(self.retCodes[status]),
                                   status+': '+stsmsg, status+': '+detmsg)
            sched.de_queue(host)
            return (rc, stsmsg, stsmsg+detmsg)

        def __undefinedJobStatus():
            status = 'WARNING'
            stsmsg = status+': unable to determine job status.'
            # JobState
            self._submit_results(host, self.metJobState, str(rc),
                                 stsmsg,
                                 '%s\nJob id: %s' % (stsmsg,jobid))
            status = 'UNKNOWN'
            stsmsg = status+': unable to determine job status.'
            # JobSubmit
            self._submit_results(host, self.metJobSubmission, str(rc),
                                 stsmsg,
                                 '%s\nJob id: %s' % (stsmsg,jobid))

            shutil.move(ajf, ajf+'.FAILURE_UNDEFSTATUS')
            gridjobs_states[host] = 'Failure [undentified job status]'
            sched.de_queue(host)
            return (rc, stsmsg, detmsg)

        def __run_cmd(action, jobid):
            cmd = '%s %s' % (self.gridjobcmd[action],jobid)
            rc, _, msg = self.thr_run_cmd(cmd, _verbosity=2)
            return rc, __removeStatusStanza(msg)

        def __job_logging_info(jobid):
            return __run_cmd('LOGGING', jobid)

        def __job_cancel(jobid):
            return __run_cmd('CANCEL', jobid)

        def __job_status(jobid):
            return __run_cmd('STATUS', jobid)


        subtime = int(jd['submitTimeStamp'])
        host    = jd['hostNameCE']
        srvdesc = jd['serviceDesc']
        jobid   = jd['jobID']
        jobsts  = jd['jobStatus']

        workdir = '%s/%s' % (self.workdir_service, host)
        ajf = workdir+'/'+self._fileActiveJobName

        self.printdvm('>>> %s: Active job attributes' % host)
        self.printdvm('\n'.join([ k+' : '+v for k,v in jd.items()]))

        global gridjobs_states

        rc, _detmsg = __job_status(jobid)
        detmsg += _detmsg
        if rc != self.retCodes['OK']:
            return __failedGetJobStatus(detmsg)

        # parse details output and determine the job's status
        match = re.search('^Current Status:.*$', detmsg, re.M)

        if not match:
            return __undefinedJobStatus()

        currstat = match.group(0).rstrip()

        self.printdvm('>>> %s: %s %s' % (host,
                            time.strftime("%Y-%m-%dT%H:%M:%SZ"),currstat))

        # TODO: check times "lastStatusTimeStamp - submitTimeStamp" for jobs not in terminal states

        #non_terminal_re = '('+'|'.join([ s.title() for s in jobstates['non_terminal'] ])+')'
        allowMaxTimeout = self._timeouts['JOB_GLOBAL']
        currtime = int(time.time())

        if re.search('Done.*\((Success|Exit Code.*)\)', currstat):
            '''OK for all Reasons.
            '''

            djo = workdir+'/'+self._dirJobOutputName
            fjo = djo+'/'+self._fileJobOutputName

            self.printdvm('>>> %s:\n' % host+\
                          'Job is in Done state.\n'+\
                          'Get results and submit passive check result to '+\
                          '<%s,%s>\n' % (host, srvdesc))

            cmd = self.gridjobcmd['OUTPUT']+' --dir %s %s 2>&1' % (djo, jobid)
            rc, stsmsg, detmsg = self.thr_run_cmd(cmd, _verbosity=2)
            detmsg = __removeStatusStanza(detmsg)

            if self.verbosity >= 3:
                detmsg += '\nJob output:\n'
                try:
                    for ln in open(fjo,'r').readlines():
                        detmsg += ln
                except StandardError:
                    detmsg += '  Could not open job output file: '+fjo+'\n'

            # WMS Submit [P]
            self._submit_results(host, srvdesc, str(rc), stsmsg, detmsg)
            # WMS State [A/P]
            self._submit_results(host, self.metJobState, str(rc),
                           stsmsg, detmsg)
            _unlink(ajf)
            gridjobs_states[host] = 'Done'
            sched.de_queue(host)
            return (rc, stsmsg, detmsg)
        elif re.search('Aborted', currstat):
            def __job_aborted_reason(msg):
                if re.search('Reason.*request expired', msg, re.M):
                    if re.search('Reason.*BrokerHelper: no compatible resources',
                                 msg, re.M):
                        status = 'UNKNOWN'
                        stsmsg = status+': Job was aborted. Failed to match.'
                        _detmsg = stsmsg+detmsg
                    else:
                        status = 'WARNING'
                        stsmsg = status+': Job was aborted. Check WMS.'
                        _detmsg = stsmsg+detmsg
                else:
                    status = 'OK'
                    stsmsg = status+': Job was aborted.'
                    _detmsg = stsmsg+detmsg
                return status, stsmsg, _detmsg

            rc, msg = __job_logging_info(jobid)
            if samutils.to_status(rc) != 'OK':
                status = 'CRITICAL'
                stsmsg = status+': Job was aborted. Failed to get job logging info.'
                detmsg = stsmsg+detmsg
            else:
                status, stsmsg, detmsg = __job_aborted_reason(msg)
            detmsg += msg

            # WMS Submit [P]
            self._submit_results(host, srvdesc, str(self.retCodes[status]),
                           stsmsg, detmsg)
            # WMS State [A/P]
            self._submit_results(host, self.metJobState, str(self.retCodes[status]),
                           stsmsg, detmsg)
            _unlink(ajf)
            gridjobs_states[host] = 'Aborted'
            sched.de_queue(host)
            return (rc, stsmsg, detmsg)
        elif re.search('Waiting', currstat):
            jobstate = 'Waiting'
            if currtime - subtime >= self._timeouts['JOB_WAITING']:
                return job_cancelTimeout(self._timeouts['JOB_WAITING'],
                                           jobstate, status='WARNING')
            update_non_terminal(jobstate, jd, ajf, _detmsg=detmsg)
            jobsts = '[%s]'%jobstate

        elif re.search('Submitted', currstat):
            jobstate = 'Submitted'
            if currtime - subtime >= self._timeouts['JOB_SUBMTTED']:
                return job_cancelTimeout(self._timeouts['JOB_SUBMTTED'],
                                           jobstate, status='WARNING')
            update_non_terminal(jobstate, jd, ajf, _detmsg=detmsg)
            jobsts = '[%s]'%jobstate

        elif re.search('Ready', currstat):
            jobstate = 'Ready'
            if currtime - subtime >= self._timeouts['JOB_READY']:
                return job_cancelTimeout(self._timeouts['JOB_READY'],
                                           jobstate, status='WARNING')
            update_non_terminal(jobstate, jd, ajf, _detmsg=detmsg)
            jobsts = '[%s]'%jobstate

        elif re.search('Scheduled', currstat):
            jobstate = 'Scheduled'
            if currtime - subtime >= allowMaxTimeout:
                return job_cancelTimeoutGlobal()
            update_non_terminal(jobstate, jd, ajf, _detmsg=detmsg)
            jobsts = '[%s]'%jobstate

        elif re.search('Running', currstat):
            jobstate = 'Running'
            if currtime - subtime >= allowMaxTimeout:
                return job_cancelTimeoutGlobal()
            update_non_terminal(jobstate, jd, ajf, _detmsg=detmsg)
            jobsts = '[%s]'%jobstate

        elif re.search('Cleared', currstat):
            # Job was cleared, but active job file was left.
            # There is not need to update job's status.
            self.printdvm('>>> %s: Warning. Job was cleared, ' % host +\
                          'but active job file was left\n%s' % ajf)
            _unlink(ajf)
            jobsts = 'Cleared'

        elif re.search('Cancelled', currstat):
            # Job was cancelled, but active job file was left. Seems like it wasn't
            # us who did delete the file.

            #===============================================================================
            # *************************************************************
            # BOOKKEEPING INFORMATION:
            #
            # Status info for the Job : https://wms206.cern.ch:9000/OoiKF7G4RqojM0xQLiiGUg
            # Current Status:     Cancelled
            # Logged Reason(s):
            #    -
            #    - Aborted by user
            # Status Reason:      Aborted by user.
            # Destination:        agh2.atlas.unimelb.edu.au:2119/jobmanager-lcgpbs-ops
            # Submitted:          Tue Jan 27 10:33:23 2009 CET
            # *************************************************************
            #===============================================================================

            # job status - WARNING
            # CE state - UNKNOWN

            self.printdvm('>>> %s: Warning. Job was cancelled, ' % host +\
                          'but active job file was left\n%s' % ajf)
            os.unlink(ajf)
            jobsts = 'Cancelled'

        else:
            jobstate = 'UNDETERMINED'
            self.printd('>>> %s: Warning. Job is in %s: %s' % (host, jobstate,
                                                               jobid))
            if currtime - subtime >= allowMaxTimeout:
                return job_cancelTimeout(allowMaxTimeout,
                                        jobstate, status='UNKNOWN')
            jobsts = jobsts = '[%s]'%jobstate

        gridjobs_states[host] = jobsts
        sched.de_queue(host)
        return(rc, stsmsg, detmsg)

    def metricJobMonit(self):
        """Babysitter for grid jobs.
        """
        perf_data = PerfData(self.jm_perf_data_order)

        def __get_badjds():
            if not badjds:
                return ''
            return '\n%s\nBad job descriptions:\n%s' % \
                    ('-'*21, '---\n'.join(badjds))

        def __status():
            if badjds:
                return 'WARNING'
            else:
                return 'OK'

        def __update_perfdata_from_jobstates(perfdata, js):
            perfdata['unknown'] = [0,1,2]
            for k,v in js.items():
                k = re.sub(' .*$', '', k)
                k = re.sub('[][><?!@]', '', k).upper()
                if k in self.jm_perf_data_order:
                    if perfdata.has_key(k):
                        perfdata[k] += v
                    else:
                        perfdata[k] = v
                else:
                    perfdata['unknown'][0] += v
            return perfdata

        # get active job files
        hostsdir = self.workdir_service
        if self._hosts:
            hosts = self._hosts
        else:
            hosts = os.listdir(hostsdir)
        hostsajd = {}
        badjds = []
        for h in hosts:
            ajf = '%s/%s/%s' % (hostsdir, h, self._fileActiveJobName)
            try:
                os.stat(ajf)
            except OSError:
                pass
            else:
                rc,jd = self._load_activejob(ajf)
                if rc:
                    hostsajd[h] = jd
                else:
                    badjds.append('%s - %s' % (h,jd))

        # no active jobs
        if not hostsajd:
            status = __status()
            summary = detmsg = status+': no active jobs ['+\
                time.strftime("%Y-%m-%dT%H:%M:%SZ")+']'
            self.perf_data = [('jobs_processed', 0)]
            return (status, summary, detmsg + __get_badjds())

        self.printdvm('>>> Active jobs to process')
        self.printdvm('\n'.join([x['jobID'] for x in hostsajd.values()]))

        # spawn threads to process active jobs
        thrsched = sched.SchedulerThread()
        for h,jd in hostsajd.items():
            thrsched.run(self._thr_ajworker, jd, h)

        thrsched.wait()

        status = __status()
        summary = detmsg = status+': Jobs processed - '+str(len(hostsajd))

        pd = {'jobs_processed' : len(hostsajd)}

        global gridjobs_states
        js = {}
        for h in hostsajd.keys():
            try:
                try:
                    s = gridjobs_states[h]
                    if re.match('Failure', s):
                        status = 'UNKNOWN'
                except KeyError:
                    s = 'Missed [%s]' % h
                js[s] += 1
            except KeyError:
                js[s] = 1
        jss = '\n'.join([ k+' : '+str(v) for k,v in js.items() ])
        detmsg += '\n'+jss
        # performance data
        perf_data.update(__update_perfdata_from_jobstates(pd, js))
        self.perf_data = perf_data
        return (status, summary, detmsg + __get_badjds())
