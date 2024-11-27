INCLUDES = \
	$(DBUS_CFLAGS)				\
	$(DBUS_GLIB_CFLAGS)			\
	-DDBUS_COMPILATION

LIBS = \
	$(DBUS_LIBS)				\
	$(DBUS_GLIB_LIBS)			\
	-ldbus-glib-1

# example-service
noinst_PROGRAMS = example-service

example_service_SOURCES = example-service.c

BUILT_SOURCES = example-service-glue.h

example-service-glue.h: example-service.xml
	$(LIBTOOL) --mode=execute dbus-binding-tool --prefix=test_obj --mode=glib-server --output=example-service-glue.h $(srcdir)/example-service.xml

CLEANFILES = $(BUILT_SOURCES)

EXTRA_DIST = example-service.xml 

# example-client
noinst_PROGRAMS += example-client
example_client_SOURCES= example-client.c
