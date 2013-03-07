#!/usr/bin/env python

from distutils.core import setup
import ConfigParser

pkg_version = '0.0.0'
try:
    parser = ConfigParser.ConfigParser()
    parser.read('setup.cfg')
    pkg_version = parser.get('global','pkgversion')
except:
    pass

setup(
      name='glite-wms-wmproxy-api-python',
      version=pkg_version,
      description='Python libraries for the WM Proxy service',
      license='Apache Software License',
      py_modules=['WMPClient', 'WMPConnection', 'wmproxymethods'],
      package_dir = {'': 'src'},
      data_files=[
                  ('usr/share/doc/glite-wms-wmproxy-api-python-' + pkg_version, ['LICENSE'])
                 ]
     )
