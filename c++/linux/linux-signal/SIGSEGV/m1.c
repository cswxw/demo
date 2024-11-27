#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>


/*
0x40076d
0x400850
0x400866
0x7ff522960445
0x400679

Obtained 5 stack frames.
./m1() [0x40076d]
./m1() [0x400850]
./m1() [0x400866]
/usr/lib64/libc.so.6(__libc_start_main+0xf5) [0x7ff522960445]
./m1() [0x400679]
*/
/* Obtain a backtrace and print it to @code{stdout}. */
void print_trace (void)
{
		void *array[10]={0};
		size_t size;
		char **strings;
		size_t i;
		size = backtrace (array, 10);   
		for(i=0; i < size;i++){
			printf("%p\n", (long*)array[i]);   // bt address
		}
		printf("\n");
		strings = backtrace_symbols (array, size);
		if (NULL == strings)
		{
				perror("backtrace_synbols");
				exit(EXIT_FAILURE);
		}

		printf ("Obtained %zd stack frames.\n", size);

		for (i = 0; i < size; i++)
				printf ("%s\n", strings[i]);  // bt address and info

		free (strings);
		strings = NULL;
}

/* A dummy function to make the backtrace more interesting. */
void dummy_function (void)
{
		print_trace ();
}

int main (int argc, char *argv[])
{
		dummy_function ();

		return 0;
}
