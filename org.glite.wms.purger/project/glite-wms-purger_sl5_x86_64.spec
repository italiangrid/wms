Summary: Cleanup module for the Workload Management System
Name: glite-wms-purger
Version:
Release:
License: Apache Software License
Vendor: EMI
Packager: WMS group <wms-support@lists.infn.it>
URL: http://glite.cern.ch/
Group: System Environment/Libraries
BuildArch: x86_64
#Requires: glite-wms-common
#Requires: glite-jobid-api-c
#Requires: classads
#Requires: glite-px-proxyrenewal
#Requires: glite-lb-client
#Requires: glite-wms-utils-classad
#Requires: boost
BuildRequires: chrpath
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
AutoReqProv: yes
Source: %{name}-%{version}-%{release}.tar.gz

%description
Cleanup module for the Workload Management System

%prep
 

%setup -c

%build
%{!?extbuilddir:%define extbuilddir "--"}
if test "x%{extbuilddir}" == "x--" ; then
  ./configure --prefix=%{buildroot}/usr --disable-static PVER=%{version}
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
sed 's|^prefix=.*|prefix=/usr|g' %{buildroot}/usr/lib64/pkgconfig/wms-purger.pc > %{buildroot}/usr/lib64/pkgconfig/wms-purger.pc.new
mv %{buildroot}/usr/lib64/pkgconfig/wms-purger.pc.new %{buildroot}/usr/lib64/pkgconfig/wms-purger.pc
rm %{buildroot}/usr/lib64/*.la
chrpath --delete %{buildroot}/usr/lib64/libglite_wms_*.so.0.0.0
 

%clean
rm -rf %{buildroot}

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig


%files
%defattr(-,root,root)
/usr/sbin/glite-wms-purgeStorage
/usr/sbin/glite-wms-create-proxy.sh
/usr/sbin/glite-wms-purgeStorage.sh
%dir /usr/share/doc/glite-wms-purger-%{version}/
%doc /usr/share/doc/glite-wms-purger-%{version}/LICENSE
/usr/lib64/libglite_wms_purger.so.0
/usr/lib64/libglite_wms_purger.so.0.0.0

%changelog


%package devel
Summary: Development files for the WMS purger module
Group: System Environment/Libraries

%description devel
Development files for the WMS purger module

%files devel
%defattr(-,root,root)
%dir /usr/include/glite/
%dir /usr/include/glite/wms/
%dir /usr/include/glite/wms/purger/
/usr/include/glite/wms/purger/purger.h
/usr/lib64/pkgconfig/wms-purger.pc
/usr/lib64/libglite_wms_purger.so

