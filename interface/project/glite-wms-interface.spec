Summary: Workload Management Proxy service
Name: glite-wms-interface
Version: %{extversion}
Release: %{extage}.%{extdist}
License: ASL 2.0
Vendor: EMI
URL: http://web.infn.it/gLiteWMS/
Group: Applications/Internet
BuildArch: %{_arch}
#Obsoletes: glite-wms-wmproxy < 3.5.0
#Obsoletes: glite-wms-wmproxy-interface < 3.5.0
Obsoletes: glite-wms-wmproxy
Obsoletes: glite-wms-wmproxy-interface
Provides: glite-wms-wmproxy glite-wms-wmproxy-interface
#Conflicts: glite-wms-wmproxy >= 3.5.0
#Conflicts: glite-wms-wmproxy-interface >= 3.5.0
Provides: glite-wms-wmproxy = 3.5.0, glite-wms-wmproxy-interface = 3.5.0
Requires: mod_fcgid
Requires: httpd
Requires: mod_ssl
Requires: gridsite-apache
Requires: glite-px-proxyrenewal glite-px-proxyrenewal-progs
Requires: %{!?extbuilddir: glite-wms-common, glite-wms-configuration, glite-wms-purger, } lcmaps-plugins-basic
Requires(post): chkconfig
Requires(preun): chkconfig
Requires(preun): initscripts
BuildRequires: glite-jobid-api-c-devel, glite-jobid-api-cpp-devel, voms-devel, chrpath
BuildRequires: gridsite-devel, libxml2-devel
BuildRequires: argus-pep-api-c-devel, libtool
BuildRequires: lcmaps-without-gsi-devel, lcmaps-devel, classads-devel
BuildRequires: glite-jdl-api-cpp-devel, glite-lb-client-devel, fcgi-devel
BuildRequires: glite-px-proxyrenewal-devel, libxslt-devel, libtar-devel
BuildRequires: %{!?extbuilddir: glite-wms-common-devel, glite-wms-purger-devel, } gsoap-devel, glite-build-common-cpp, emi-pkgconfig-compat
BuildRequires: httpd-devel, zlib-devel, boost-devel, c-ares-devel
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
AutoReqProv: yes
Source: %{name}-%{version}-%{release}.tar.gz

%global debug_package %{nil}

%description
Workload Management Proxy service

%prep
 
%setup -c -q

%build
%{!?extbuilddir:%define extbuilddir "--"}
if test "x%{extbuilddir}" == "x--" ; then

  %if %{extdist} == "sl6"
    ln -sf $PWD/src/server/stdsoap2-2_7_16.cpp $PWD/src/server/stdsoap2.cpp
  %else
    ln -sf $PWD/src/server/stdsoap2-2_7_13.cpp $PWD/src/server/stdsoap2.cpp
  %endif

  ./configure --srcdir=$PWD --prefix=%{buildroot}/usr --sysconfdir=%{buildroot}/etc --disable-static PVER=%{version}
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
rm %{buildroot}%{_libdir}/*.la
strip -s %{buildroot}%{_libdir}/*.so.0.0.0
strip -s %{buildroot}/usr/bin/glite_wms_wmproxy_server
strip -s %{buildroot}/usr/libexec/glite_wms_wmproxy_dirmanager
chrpath --delete %{buildroot}%{_libdir}/*.so.0.0.0
chrpath --delete %{buildroot}/usr/bin/glite_wms_wmproxy_server
chrpath --delete %{buildroot}/usr/libexec/glite_wms_wmproxy_dirmanager
export QA_SKIP_BUILD_ROOT=yes

%clean
rm -rf %{buildroot}

%post
/sbin/ldconfig
/sbin/chkconfig --add glite-wms-wmproxy

%preun
if [ $1 -eq 0 ] ; then
    /sbin/service glite-wms-wmproxy stop >/dev/null 2>&1
    /sbin/chkconfig --del glite-wms-wmproxy
fi

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root)
%dir /etc/glite-wms/
%dir /etc/lcmaps/
%config(noreplace) /etc/glite-wms/wmproxy_httpd.conf.template
%config(noreplace) /etc/glite-wms/wmproxy.gacl.template
%config(noreplace) /etc/lcmaps/lcmaps.db.template
/etc/rc.d/init.d/glite-wms-wmproxy
%attr(4755, root, root) /usr/sbin/glite_wms_wmproxy_load_monitor
%attr(0755, root, root) /usr/bin/glite_wms_wmproxy_server
%attr(0755, root, root) /usr/bin/glite-wms-wmproxy-purge-proxycache
%attr(0755, root, root) /usr/bin/glite-wms-wmproxy-purge-proxycache_keys
%dir /usr/share/doc/glite-wms-wmproxy-%{version}/
%doc /usr/share/doc/glite-wms-wmproxy-%{version}/LICENSE
%{_libdir}/libglite_wms_wmproxy_*.so.0.0.0
%{_libdir}/libglite_wms_wmproxy_*.so.0
%{_libdir}/libglite_wms_wmproxy_*.so
%attr(4755, root, root) /usr/libexec/glite_wms_wmproxy_dirmanager

%changelog
* %{extcdate} WMS group <wms-support@lists.infn.it> - %{extversion}-%{extage}.%{extdist}
- %{extclog}
