#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <execinfo.h>
#include <signal.h>

/*
Obtained 8 stack frames.nm
./m2() [0x40078c]
/usr/lib64/libc.so.6(+0x362f0) [0x7f33cad772f0]
./m2() [0x400854]
./m2() [0x400867]
./m2() [0x400877]
./m2() [0x4008b1]
/usr/lib64/libc.so.6(__libc_start_main+0xf5) [0x7f33cad63445]
./m2() [0x400679]

command:
	
	objdump -d m2 > test.s
	vim test.s   and  find 0x40078c   to look asm
  or
  	addr2line 0x40078c -e m2 -f  
	[info]:	dump
			/home/adcsw/work/code/trace/m2.c:14

*/
void dump(int signo)
{
void *buffer[30] = {0};
size_t size;
char **strings = NULL;
size_t i = 0;

size = backtrace(buffer, 30);
fprintf(stdout, "Obtained %zd stack frames.nm\n", size);
strings = backtrace_symbols(buffer, size);
if (strings == NULL)
{
perror("backtrace_symbols.");
exit(EXIT_FAILURE);
}

for (i = 0; i < size; i++)
{
fprintf(stdout, "%s\n", strings[i]);
}
free(strings);
strings = NULL;
exit(0);
}

void func_c()
{
*((volatile char *)0x0) = 0x9999;
}

void func_b()
{
func_c();
}

void func_a()
{
func_b();
}

int main(int argc, const char *argv[])
{
if (signal(SIGSEGV, dump) == SIG_ERR)
perror("can't catch SIGSEGV");
func_a();
return 0;
}
