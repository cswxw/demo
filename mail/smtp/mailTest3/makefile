OBJ=smtpmail.o main.o
std=std=c++11
gdb=

main:$(OBJ)
	g++ $(OBJ) $(gdb) -o main

main.o:smtpmail.h main.cc
	g++ -$(std) $(gdb) -c main.cc

smtpmail.o:log.h base64encode.h smtpmail.h smtpmail.cc
	g++ -$(std) $(gdb) -c smtpmail.cc

.PHONY:clear
clear:
	rm *.o main
