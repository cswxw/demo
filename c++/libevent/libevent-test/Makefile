

CFLAGS := -g -D_DEBUG -std=c99 -I/root/code/libevent/libevent-static-debug/include -L/root/code/libevent/libevent-static-debug/lib  -Wl,-Bstatic  -levent -levent_core -levent_extra -Wl,-Bdynamic 

all : client server
client : client.o
	gcc  -o $@ $^ $(CFLAGS)

server : server.o
	gcc  -o $@ $^ $(CFLAGS)
	
clean:
	-rm client client.o server server.o -rf