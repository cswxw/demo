#!/bin/sh


g++ -g -I../include -c -o pkwrite.o pkwrite.c
g++ -o test pkwrite.o ../lib/linux-x86_64/*.a -ldl
