%define revision 4

Summary: Open Source diagnosis and recovery solution
Name: phronesisAgent
Version: 1.0.0
Release: %{revision}%{?dist}
License: GPLv2
Group: Applications/System
Source0:   %{name}-%version.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}

BuildRequires: gcc

%description
Here I should do a description of the phronesis project


%package lib
Summary: library file for %{name}
Group: Applications/System

%description lib
This packages contain the base library for %{name}


%prep
%setup

%build
sed -e "s:BASETOSED:%{_builddir}/%{name}-%version/:g" %{_builddir}//%{name}-%version/shared_Abs.mk>%{_builddir}/%{name}-%version/shared.mk


%{__make} phronesisanalyzer phronesisanalyzerAgent

%install
%{__rm} -rf %{buildroot}
%{__mkdir} -p %{buildroot}/etc/%{name}
%{__mkdir} -p %{buildroot}/usr/bin
%{__mkdir} -p %{buildroot}/usr/lib
%{__mkdir} -p %{buildroot}/etc/init.d/
cp bin/phronesisAnalyzerAgent %{buildroot}/usr/bin/
cp bin/libphronesisanalyzer.so.1.0 %{buildroot}/usr/lib/
cp default-conf/phronesisAnalyzerAgent.conf %{buildroot}/etc/%{name}
cp init.d/phronesisAgentd %{buildroot}/etc/init.d/


%post
ln -s /usr/lib/libphronesisanalyzer.so.1.0 /usr/lib/libphronesisanalyzer.so
ldconfig
/sbin/service phronesisAnalyzerd start &>/dev/null || :


%clean
%{__rm} -rf %{buildroot}

%files
%defattr(-,root,root,-)
%dir /etc/%{name}
%attr(755,-,-)   /usr/bin/phronesisAnalyzerAgent
%attr(755,-,-)   /etc/init.d/phronesisAgentd
%config(noreplace)   /etc/%{name}/phronesisAnalyzerAgent.conf

%files lib
%defattr(-,root,root,-)
%attr(755,-,-)   /usr/lib/libphronesisanalyzer.so.1.0


%changelog
* Wed Jun 05 2013 Christophe Haen <christophe.haen@cern.ch> - 1.0.0
- initial package

