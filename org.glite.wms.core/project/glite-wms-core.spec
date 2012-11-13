Summary: Core logic for the Workload Management System
Name: glite-wms-core
Version: %{extversion}
Release: %{extage}.%{extdist}
License: Apache Software License
Vendor: EMI
URL: http://glite.cern.ch/
Group: Applications/Internet
BuildArch: %{_arch}
Requires: glite-wms-configuration glite-wms-helper glite-wms-ism glite-wms-purger glite-wms-common glite-lb-client
Requires(post): chkconfig
Requires(preun): chkconfig
Requires(preun): initscripts
BuildRequires: chrpath, libtool, boost-devel, c-ares-devel, classads-devel, globus-ftp-client-devel, globus-ftp-control-devel, docbook-style-xsl
BuildRequires: glite-build-common-cpp, glite-jobid-api-c-devel
BuildRequires: %{!?extbuilddir: glite-wms-common-devel, glite-wms-purger-devel, } glite-px-proxyrenewal-devel
BuildRequires: glite-lb-client-devel, glite-jobid-api-cpp-devel, libxslt
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
AutoReqProv: yes
Source: %{name}-%{version}-%{release}.tar.gz
Obsoletes: glite-wms-ism, glite-wms-helper, glite-wms-manager

%global debug_package %{nil}

%description
Core logic for the Workload Management System

%prep
 
%setup -c -q

%build
%{!?extbuilddir:%define extbuilddir "--"}
if test "x%{extbuilddir}" == "x--" ; then
  ./configure --prefix=%{buildroot}/usr --sysconfdir=%{buildroot}/etc --disable-static PVER=%{version}
  make
fi

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}
%{!?extbuilddir:%define extbuilddir "-"}
if test "x%{extbuilddir}" == "x-" ; then
  make install
else
  cp -R %{extbuilddir}/* %{buildroot}
fi
rm -f %{buildroot}%{_libdir}/libglite_wms_ism*.la
chrpath --delete %{buildroot}%{_libdir}/libglite_wms_*.so.0.0.0
chrpath --delete %{buildroot}/usr/bin/glite-wms-workload_manager
strip -s %{buildroot}/usr/bin/glite-wms-workload_manager
chrpath --delete %{buildroot}/usr/bin/glite-wms-query-job-state-transitions
strip -s %{buildroot}/usr/bin/glite-wms-query-job-state-transitions
export QA_SKIP_BUILD_ROOT=yes

%clean
rm -rf %{buildroot}

%post
/sbin/chkconfig --add glite-wms-wm 

%preun
if [ $1 -eq 0 ] ; then
    /sbin/service glite-wms-wm stop >/dev/null 2>&1
    /sbin/chkconfig --del glite-wms-wm
fi

%files
%defattr(-,root,root)
%{_libdir}/libglite_wms_ism*.so.0.0.0
%{_libdir}/libglite_wms_ism*.so.0
%{_libdir}/libglite_wms_helper*.so.0.0.0
%{_libdir}/libglite_wms_helper*.so.0
/etc/rc.d/init.d/glite-wms-wm
/usr/bin/glite-wms-workload_manager
/usr/bin/glite-wms-stats
/usr/bin/glite-wms-query-job-state-transitions
%dir /usr/share/doc/%{name}-%{version}/
%doc /usr/share/doc/%{name}-%{version}/LICENSE
%doc /usr/share/man/man1/glite-wms-*.1.gz

%changelog
* %{extcdate} WMS group <wms-support@lists.infn.it> - %{extversion}-%{extage}.%{extdist}
- %{extclog}
