Summary: Configuration module for the Workload Management System
Name: glite-wms-configuration
Version: %{extversion}
Release: %{extage}.%{extdist}
License: Apache Software License
Vendor: EMI
URL: http://glite.cern.ch/
Group: System Environment/Libraries
BuildArch: noarch
BuildRequires: condor
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
AutoReqProv: yes
Source: %{name}-%{version}-%{release}.tar.gz

%description
Configuration module for the Workload Management System

%prep
 

%setup -c -q


%install
rm -rf %{buildroot}
mkdir -p %{buildroot}
%{!?extbuilddir:%define extbuilddir "--"}
if test "x%{extbuilddir}" == "x--" ; then
  ./install.sh %{buildroot}
else
  cp -R %{extbuilddir}/* %{buildroot}
fi


%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root)
%dir /etc/glite-wms/
%config(noreplace) /etc/glite-wms/wms.conf.template
%dir /usr/share/doc/glite-wms-configuration-%{version}/
%doc /usr/share/doc/glite-wms-configuration-%{version}/LICENSE
/usr/libexec/glite-wms-parse-configuration.sh
/usr/libexec/glite-wms-check-daemons.sh
/usr/libexec/glite-wms-services-certs.sh

%changelog
* %{extcdate} WMS group <wms-support@lists.infn.it> - %{extversion}-%{extage}.%{extdist}
- %{extclog}


