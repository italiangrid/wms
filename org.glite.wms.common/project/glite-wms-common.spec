Summary: Common libraries for the Workload Management System
Name: glite-wms-common
Version: %{extversion}
Release: %{extage}.%{extdist}
License: ASL 2.0
Vendor: EMI
URL: http://glite.cern.ch/
Group: System Environment/Libraries
BuildArch: %{_arch}
BuildRequires: %{!?extbuilddir: glite-jobid-api-c-devel,} chrpath
BuildRequires: %{!?extbuilddir: glite-jobid-api-cpp-devel,} libtool, gcc, gcc-c++, cmake
BuildRequires: %{!?extbuilddir: glite-wms-utils-exception-devel,} boost-devel
BuildRequires: %{!?extbuilddir: glite-wms-utils-classad-devel,} classads-devel
BuildRequires: globus-common-devel, globus-ftp-client-devel
BuildRequires: globus-gss-assist-devel, globus-io-devel
BuildRequires: %{!?extbuilddir: glite-build-common-cpp, } cppunit-devel, openldap-devel, log4cpp, log4cpp-devel
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
AutoReqProv: yes
Source: %{name}-%{version}-%{release}.tar.gz

%global debug_package %{nil}

%description
Common libraries for the Workload Management System

%prep
 

%setup -c -q

%build
%{!?extbuilddir:%define extbuilddir "--"}
if test "x%{extbuilddir}" == "x--" ; then
  ./configure --srcdir=$PWD --prefix=%{buildroot}/usr --disable-static PVER=%{version}
  chmod u+x $PWD/src/scripts/generator.pl
  for hfile in `ls $PWD/src/configuration/*.h.G`; do
    $PWD/src/scripts/generator.pl $PWD/src/configuration/Configuration.def -H $hfile
  done
  for cfile in `ls $PWD/src/configuration/*.cpp.G`; do
    $PWD/src/scripts/generator.pl $PWD/src/configuration/Configuration.def -c $cfile
  done
  #cmake -DPREFIX:string=%{buildroot}/usr -DPVER:string=%{version} .
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
sed 's|^prefix=.*|prefix=/usr|g' %{buildroot}%{_libdir}/pkgconfig/wms-common.pc > %{buildroot}%{_libdir}/pkgconfig/wms-common.pc.new
mv %{buildroot}%{_libdir}/pkgconfig/wms-common.pc.new %{buildroot}%{_libdir}/pkgconfig/wms-common.pc
sed 's|^prefix=.*|prefix=/usr|g' %{buildroot}%{_libdir}/pkgconfig/wms-common-ii.pc > %{buildroot}%{_libdir}/pkgconfig/wms-common-ii.pc.new
mv %{buildroot}%{_libdir}/pkgconfig/wms-common-ii.pc.new %{buildroot}%{_libdir}/pkgconfig/wms-common-ii.pc
sed 's|^prefix=.*|prefix=/usr|g' %{buildroot}%{_libdir}/pkgconfig/wms-common-process.pc > %{buildroot}%{_libdir}/pkgconfig/wms-common-process.pc.new
mv %{buildroot}%{_libdir}/pkgconfig/wms-common-process.pc.new %{buildroot}%{_libdir}/pkgconfig/wms-common-process.pc
sed 's|^prefix=.*|prefix=/usr|g' %{buildroot}%{_libdir}/pkgconfig/wms-common-util.pc > %{buildroot}%{_libdir}/pkgconfig/wms-common-util.pc.new
mv %{buildroot}%{_libdir}/pkgconfig/wms-common-util.pc.new %{buildroot}%{_libdir}/pkgconfig/wms-common-util.pc
sed 's|^prefix=.*|prefix=/usr|g' %{buildroot}%{_libdir}/pkgconfig/wms-common-logger.pc > %{buildroot}%{_libdir}/pkgconfig/wms-common-logger.pc.new
mv %{buildroot}%{_libdir}/pkgconfig/wms-common-logger.pc.new %{buildroot}%{_libdir}/pkgconfig/wms-common-logger.pc
sed 's|^prefix=.*|prefix=/usr|g' %{buildroot}%{_libdir}/pkgconfig/wms-common-conf.pc > %{buildroot}%{_libdir}/pkgconfig/wms-common-conf.pc.new
mv %{buildroot}%{_libdir}/pkgconfig/wms-common-conf.pc.new %{buildroot}%{_libdir}/pkgconfig/wms-common-conf.pc
sed 's|^prefix=.*|prefix=/usr|g' %{buildroot}%{_libdir}/pkgconfig/wms-common-quota.pc > %{buildroot}%{_libdir}/pkgconfig/wms-common-quota.pc.new
mv %{buildroot}%{_libdir}/pkgconfig/wms-common-quota.pc.new %{buildroot}%{_libdir}/pkgconfig/wms-common-quota.pc
rm -f %{buildroot}%{_libdir}/*.la
strip -s %{buildroot}%{_libdir}/libglite_wms_*.so.0.0.0
strip -s %{buildroot}/usr/sbin/glite-wms-quota-adjust
strip -s %{buildroot}/usr/bin/glite-wms-get-configuration
strip -s %{buildroot}/usr/libexec/glite-wms-eval_ad_expr
chrpath --delete %{buildroot}%{_libdir}/libglite_wms_*.so.0.0.0
chrpath --delete %{buildroot}/usr/sbin/glite-wms-quota-adjust
chrpath --delete %{buildroot}/usr/bin/glite-wms-get-configuration
chrpath --delete %{buildroot}/usr/libexec/glite-wms-eval_ad_expr 
export QA_SKIP_BUILD_ROOT=yes

%clean
rm -rf %{buildroot}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root)
%{_libdir}/libglite_wms_*.so.0.0.0
%{_libdir}/libglite_wms_*.so.0
%dir /usr/share/doc/glite-wms-common-%{version}/
%doc /usr/share/doc/glite-wms-common-%{version}/LICENSE
/usr/sbin/glite-wms-quota-adjust
/usr/bin/glite-wms-get-configuration
/usr/libexec/glite-wms-eval_ad_expr

%package devel
Summary: Development files for WMS common module
Group: System Environment/Libraries
Requires: %{name}%{?_isa} = %{version}-%{release}
Requires: glite-jobid-api-c-devel, glite-jobid-api-cpp-devel
Requires: glite-wms-utils-exception-devel
Requires: glite-wms-utils-classad-devel, glite-build-common-cpp
Requires: globus-common-devel, globus-ftp-client-devel
Requires: globus-gss-assist-devel, globus-io-devel

%description devel
Development files for WMS common module

%files devel
%defattr(-,root,root)
%dir /usr/include/glite/
%dir /usr/include/glite/wms/
%dir /usr/include/glite/wms/common/
%dir /usr/include/glite/wms/common/logger/
%dir /usr/include/glite/wms/common/configuration/
%dir /usr/include/glite/wms/common/utilities/
%dir /usr/include/glite/wms/common/process/

/usr/include/glite/wms/common/logger/*.h
/usr/include/glite/wms/common/configuration/*.h
/usr/include/glite/wms/common/utilities/*.h
/usr/include/glite/wms/common/process/*.h
%{_libdir}/pkgconfig/wms-common*.pc
%{_libdir}/libglite_wms_*.so

%changelog
* %{extcdate} WMS group <wms-support@lists.infn.it> - %{extversion}-%{extage}.%{extdist}
- %{extclog}

