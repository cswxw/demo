Name:     demo
Version:  1.0
Release:  1%{?dist}
Summary:  this is demo program!
License:  GPL
URL:      http://www.demo.cn/
%description
this is used for demo!


%install
cp -R * %{buildroot}


%files 
/*


%changelog
* Sun Dec 4 1990 Your Name <demo@xxx.xxx> - 1.1
- Update to 1.1
* Sat Dec 3 1990 Your Name <demo@xxx.xxx> - 1.0
- Update to 1.0



