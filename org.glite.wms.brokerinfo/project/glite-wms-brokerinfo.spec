Summary:"files for gLite wms brokerinfo"
Name:glite-wms-brokerinfo
Version:@MODULE.VERSION@
Release:@MODULE.BUILD@
Copyright:Open Source EGEE License
Vendor:EU EGEE project
Group:System/Application
Prefix:/opt/glite
Requires: glite-wms-common glite-wms-rls glite-wms-ism
BuildRoot:%{_builddir}/%{name}-%{version}
Source:glite-wms-brokerinfo-@MODULE.VERSION@_bin.tar.gz

%define debug_package %{nil}
%define    __spec_install_post     %{nil}

%description
"brokerinfo"

%prep
 

%setup -c

%build
 

%install
 

%clean
 
%pre
%post
%preun
%postun
%files
%defattr(-,root,root)
%{prefix}/include/glite/wms/brokerinfo/brokerinfo.h
%{prefix}/include/glite/wms/brokerinfo/glue_attributes.h
%{prefix}/lib/libglite_wms_brokerinfo.a
%{prefix}/lib/libglite_wms_rls.so.0.0.0
%{prefix}/lib/libglite_wms_rls.so.0
%{prefix}/lib/libglite_wms_rls.so
%{prefix}/lib/libglite_wms_rls.a
%{prefix}/lib/libglite_wms_dli_sici.so.0.0.0
%{prefix}/lib/libglite_wms_dli_sici.so.0
%{prefix}/lib/libglite_wms_dli_sici.so
%{prefix}/lib/libglite_wms_dli_sici.a
%{prefix}/share/doc/glite-wms-brokerinfo-@MODULE.VERSION@/LICENSE

%changelog

