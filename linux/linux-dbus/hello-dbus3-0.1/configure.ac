AC_INIT()
AM_INIT_AUTOMAKE(hello-dbus3, 0.1)
AM_CONFIG_HEADER(config.h)

AC_PROG_CC


# Dbus detection
PKG_CHECK_MODULES(DBUS, dbus-1 >= 1.1, have_dbus=yes, have_dbus=no)

if test x$have_dbus = xno ; then
    AC_MSG_ERROR([DBus development libraries not found])
fi
AM_CONDITIONAL(HAVE_DBUS, test x$have_dbus = xyes)

AC_SUBST(DBUS_CFLAGS)
AC_SUBST(DBUS_LIBS)


# Glib detection
PKG_CHECK_MODULES(DBUS_GLIB, gobject-2.0 >= 2.6, have_glib=yes, have_glib=no)

if test x$have_glib = xno ; then
    AC_MSG_ERROR([GLib development libraries not found])
fi

AM_CONDITIONAL(HAVE_GLIB, test x$have_glib = xyes)

AC_SUBST(DBUS_GLIB_CFLAGS)
AC_SUBST(DBUS_GLIB_LIBS)


AC_OUTPUT([Makefile
           src/Makefile])
