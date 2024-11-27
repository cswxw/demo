Name:     demo
Version:  0.0.1
Release:  1%{?dist}
Summary:  this is demo program!
License:  GPL
URL:      http://www.demo.cn/
SOURCE0:  demo-0.0.1.tar.gz

%description
this is used for demo!


%prep
rm -rf %{_builddir}/*  #删除之前
%setup -q   #解压


%build
./configure #编译
make

%install
rm -rf %{buildroot}/*
make install DESTDIR=$RPM_BUILD_ROOT   #安装



%files 
/usr/local/*  #打包的文件


%changelog



