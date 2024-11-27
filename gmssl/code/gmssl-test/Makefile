CXXFLAGS += -lcrypto  -L/usr/local/lib64 -lssl -g3 -D_DEBUG 
main: main.o MyDebug.o Cert.o
	g++ -o $@ $^ -lcrypto  -L/usr/local/lib64 -lssl -g3
	-rm -f *.o

test: test.o MyDebug.o Cert.o
	g++ -o $@ $^ -lcrypto  -L/usr/local/lib64 -lssl -g3
	-rm -f *.o
