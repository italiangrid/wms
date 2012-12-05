Summary: Nagios probe for the EMI WMS service
Name: emi-wms-nagios
URL: http://web.infn.it/gLiteWMS/
Version: %{extversion}
Release: %{extage}.%{extdist}
License: ASL 2.0
Group: Applications/Communications
Source: %{name}-%{version}-%{release}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root
Requires: openssl >= 0.9.8e-12
Requires: python >= 2.4
Requires: python-GridMon >= 1.1.10
Requires: python-ldap
Requires: nagios-submit-conf >= 0.2
#Requires: python-suds >= 0.3.5
#Requires: grid-monitoring-probes-hr.srce >= 0.20.1
AutoReqProv: no
BuildArch: noarch
BuildRequires: python >= 2.4

%global debug_package %{nil}
%{!?python_sitelib: %global python_sitelib %(%{__python} -c "from distutils.sysconfig import get_python_lib; print get_python_lib()")}
%define dir %{_libexecdir}/grid-monitoring/probes/emi.wms

%description
This package contains nagios probe for EMI WMS service. 
It use a python-based gridmetrics.

%prep
%setup -c -q

%build

%install
export DONT_STRIP=1
%{__rm} -rf %{buildroot}
%{__python} setup.py install_lib -O1 --skip-build --build-dir=./src/wmsmetrics --install-dir=%{buildroot}%{python_sitelib}/wmsmetrics
mkdir -p %{buildroot}/usr/libexec/grid-monitoring/probes/emi.wms
install --directory %{buildroot}%{dir}
install --mode 755 ./src/WMS-probe  %{buildroot}%{dir}
install --mode 755 ./src/WMS-jdl.template  %{buildroot}%{dir}
install --directory %{buildroot}%{python_sitelib}/wmsmetrics
install --mode 644 ./src/wmsmetrics/*.py* %{buildroot}%{python_sitelib}/wmsmetrics

%post

%postun

%clean
%{__python} setup.py clean --all

%files
%defattr(-,root,root,-)
%{dir}/WMS-probe
%{dir}/WMS-jdl.template
%{python_sitelib}/wmsmetrics/*.py*
%doc README
%doc CHANGES

%changelog
* Wed Dec 5 2012 M. Cecchi
- Integrated with the WMS script to build from source and repackage
- Makefile removed
- Version number goes with WMS version
* Thu Sep 15 2011 A. Gianelle - 1.0.0
- Porting to EMI 
* Tue Jul 5 2011 Emir Imamagic <eimamagi@srce.hr> - 0.2.3-1
- SAM-1699: Modify CE-sft-softver to support EMI WN
* Fri Jul 1 2011 Emir Imamagic <eimamagi@srce.hr> - 0.2.2-1
- SAM-1656: CA release 1.40
* Tue Jun 28 2011 Emir Imamagic <eimamagi@srce.hr> - 0.2.1-1
- SAM-1656: CA release 1.39
* Tue May 3 2011 Emir Imamagic <eimamagi@srce.hr> - 0.1.20-2
- SAM-1421: Raise critical if WN job workdir can't be created
* Wed Feb 9 2011 Emir Imamagic <eimamagi@srce.hr> - 0.1.19-1
- SAM-1253: CA release 1.38
* Fri Nov 12 2010 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.18-1
- bugfix
* Wed Oct 27 2010 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.17-1
- bugfix
* Tue Aug 24 2010 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.15-1
- added LFC-probe
* Wed Jul 28 2010 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.14-1
- dependency: python-GridMon >= 1.1.10
* Tue Jun 22 2010 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.12-5
- dependency: python-GridMon >= 1.1.9
* Tue Jun 1 2010 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.12-4
- SAM-503: creation/deletion of probes working directory moved from here
  to python-GridMon RPM
* Tue May 18 2010 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.12-3
- changed license to ASL 2.0
* Mon May 10 2010 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.12-1
- added CHANGES file to the package documentation
* Wed Apr 21 2010 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.11-1
- added pylib/jobmonit package
- dependency: python-GridMon >= 1.1.7
* Thu Mar 18 2010 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.10-1
- changes to use new VOMS FQAN based Nagios metrics naming
- propagation of VOMS FQANs to WN tests
- --prev-status paramter to provide previous status of the metric
- treating Scheduled and Running job states in job submission
  * increased default timeout to 5h30min (--timeout-job-schedrun)
  * metric stays CRITICAL after two successive WARNINIG
- fixed bugs:
  * 64093 integrate Net::STOMP and latest MBs discovery utilities
  * 64489 possible bug in org.sam.CREAMCE-DirectJobMonit-dteam
* Thu Mar 11 2010 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.9-2
- fix in MPI metric: fail gracefully on WN if BDII can't be contacted
* Tue Mar 09 2010 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.9-1
- fixed bugs:
  * 63490 WN - Nagios named pipe should be in /tmp because of $HOME on afs
  * 63711 MPI tests - integration
  * 51274 UNKNOWN: unhandled exception while gathering metric results
  * 53658 Report lcg_utils/gfal version in org.sam.WN-Rep tests
  * 63576 Multiple WARNs for job submission timeouts -> CRITICAL
- dependency python-GridMon >= 1.1.5
- sanitazing tests output run by samtest-run or nagtest-run
- lowered error levels in gLExec metric (CRITICAL -> WARNING and UNKNOWN)
- refactoring
  * output of summary and details data
  * scheduler for JobMonit moved to a separate module scheduler.py
  * CREAMCE direct job submission metrics moved to gridmetrics.creammetrics
* Mon Feb 22 2010 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.8-2
- CA release 1.34-1
- WMS-JobState: support for getting list of good CEs from remote server
- force en_US.UTF-8 locale on WNs
* Wed Feb 17 2010 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.8-1
- integrated LFC certificate lifetime metric from SAM
* Tue Feb 16 2010 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.7-2
- split of runtime definition of verbosity levels for FW on WN and
  WN metrics (--wn-verb-fw, --wn-verb)
- setting LFC and replication SE for WN replica tests from configuration
  file and CLI in CE-probe (--wn-lfc, --wn-se-rep)
- gLExec WN metric (org.sam.glexec.WN-gLExec)
- CA release 1.33-1
- updating dependency on python-GridMon >= 1.1.4
* Mon Feb 08 2010 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.6-1
- propagation of WN job exit code to JobMonit metric
  - WN job with Nagios compliant exit codes depending on situation
  - JobMonit checks WN exit codes and accordingly sets the status for
    JobState/JobSubmit
- refactoring of JobStat and JobMonit metrics
- added --timeout-job-discard for JobState and JobMonit
- WN-Rep* metrics removed as children from WN-RepFree to comply to SAM
* Mon Feb 01 2010 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.5-4
- fixing problem with brokers discovery on WN (typo in "if" statement
  in wnjob/nagrun.sh)
* Wed Jan 27 2010 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.5-3
- --hosts parameter to org.sam.<Service>-JobMonit metric to select only
  certain hosts to be checked with the monitor.
- better treatment of corrupted job descriptions in JobMonit metric.
- removed JS metric - synchronous jobs submission.
- updated OCSP handler, messaging utilities and libraries for WN Nagios.
- added debug levels to framework launch script on WNs (set via
  --wnjob-verb job submission probes parameter).
- --ns changed to --namespace for all metrics.
- --config to specify metrics configuration file. Default
  /etc/gridmon/org.sam.conf
* Mon Dec 14 2009 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.5-2
- added CREAM CE metrics for testing direct job submission.
- added probe for WMS service.
- added metrics and passive checks output sanitizer.
- making Nagios WN checks config files as config(noreplace) in RPM.
- added new gLite error expressions to /etc/gridmon/org.sam.errdb.
- checked/updated LDAP queries in all metrics. Fixing:
  * https://savannah.cern.ch/bugs/?59599
- merged 'gridmonsam' package with 'gridmon' from python-GridMon
- introducing dependencies
  * python-suds (SOAP Python client)
  * python-GridMon ('gridmon' Python package)
* Thu Nov 19 2009 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.4-14
- due to migration of "central" SE used in SAM WN replica tests changing
  default from lxdpm103.cern.ch to samdpm002.cern.ch (org.sam/WN-probe).
* Thu Nov 12 2009 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.4-12
- fixing
  * https://savannah.cern.ch/bugs/?58640
  * https://savannah.cern.ch/bugs/?58599 (added ce_wms error topic with
    some glite WMS error messages into /etc/gridmon/org.sam.errdb)
* Thu Oct 29 2009 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.4-11
- CA 1.32-1: test reference DB and config with 8 + 7 days timeout.
* Wed Oct 14 2009 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.4-10
- fixing a bug in SRM-probe: after refactoring to_retcode() was moved to
  different module but in one place was still used from gridmonsam.probe
* Tue Oct 06 2009 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.4-9
- fixing bug with directories not being deleted from LFC when checking
  if LFC we were given is writable. Only CLI based method was affected.
* Wed Sep 09 2009 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.4-8
- due to migration of "central" SE used in SAM WN replica tests changing
  default from lxdpm104.cern.ch to lxdpm103.cern.ch (org.sam/WN-probe).
* Thu Aug 20 2009 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.4-7
- CA 1.31-1: test reference DB and config with 8 + 7 days timeout.
* Wed Jul 22 2009 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.4-6
- on 64bit WN if we are unable to load lcg_util Python lib with default
  Python, nagrun.sh will look for 32bit Python installed in VO software
  area. If found, and lcg_util lib can be loaded nagrun.sh will arrange
  for the 32bit Python to be used.
- added WN testing using lcg_util, lfc CLIs. If lcg_util, lfc Python
  libraries are not available or not loadable, then, all tests are
  performed using respective CLIs.
- added:
  * usage of timeouts in lcg_util depending on versions and either CLI or
    API is being used
  * import of lfc2 (in WN-probe) if available
  * some new glite error expressions to /etc/gridmon/org.sam.errdb
- more verbose (Nagios compliant) output when probe fails to import grid
  API or grid monitoring modules
- CE-sft-csh modified
- added ldap queries using ldapsearch CLI. If Python ldap API is not
  available code will automatically use CLI interface.
- new test for WN: check_pyver - checks installed version of Python
- WN tarball assembly (during CE job submission) was extended to allow
  integration of WN probes external to the org.sam framework.
- added possibility to specify JDL as a template (CLI input parameter
  --jdl-templ to org.sam.CE-JobState probe). As well as some ClassAdds.
- simplified parsing on command line parameters for sub-classes of
  MetricGatherer via per metric definitions of required command line
  parameters in the metrics description dictionary.
- proper README file and dynamic build of HTML documentation for Python
  modules.
* Mon May 18 2009 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.3-4
- fixing re-opened https://savannah.cern.ch/bugs/index.php?50177
- modifications to the way Nagios is launched on WNs (see bug #50341):
   * nagios is launched as background process not daemon
   * child_processes_fork_twice=0 in nagios config
   * "killall nagios" added to "nagrun.sh"
* Thu May 13 2009 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.3-3
- CA 1.29-1: test reference DB and config with 8 + 7 days timeout.
* Thu May 13 2009 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.3-2
- fixing:
   * python-GridMon should be taken to WNs
   * https://savannah.cern.ch/bugs/?48850
   * https://savannah.cern.ch/bugs/?50177
- adding:
   * critical SAM tests using samtest-run wrapper
     wnjob/nagios.d/probes/sam/CE-sft-{brokerinfo,caver,csh,softver}
   * python-ldap package (arch dependently) to temporarily and partially
     solve https://savannah.cern.ch/bugs/?49805
* Mon Mar 06 2009 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.2
- fixing:
   https://savannah.cern.ch/bugs/index.php?47667
   https://savannah.cern.ch/bugs/index.php?47639
- adding new error message into Errors DB [wms] section.
- added support for building OS-flavoured RPMs (.spec and Makefile)
* Mon Feb 25 2009 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.1
- "VO-enablement" of passive check names sent to Nagios.
* Mon Feb 24 2009 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.1.0
- publication of Nagios passive checks moved to python-GridMon >= 1.1
- added dependency on python-GridMon
- new modules: gridmonsam/{errmatch.py,utils.py}. The coded moved from
  gridmonsam/probe.py.
* Mon Jan 29 2009 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.0.9-3
- fixing recently introduced problem with propagation of return codes from
  metrics to the metrics' wrapper.
- added --timeout-job-global, --timeout-job-waiting parameters to CE-probe's
  org.sam.CE-JobMonit metric
* Mon Jan 28 2009 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.0.9-1
- fixed bug #46373
- beta release of a split of CE probe into a grid job submitter and submitted
  jobs babysitter. The job submitter submits a grid job via WMS to a particular
  CE. The babysitter is a multi-threaded code, which spawns a number of workers
  to get the grid jobs statuses and submits passive check results with the
  job's statuses and the final terminal state of each job.
- wnjob/nagrun.sh was made less verbose (configurable).
* Thu Jan 22 2009 James Casey <james.casey@cern.ch> - 0.0.8-2
- removed deps on GFAL-client and LFC we don't want them pulled automatically
  onto nagios node
* Mon Jan 19 2009 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.0.8-1
- fixed a bug at initialization of gfal - 'no_bdii_check' was missing.
- added 64bit Time::HiRes Perl module
- added statically compiled 'nagiostats'
- Python 'tarfile' to create zipped tarball for WN
- nagrun.sh accepts named command line parameters
- JDL was changed to reflect changes in nagrun.sh interface
* Fri Jan 16 2009 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.0.7-1
- .spec and Makefile. Made the built package arch independent. (On SL(C)4 the
  building should be done without redhat-rpm-config package installed to avoid
  building of debug info for Nagios binaries, which come with the package.)
* Fri Jan 16 2009 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.0.6-1
- send_to_msg to accepts full destination
- refined Nagios configuration of WN-probe/metrics (command and service objects)
* Tue Jan 13 2009 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.0.5-1
- CE-probe accepts MB topic/queue (mandatory parameter) and MB URI, and passes
  them to monitoring job on WN to be used with 'send_to_msg'.
- added wnjob/nagios.d/bin/find_broker for MB discovery on WNs
- statically compiled Nagios binary (for execution on WN)
* Fri Jan 09 2009 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.0.4-1
- added org.sam.WN-Rep{Get,Del}
- --mb-destination (topic/queue on MB) and --mb-uri (MB URI) were introduced in
  org.sam.CE-probe (not yet being propagated to WN)
- imporvements in command line paramters parsing and handling. All common ones
  to probe were moved to Metric.Gatherer.
* Thu Jan 08 2009 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.0.3-3
- changes to .spec in post and postun for safe upgrades.
- Probes and metrics optional arguments:
  - removed -o "..." functionality to feed options to metrics. Now they can
    be provided along with others. Hacked Python's 'getopt' module to loosen
    checks of long options.
  - removed -m as mandatory option. Now, w/o -m a default active "wrapper"
    check is executed.
* Tue Jan 06 2009 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.0.2-1
- .spec file
  - added dependency on GFAL-client, lcg_util, python-ldap
  - added post and postun to create/remove probes working directory
- gridmonsam/probe.py module
  - added publication of passive checks via NSCA
- added gridmonsam/{pexpect,pexpectpgrp}.py modules for getting line buffered
  output from forked processes
- added /etc/gridmon/org.sam.conf configuration file
- moved errors DB to /etc/gridmon/org.sam.errdb
- added check_sam - Nagios check wrapper to launch native SAM tests (produces
  Nagios compliant output and maps SAM exit codes to Naigios ones.)
- wnjob/nagrun.sh - changes according to the changes in the interface to
  sent_to_msg
 * Tue Dec 09 2008 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - 0.0.1-4
- split of CE-probe into WN-probe and CE-probe
- added org.sam/wnjob/ directory containing Nagios framework to run on WNs
- configuration moved to /etc/gridmon
* Fri Nov 21 2008 K. Skaburskas <Konstantin.Skaburskas@cern.ch> - initial version
- Initial build.
