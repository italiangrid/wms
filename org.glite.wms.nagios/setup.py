#!/usr/bin/env python

from distutils.core import setup

comp_version = '1.0.0'

setup(
      name="emi-wms-nagios",
      version=comp_version,
      description='Nagios probe for the EMI WMS service',
      license='Apache Software License',
      packages=['wmsmetrics'],
      package_dir = {'': 'src'},
      data_files=[
                  ('libexec/grid-monitoring/probes/emi.wms', ['src/WMS-jdl.template', 'src/WMS-probe']),
                  ('share/doc/emi-wms-nagios-' + comp_version, ['CHANGES', 'README'])
                 ]
     )
