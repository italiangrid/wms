Summary: Integrated CREAM Environment
Name: glite-wms-ice
Version: %{extversion}
Release: %{extage}.%{extdist}
License: ASL 2.0
Vendor: EMI
URL: http://glite.cern.ch/
Group: Applications/Internet
BuildArch: %{_arch}
Requires: glite-wms-configuration
Requires: glite-px-proxyrenewal
Requires(post): chkconfig
Requires(preun): chkconfig
Requires(preun): initscripts
BuildRequires: %{!?extbuilddir: glite-wms-common-devel, glite-wms-purger-devel, } chrpath
BuildRequires: glite-ce-cream-client-devel, libtool, classads-devel
BuildRequires: glite-px-proxyrenewal-devel, gsoap-devel, boost-devel
BuildRequires: gridsite-devel, libxml2-devel, log4cpp-devel
BuildRequires: glite-build-common-cpp, myproxy-devel, c-ares-devel
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
AutoReqProv: yes
Source: %{name}-%{version}-%{release}.tar.gz

%global debug_package %{nil}

%description
Integrated CREAM Environment

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
%{!?extbuilddir:%define extbuilddir "--"}
if test "x%{extbuilddir}" == "x--" ; then
  make install
else
  cp -R %{extbuilddir}/* %{buildroot}
fi
rm -f %{buildroot}%{_libdir}/*.la
chrpath --delete %{buildroot}%{_libdir}/libglite_wms_*.so.0.0.0
chrpath --delete %{buildroot}/usr/bin/queryStats
chrpath --delete %{buildroot}/usr/bin/glite-wms-ice-safe
chrpath --delete %{buildroot}/usr/bin/putFL
chrpath --delete %{buildroot}/usr/bin/glite-wms-ice-db-rm
chrpath --delete %{buildroot}/usr/bin/putFL_reschedule
chrpath --delete %{buildroot}/usr/bin/glite-wms-ice-rm
chrpath --delete %{buildroot}/usr/bin/glite-wms-ice-proxy-renew
chrpath --delete %{buildroot}/usr/bin/queryDb
chrpath --delete %{buildroot}/usr/bin/glite-wms-ice
export QA_SKIP_BUILD_ROOT=yes

%clean
rm -rf %{buildroot}

%post 
/sbin/ldconfig
/sbin/chkconfig --add glite-wms-ice

%preun
if [ $1 -eq 0 ] ; then
    /sbin/service glite-wms-ice stop >/dev/null 2>&1
    /sbin/chkconfig --del glite-wms-ice
fi

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root)
/etc/rc.d/init.d/glite-wms-ice
/usr/bin/queryDb
/usr/bin/glite-wms-ice-proxy-renew
/usr/bin/glite-wms-ice-db-rm
/usr/bin/glite-wms-ice-safe
/usr/bin/putFL_cancel
/usr/bin/putFL_reschedule
/usr/bin/putFL
/usr/bin/queryStats
/usr/bin/glite-wms-ice-rm
/usr/bin/glite-wms-ice
%dir /usr/share/doc/glite-wms-ice-%{version}/
%doc /usr/share/doc/glite-wms-ice-%{version}/LICENSE
%{_libdir}/libglite_wms_*.so.0.0.0
%{_libdir}/libglite_wms_*.so.0
%{_libdir}/libglite_wms_*.so


%changelog
* %{extcdate} WMS group <wms-support@lists.infn.it> - %{extversion}-%{extage}.%{extdist}
- %{extclog}

