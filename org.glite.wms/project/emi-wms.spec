Summary: This is a dummy package used to install the WMS node
Name: emi-wms
Version: 1.0.2
Release: 0.sl5
License: Apache License 2.0
Vendor: EMI
Group: System Environment/Libraries
Packager: ETICS
BuildArch: x86_64
Requires: lcmaps-plugins-voms
Requires: emi-version
Requires: glite-wms-ice
Requires: emi-lb
Requires: jemalloc
Requires: bdii
Requires: lcg-expiregridmapdir
Requires: globus-gridftp-server-progs
Requires: lcas-plugins-voms
Requires: fetch-crl
Requires: mysql-server
Requires: lcas-plugins-basic
Requires: glite-yaim-wms
Requires: condor-lcg
Requires: glite-wms-manager
Requires: globus-proxy-utils
Requires: glite-wms-wmproxy
Requires: glite-info-provider-service
Requires: lcas-lcmaps-gt4-interface
Requires: glue-schema
Requires: glite-wms-jobsubmission
Requires: glite-initscript-globus-gridftp
BuildRoot: %(mktemp -ud %{_tmppath}/%{name}-%{version}-%{release}-XXXXXX)
AutoReqProv: yes
Source: emi-wms-1.0.2-0.tar.gz

%description
This is a dummy package used to install the WMS node

%prep
 

%setup -c

%install
 
 rm -rf %{buildroot}
 mkdir -p %{buildroot}

%clean
 

%files
%defattr(-,root,root)
 

%changelog
 
