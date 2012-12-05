#!/usr/bin/env python

from distutils.core import setup
from subprocess import call

mycomp_version = '3.5.0'
myname="emi-wms-nagios"
call(['mkdir', '-p', myname + '-' + mycomp_version + '/usr/libexec/grid-monitoring/probes/emi.wms'])
call(['mkdir', '-p', myname + '-' + mycomp_version + '/wmsmetrics'])
call(['cp', '-rpf', 'src/WMS-probe', 'src/WMS-jdl.template', myname + '-' + mycomp_version + '/usr/libexec/grid-monitoring/probes/emi.wms'])
call(['cp', '-rpf', 'src/wmsmetrics/__init__.py', 'src/wmsmetrics/WmsMetrics.py', 'src/wmsmetrics/scheduler.py', myname + '-' + mycomp_version + '/wmsmetrics'])
call(['cp', '-f', 'CHANGES', 'setup.py', 'README', myname + '-' + mycomp_version])
setup(
      name=myname,
      version=mycomp_version,
      description='Nagios probe for the EMI WMS service',
      license='Apache Software License',
      packages=['wmsmetrics'],
      package_dir = {'': 'src'},
      data_files=[
                  ('libexec/grid-monitoring/probes/emi.wms', ['src/WMS-jdl.template', 'src/WMS-probe']),
                  ('share/doc/emi-wms-nagios-' + mycomp_version, ['CHANGES', 'README'])
                 ]
     )
