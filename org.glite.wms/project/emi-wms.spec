Summary: Meta-package to install the WMS service
Version: %{extversion}
Release: %{extage}.%{extdist}
Name: emi-wms
License: Apache License 2.0
Vendor: EMI
Buildarch: noarch
Group: System Environment/Libraries
Requires: lcmaps-plugins-voms
Requires: emi-version
Requires: emi-lb
Requires: jemalloc
Requires: bdii
Requires: lcg-expiregridmapdir
Requires: globus-gridftp-server-progs
Requires: globus-proxy-utils
Requires: glite-initscript-globus-gridftp
Requires: kill-stale-ftp
Requires: lcas-plugins-voms
Requires: fetch-crl
Requires: mysql-server
Requires: lcas-plugins-basic
Requires: glite-info-provider-service
Requires: lcas-lcmaps-gt4-interface
Requires: glue-schema
Requires: condor-emi
Requires: glite-wms-common
Requires: glite-wms-configuration
Requires: glite-wms-interface
Requires: glite-wms-ice
Requires: glite-wms-jobsubmission
Requires: glite-wms-core

BuildRoot: %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)
AutoReqProv: yes
Source: emi-wms-%{version}-%{release}.tar.gz

%description
Meta-package to install the WMS service

%prep
%setup -c

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}

%clean

%files
%defattr(-,root,root)

%changelog
