CXXFLAGS += -g
server-oneshot: server-oneshot.o
	g++ -o $@ $^ -lpthread -g
	-rm -f *.o

