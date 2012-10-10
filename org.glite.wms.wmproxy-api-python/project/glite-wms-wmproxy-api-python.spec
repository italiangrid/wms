Summary: Python libraries for the WM Proxy service
Name: glite-wms-wmproxy-api-python
Version: %{extversion}
Release: %{extage}.%{extdist}
License: Apache Software License
Vendor: EMI
URL: http://glite.cern.ch/
Group: System Environment/Libraries
BuildArch: noarch
BuildRequires: python-fpconst, PyXML, SOAPpy, python-devel
Requires: python-fpconst, PyXML, SOAPpy
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
AutoReqProv: yes
Source: %{name}-%{version}-%{release}.tar.gz

%if ! (0%{?fedora} > 12 || 0%{?rhel} > 5)
%{!?python_sitelib: %global python_sitelib %(%{__python} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib())")}
%{!?python_sitearch: %global python_sitearch %(%{__python} -c "from distutils.sysconfig import get_python_lib; print(get_python_lib(1))")}
%endif

%description
Python libraries for the WM Proxy service

%prep
 

%setup -c -q

%build
%{!?extbuilddir:%define extbuilddir "--"}
if test "x%{extbuilddir}" == "x--" ; then
  printf "[global]
pkgversion=%{version}" > setup.cfg
fi

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}
%{!?extbuilddir:%define extbuilddir "--"}
if test "x%{extbuilddir}" == "x--" ; then
  python setup.py install -O1 --prefix %{buildroot}/usr --install-data %{buildroot}
else
  cp -pR %{extbuilddir}/* %{buildroot}
fi

%clean
rm -rf %{buildroot}

%files
%defattr(-,root,root)
%dir /usr/share/doc/glite-wms-wmproxy-api-python-%{version}/
%doc /usr/share/doc/glite-wms-wmproxy-api-python-%{version}/LICENSE
%{python_sitelib}/*.py
%{python_sitelib}/*.pyc
%{python_sitelib}/*.pyo
%if %{extdist} == "sl6"
%{python_sitelib}/*.egg-info
%endif


%changelog
* %{extcdate} WMS group <wms-support@lists.infn.it> - %{extversion}-%{extage}.%{extdist}
- %{extclog}
 
