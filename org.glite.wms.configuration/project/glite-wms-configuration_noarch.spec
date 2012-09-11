Summary: Configuration module for the Workload Management System
Name: glite-wms-configuration
Version: %{extversion}
Release: %{extage}.%{extdist}
License: Apache Software License
Vendor: EMI
URL: http://glite.cern.ch/
Group: System Environment/Libraries
BuildArch: noarch
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
AutoReqProv: yes
Source: %{name}-%{version}-%{release}.tar.gz
Requires: glite-yaim-core
Obsoletes: glite-yaim-wms
Prefix: /opt/glite

%description
Configuration module for the Workload Management System

%prep
 
%setup -c -q

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}
%{!?extbuilddir:%define extbuilddir "--"}
if test "x%{extbuilddir}" == "x--" ; then
  ./install.sh %{buildroot} %{version}
else
  cp -R %{extbuilddir}/* %{buildroot}
fi

%build
make install prefix=%{buildroot}%{prefix}

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
%{prefix}/yaim/functions/config_*
%config(noreplace) %{prefix}/yaim/node-info.d/glite-*
%{prefix}/share/man/man1/glite-WMS.1
%{prefix}/yaim/defaults/glite-*
%{prefix}/yaim/services/glite-wms
#%{prefix}/yaim/etc/versions/glite-yaim-wms

%post
                                                                                                                           
%postun
rm -f %{prefix}/share/man/man1/yaim-WMS.1

%changelog
* %{extcdate} WMS group <wms-support@lists.infn.it> - %{extversion}-%{extage}.%{extdist}
- %{extclog}
