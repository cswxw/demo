CFLAGS= -g `pkg-config --libs --cflags dbus-1 dbus-glib-1`
all: client server

server:
	gcc server.cpp -o server $(CFLAGS)

client:
	gcc client.cpp -o client $(CFLAGS)
test:
	gcc test.cpp -o test $(CFLAGS)

sync:
	gcc sync.cpp -o sync $(CFLAGS)

async:
	gcc async.cpp -o async $(CFLAGS)	
	
