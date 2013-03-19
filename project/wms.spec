Summary: Workload Management System
Name: glite-wms-server
Version: %{extversion}
Release: %{extage}.%{extdist}
License: ASL 2.0
Vendor: EMI
URL: http://web.infn.it/gLiteWMS/
Group: Applications/Internet
BuildArch: %{_arch}
Requires: glite-yaim-core, glite-yaim-lb, glite-px-proxyrenewal,httpd,gridsite-apache,mod_fcgid,mod_ssl,gsoap,
Requires: glite-info-provider-service,glite-initscript-globus-gridftp,glite-jobid-api-c,glite-lb-client,voms,gridsite,
Requires: glite-wms-utils-exception,boost,c-ares,argus-pep-api-c,lcmaps-without-gsi,
Requires: glite-wms-utils-classad,classads,globus-ftp-client,globus-ftp-control,globus-common,globus-ftp-client
Requires: glite-lbjp-common-gsoap-plugin,lcmaps,fcgi,globus-gss-assist,globus-io,glite-jdl-api-cpp
Requires: globus-gram-protocol,condor-emi,openldap,log4cpp,glite-ce-cream-client-api-c,globus-gridftp-server-progs
Requires: lcas-lcmaps-gt4-interface,glite-lb-yaim,globus-proxy-utils,perl-suidperl,fetch-crl,glite-lb-server,glite-lb-logger-msg,bdii,lcmaps-plugins-basic,lcmaps-plugins-voms
BuildRequires: glite-jobid-api-c-devel, chrpath, glite-lb-client-devel,glite-jobid-api-c-devel,voms-devel,gridsite-devel,libxml2-devel
BuildRequires: glite-jobid-api-cpp-devel, glite-jobid-api-c-devel, gcc, gcc-c++, cmake,glite-px-proxyrenewal-devel
BuildRequires: glite-wms-utils-exception-devel, boost-devel, c-ares-devel,argus-pep-api-c-devel,lcmaps-without-gsi-devel,libtar-devel
BuildRequires: glite-wms-utils-classad-devel, classads-devel,globus-ftp-client-devel,globus-ftp-control-devel,docbook-style-xsl
BuildRequires: globus-common-devel, globus-ftp-client-devel,gsoap-devel,glite-lbjp-common-gsoap-plugin-devel,lcmaps-devel,fcgi-devel
BuildRequires: globus-gss-assist-devel, globus-io-devel,glite-jdl-api-cpp-devel,libxslt,globus-gram-protocol-devel,condor-emi,libxslt-devel
BuildRequires: emi-pkgconfig-compat, openldap-devel, log4cpp-devel,docbook-style-xsl
BuildRequires: zlib-devel,httpd-devel,glite-ce-cream-client-devel
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
AutoReqProv: yes
Source: %{name}-%{version}-%{release}.tar.gz

%global debug_package %{nil}

%description
Workload Management System (server and libraries)

%prep
 

%setup -c -q

%build
%{!?extbuilddir:%define extbuilddir "--"}
if test "x%{extbuilddir}" == "x--" ; then
  echo "buildroot=%{buildroot}"
  cmake . -DPREFIX=%{buildroot} -DPVER=%{version} -DOS=%{release} -DMOCKCFG=none
  chmod u+x $PWD/common/src/scripts/generator.pl
  for hfile in `ls $PWD/common/src/configuration/*.h.G`; do
    $PWD/common/src/scripts/generator.pl $PWD/common/src/configuration/Configuration.def -H $hfile
  done
  for cfile in `ls $PWD/common/src/configuration/*.cpp.G`; do
    $PWD/common/src/scripts/generator.pl $PWD/common/src/configuration/Configuration.def -c $cfile
  done
  %if %{extdist} == "sl6"
    ln -sf $PWD/interface/src/server/stdsoap2-2_7_16.cpp $PWD/interface/src/server/stdsoap2.cpp
  %else
    ln -sf $PWD/interface/src/server/stdsoap2-2_7_13.cpp $PWD/interface/src/server/stdsoap2.cpp
  %endif

  make
  sh configuration/install.sh %{buildroot} %{version}
fi

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}
%{!?extbuilddir:%define extbuilddir "--"}
if test "x%{extbuilddir}" == "x--" ; then
  make install
  
