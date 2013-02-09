# no post install
#%define __os_install_post %{nil}
# no strip
#%define __strip /bin/true
# no debug package
#%define debug_package %{nil}
# disable automatic dependency processing
#AutoReqProv: no

Name:           USense
Version:1.0.0
Release:        1
License:        GNU Public License 2.0
Summary:        This is a integrated platform for SNS and Apps
Url:            www.hustnet.info
Group:          B Team.
Source0:        %{name}-%{version}.tar.gz
#BuildRequires:  pkgconfig(QtCore)
BuildRequires:  libqt-devel

%description
# Add here description of the package.

%prep
%setup -q

%build
# Add here commands to configure the package.
#%qmake
#qmake -makefile -nocache QMAKE_STRIP=: PREFIX=%{_prefix}

# Add here commands to compile the package.
#make %{?jobs:-j%jobs}
#make %{?_smp_mflags}
make

%install
# Add here commands to install the package.
#%qmake_install
make install INSTALL_ROOT=%{buildroot}

%files
%defattr(-,root,root,-)
/*
