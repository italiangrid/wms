Summary: Core logic for the Workload Management System
Name: glite-wms-core
Version: %{extversion}
Release: %{extage}.%{extdist}
License: ASL 2.0
Vendor: EMI
URL: http://web.infn.it/gLiteWMS/
Group: Applications/Internet
BuildArch: %{_arch}
#Requires: %{name}%{?_isa} = %{version}-%{release}
Requires: %{!?extbuilddir: glite-wms-common, glite-wms-configuration, glite-wms-helper, glite-wms-purger, } glite-lb-client
Requires: boost, classads, openldap, glite-build-common-cpp, glite-wms-utils-classad
BuildRequires: chrpath, libtool, boost-devel, c-ares-devel, classads-devel, globus-ftp-client-devel, globus-ftp-control-devel, docbook-style-xsl
BuildRequires: glite-build-common-cpp, glite-jobid-api-c-devel, openldap-devel, boost-devel, classads-devel, glite-wms-utils-classad-devel
BuildRequires: %{!?extbuilddir: glite-wms-common-devel, glite-wms-purger-devel, } glite-px-proxyrenewal-devel
BuildRequires: glite-lb-client-devel, glite-jobid-api-cpp-devel, libxslt
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
AutoReqProv: yes
Source: %{name}-%{version}-%{release}.tar.gz
Obsoletes: glite-wms-ism < 3.5.0
Obsoletes: glite-wms-helper < 3.5.0
Obsoletes: glite-wms-manager < 3.5.0
Conflicts: glite-wms-ism < 3.5.0
Conflicts: glite-wms-helper < 3.5.0
Conflicts: glite-wms-manager < 3.5.0
Provides: glite-wms-ism = 3.5.0
Provides: glite-wms-helper = 3.5.0
Provides: glite-wms-manager = 3.5.0

%global debug_package %{nil}

%description
Core logic for the Workload Management System

%prep
 
%setup -c -q

%build
%{!?extbuilddir:%define extbuilddir "-"}
if test "x%{extbuilddir}" == "x-" ; then
  ./configure --prefix=%{buildroot}/usr --sysconfdir=%{buildroot}/etc --disable-static PVER=%{version}
  make
fi

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}
if test "x%{extbuilddir}" == "x-" ; then
  make install
else
  cp -R %{extbuilddir}/* %{buildroot}
fi
# stripping rpath and symbols
strip %{buildroot}%{_libdir}/libglite_wms_*.so.0.0.0
chrpath --delete %{buildroot}%{_libdir}/libglite_wms_*.so.0.0.0
chrpath --delete %{buildroot}/usr/bin/glite-wms-workload_manager
strip -s %{buildroot}/usr/bin/glite-wms-workload_manager
chrpath --delete %{buildroot}/usr/bin/glite-wms-query-job-state-transitions
strip -s %{buildroot}/usr/bin/glite-wms-query-job-state-transitions
chmod 0755 %{buildroot}/usr/bin/glite-wms-stats
export QA_SKIP_BUILD_ROOT=yes

%clean
rm -rf %{buildroot}

%post
/sbin/chkconfig --add glite-wms-wm 
/sbin/ldconfig

%postun -p /sbin/ldconfig


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

%package devel
Summary: Development files for the WMS information superkmarket
Group: System Environment/Libraries

%description devel
Development files for the WMS core module

%files devel
%defattr(-,root,root)
%dir /usr/include/glite/
%dir /usr/include/glite/wms/
%dir /usr/include/glite/wms/ism/
%dir /usr/include/glite/wms/ism/purchaser/
%dir /usr/include/glite/wms/helper/
%dir /usr/include/glite/wms/helper/jobadapter
/usr/include/glite/wms/ism/*.h
/usr/include/glite/wms/ism/purchaser/*.h
%dir /usr/include/glite/wms/helper/*.h
%dir /usr/include/glite/wms/helper/jobadapter/*.h
%{_libdir}/libglite_wms_*.so
%{_libdir}/libglite_wms_*.la

%changelog
* %{extcdate} WMS group <wms-support@lists.infn.it> - %{extversion}-%{extage}.%{extdist}
- %{extclog}