else
  echo "extbuilddir=%{extbuilddir}"
  cp -R %{extbuilddir}/stage/* %{buildroot}
fi
sed 's|^prefix=.*|prefix=/usr|g' %{buildroot}%{_libdir}/pkgconfig/wms-server.pc > %{buildroot}%{_libdir}/pkgconfig/wms-server.pc.new
mv %{buildroot}%{_libdir}/pkgconfig/wms-server.pc.new %{buildroot}%{_libdir}/pkgconfig/wms-server.pc
strip -s %{buildroot}%{_libdir}/libglite_wms_*.so.0.0.0
strip -s %{buildroot}/usr/sbin/glite-wms-quota-adjust
strip -s %{buildroot}/usr/bin/glite-wms-get-configuration
strip -s %{buildroot}/usr/libexec/glite-wms-eval_ad_expr
chrpath --delete %{buildroot}%{_libdir}/libglite_wms_*.so.0.0.0
chrpath --delete %{buildroot}/usr/sbin/glite-wms-quota-adjust
chrpath --delete %{buildroot}/usr/bin/glite-wms-get-configuration
chrpath --delete %{buildroot}/usr/libexec/glite-wms-eval_ad_expr 

strip -s %{buildroot}/usr/sbin/glite-wms-purgeStorage 
chrpath --delete %{buildroot}/usr/sbin/glite-wms-purgeStorage 
chrpath --delete %{buildroot}/usr/bin/glite-wms-workload_manager
strip -s %{buildroot}/usr/bin/glite-wms-workload_manager
chrpath --delete %{buildroot}/usr/bin/glite-wms-query-job-state-transitions
strip -s %{buildroot}/usr/bin/glite-wms-query-job-state-transitions
chmod 0755 %{buildroot}/usr/bin/glite-wms-stats
chrpath --delete %{buildroot}/usr/bin/glite-wms-log_monitor
chrpath --delete %{buildroot}/usr/libexec/glite-wms-lm-job_status
chrpath --delete %{buildroot}/usr/bin/glite-wms-job_controller
strip -s %{buildroot}/usr/bin/glite-wms-job_controller
strip -s %{buildroot}/usr/bin/glite-wms-log_monitor
strip -s %{buildroot}/usr/libexec/glite-wms-lm-job_status
strip -s %{buildroot}/usr/bin/glite_wms_wmproxy_server
strip -s %{buildroot}/usr/libexec/glite_wms_wmproxy_dirmanager
strip -s %{buildroot}/usr/bin/glite-wms-ice-safe
strip -s %{buildroot}/usr/bin/glite-wms-ice-putfl
strip -s %{buildroot}/usr/bin/glite-wms-ice-db-rm
strip -s %{buildroot}/usr/bin/glite-wms-ice-rm
strip -s %{buildroot}/usr/bin/glite-wms-ice-query-db
strip -s %{buildroot}/usr/bin/glite-wms-ice-putfl-cancel
strip -s %{buildroot}/usr/bin/glite-wms-ice-query-stats
strip -s %{buildroot}/usr/bin/glite-wms-ice-proxy-renew
strip -s %{buildroot}/usr/bin/glite-wms-ice
strip -s %{buildroot}/usr/bin/glite-wms-ice-putfl-reschedule
chrpath --delete %{buildroot}/usr/bin/glite_wms_wmproxy_server
chrpath --delete %{buildroot}/usr/libexec/glite_wms_wmproxy_dirmanager
chrpath --delete %{buildroot}%{_libdir}/libglite_wms_*.so.0.0.0
chrpath --delete %{buildroot}/usr/bin/glite-wms-ice-query-stats
chrpath --delete %{buildroot}/usr/bin/glite-wms-ice-safe
chrpath --delete %{buildroot}/usr/bin/glite-wms-ice-putfl
chrpath --delete %{buildroot}/usr/bin/glite-wms-ice-db-rm
chrpath --delete %{buildroot}/usr/bin/glite-wms-ice-putfl-reschedule
chrpath --delete %{buildroot}/usr/bin/glite-wms-ice-rm
chrpath --delete %{buildroot}/usr/bin/glite-wms-ice-proxy-renew
chrpath --delete %{buildroot}/usr/bin/glite-wms-ice-query-db
chrpath --delete %{buildroot}/usr/bin/glite-wms-ice

export QA_SKIP_BUILD_ROOT=yes

%clean
rm -rf %{buildroot}

%post 
/sbin/chkconfig --add glite-wms-wm 
/sbin/chkconfig --add glite-wms-lm
/sbin/chkconfig --add glite-wms-jc
/sbin/chkconfig --add glite-wms-wmproxy
/sbin/chkconfig --add glite-wms-ice
/sbin/ldconfig

%postun 
rm -f /opt/glite/share/man/man1/yaim-WMS.1*
/sbin/ldconfig

%preun
if [ $1 -eq 0 ] ; then
    /sbin/service glite-wms-wm stop >/dev/null 2>&1
    /sbin/chkconfig --del glite-wms-wm
fi
if [ $1 -eq 0 ] ; then
    /sbin/service  glite-wms-lmstop >/dev/null 2>&1
    /sbin/chkconfig --del glite-wms-lm
    /sbin/service glite-wms-jc stop >/dev/null 2>&1
    /sbin/chkconfig --del glite-wms-jc
fi
if [ $1 -eq 0 ] ; then
    /sbin/service glite-wms-wmproxy stop >/dev/null 2>&1
    /sbin/chkconfig --del glite-wms-wmproxy
fi
if [ $1 -eq 0 ] ; then
    /sbin/service glite-wms-ice stop >/dev/null 2>&1
    /sbin/chkconfig --del glite-wms-ice
fi


%files
%defattr(-,root,root)
%{_libdir}/libglite_wms_*.so.0.0.0
%{_libdir}/libglite_wms_*.so.0
%dir /usr/share/doc/glite-wms-server-%{version}/
%doc /usr/share/doc/glite-wms-server-%{version}/LICENSE
/usr/sbin/glite-wms-quota-adjust
/usr/bin/glite-wms-get-configuration
/usr/libexec/glite-wms-eval_ad_expr
/usr/sbin/glite-wms-purgeStorage
/usr/sbin/glite-wms-create-proxy.sh
/usr/sbin/glite-wms-purgeStorage.sh
/etc/rc.d/init.d/glite-wms-wm
/usr/bin/glite-wms-workload_manager
/usr/bin/glite-wms-stats
/usr/bin/glite-wms-query-job-state-transitions
%dir /usr/share/glite-wms/
/usr/share/glite-wms/jobwrapper.template.sh
/etc/init.d/glite-wms-lm
/etc/init.d/glite-wms-jc
/usr/bin/glite-wms-job_controller
/usr/bin/glite-wms-log_monitor
/usr/libexec/glite-wms-lm-job_status
/usr/libexec/glite-wms-clean-lm-recycle.sh
%doc /usr/share/man/man1/glite-wms-*.1.gz
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
%{_libdir}/libglite_wms_wmproxy_*.so
%attr(4755, root, root) /usr/libexec/glite_wms_wmproxy_dirmanager
/etc/rc.d/init.d/glite-wms-ice
/usr/bin/glite-wms-ice-query-db
/usr/bin/glite-wms-ice-proxy-renew
/usr/bin/glite-wms-ice-db-rm
/usr/bin/glite-wms-ice-safe
/usr/bin/glite-wms-ice-putfl-cancel
/usr/bin/glite-wms-ice-putfl-reschedule
/usr/bin/glite-wms-ice-putfl
/usr/bin/glite-wms-ice-query-stats
/usr/bin/glite-wms-ice-rm
/usr/bin/glite-wms-ice
#%{_libdir}/libglite_wms_ice*.so
%config(noreplace) /etc/glite-wms/wms.conf.template
/usr/libexec/glite-wms-parse-configuration.sh
/usr/libexec/glite-wms-check-daemons.sh
/usr/libexec/glite-wms-services-certs.sh
/opt/glite/yaim/functions/config_*
%config(noreplace) /opt/glite/yaim/node-info.d/glite-*
/opt/glite/share/man/man1/glite-WMS.1.gz
/opt/glite/yaim/defaults/glite-*
/opt/glite/yaim/services/glite-wms


%package devel
Summary: Development files for WMS common module
Group: System Environment/Libraries
Requires: %{name}%{?_isa} = %{version}-%{release}
Requires: glite-jobid-api-c-devel, glite-jobid-api-cpp-devel
Requires: glite-wms-utils-exception-devel, glite-wms-utils-classad-devel
Requires: globus-common-devel, globus-ftp-client-devel,c-ares-devel
Requires: globus-gss-assist-devel, globus-io-devel,glite-lb-client-devel,boost-devel

%description devel
Development files for WMS

%files devel
%defattr(-,root,root)
%dir /usr/include/glite/
%dir /usr/include/glite/wms/
%dir /usr/include/glite/wms/common/
%dir /usr/include/glite/wms/common/logger/
%dir /usr/include/glite/wms/common/configuration/
%dir /usr/include/glite/wms/common/utilities/
%dir /usr/include/glite/wms/common/process/
%dir /usr/include/glite/wms/purger/
%dir /usr/include/glite/wms/ism/
%dir /usr/include/glite/wms/ism/purchaser/
%dir /usr/include/glite/wms/helper/
%dir /usr/include/glite/wms/helper/jobadapter
/usr/include/glite/wms/ism/*.h
/usr/include/glite/wms/ism/purchaser/*.h
%dir /usr/include/glite/wms/helper/*.h
%dir /usr/include/glite/wms/helper/jobadapter/*.h
/usr/include/glite/wms/common/logger/*.h
/usr/include/glite/wms/common/configuration/*.h
/usr/include/glite/wms/common/utilities/*.h
/usr/include/glite/wms/common/process/*.h
/usr/include/glite/wms/purger/purger.h
%{_libdir}/pkgconfig/wms-server.pc
%{_libdir}/libglite_wms_*.so
%dir /usr/include/glite/wms/jobsubmission/
/usr/include/glite/wms/jobsubmission/SubmitAdapter.h

%changelog
* %{extcdate} WMS group <wms-support@lists.infn.it> - %{extversion}-%{extage}.%{extdist}
- %{extclog}
