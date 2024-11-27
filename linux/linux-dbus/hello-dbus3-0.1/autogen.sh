#! /bin/sh
touch `find .`
aclocal
autoconf
autoheader
touch NEWS README AUTHORS ChangeLog
automake --add-missing
