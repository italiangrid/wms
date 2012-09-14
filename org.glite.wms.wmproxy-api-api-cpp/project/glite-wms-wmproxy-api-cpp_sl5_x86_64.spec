Summary: C/C++ libraries for the WM Proxy service
Name: glite-wms-wmproxy-api-cpp
Version: %{extversion}
Release: %{extage}.%{extdist}
License: Apache Software License
Vendor: EMI
URL: http://glite.cern.ch/
Group: System Environment/Libraries
BuildArch: %{_arch}
BuildRequires: %{!?extbuilddir: gridsite-devel,} chrpath, libtool, openssl-devel
BuildRequires: %{!?extbuilddir: glite-wms-wmproxy-interface,} gsoap-devel
BuildRequires: %{!?extbuilddir: glite-build-common-cpp,} doxygen, libxml2-devel
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
AutoReqProv: yes
Source: %{name}-%{version}-%{release}.tar.gz

%global debug_package %{nil}

%define filter_requires_in(P) %{expand: \
%global __filter_req_cmd %{?__filter_req_cmd} %{__grep} -v %{-P} '%*' | \
}

%filter_requires_in /usr/lib64/pkgconfig/.*\.pc$

%description
C/C++ libraries for the WM Proxy service

%prep
 

%setup -c -q

%build
%{!?extbuilddir:%define extbuilddir "--"}
if test "x%{extbuilddir}" == "x--" ; then
  ./configure --prefix=%{buildroot}/usr --disable-static PVER=%{version}
  make
  make doxygen-doc
fi

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}
%{!?extbuilddir:%define extbuilddir "--"}
if test "x%{extbuilddir}" == "x--" ; then
  make install
  mkdir -p %{buildroot}/%{_docdir}/%{name}
  cp -R doc/html %{buildroot}/%{_docdir}/%{name}
else
  cp -R %{extbuilddir}/* %{buildroot}
fi
sed 's|^prefix=.*|prefix=/usr|g' %{buildroot}/usr/lib64/pkgconfig/wmproxy-api-cpp.pc > %{buildroot}/usr/lib64/pkgconfig/wmproxy-api-cpp.pc.new
mv %{buildroot}/usr/lib64/pkgconfig/wmproxy-api-cpp.pc.new %{buildroot}/usr/lib64/pkgconfig/wmproxy-api-cpp.pc
rm %{buildroot}/usr/lib64/*.la
chrpath --delete %{buildroot}/usr/lib64/libglite_wms_wmproxy_api_cpp.so.0.0.0
 

%clean
rm -rf %{buildroot} 

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root)
%dir /usr/share/doc/glite-wms-wmproxy-api-cpp-%{version}/
/usr/share/doc/glite-wms-wmproxy-api-cpp-%{version}/CHANGES
/usr/share/doc/glite-wms-wmproxy-api-cpp-%{version}/LICENSE
/usr/lib64/libglite_wms_wmproxy_api_cpp.so.0
/usr/lib64/libglite_wms_wmproxy_api_cpp.so.0.0.0


%package devel
Summary: C/C++ libraries for the WM Proxy service (development files)
Group: System Environment/Libraries
Requires: %{name}%{?_isa} = %{version}-%{release}
Requires: gridsite-devel, gsoap-devel, libxml2-devel, openssl-devel
AutoReqProv: no

%description devel
C/C++ libraries for the WM Proxy service (development files)

%files devel
%defattr(-,root,root)
/usr/lib64/pkgconfig/wmproxy-api-cpp.pc
/usr/lib64/libglite_wms_wmproxy_api_cpp.so
%dir /usr/include/glite/
%dir /usr/include/glite/wms/
%dir /usr/include/glite/wms/wmproxyapi/
/usr/include/glite/wms/wmproxyapi/wmproxy_api_utilities.h
/usr/include/glite/wms/wmproxyapi/wmproxy_api.h



%package doc
Summary: Documentation files for the WM Proxy service API
Group: Documentation

%description doc
Documentation files for the WM Proxy service API

%files doc
%defattr(-,root,root)
%dir %{_docdir}/%{name}/html
%doc %{_docdir}/%{name}/html/*.html
%doc %{_docdir}/%{name}/html/*.css
%doc %{_docdir}/%{name}/html/*.png
%doc %{_docdir}/%{name}/html/*.gif



%changelog
* %{extcdate} WMS group <wms-support@lists.infn.it> - %{extversion}-%{extage}.%{extdist}
- %{extclog}

