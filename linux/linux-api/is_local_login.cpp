#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <utmp.h>

/*
-1: unknown
0 : remote login
1 : local login

*/
int is_local_login()
{
	struct utmp	utsearch;
	struct utmp	*utent;
	const char	*from;
	int			i;
	from = ttyname(STDIN_FILENO);//:0
	if (!from || !(*from))
	{
		printf("Couldn't retrieve the tty name, aborting.\n");
		return (-1);
	}
	if (!strncmp(from, "/dev/", strlen("/dev/")))
		from += strlen("/dev/");
	printf("Authentication request from tty %s\n", from);
	strncpy(utsearch.ut_line, from, sizeof(utsearch.ut_line) - 1);
	setutent();
	utent = getutline(&utsearch);
	endutent();
	if (!utent)
	{
		printf("No utmp entry found for tty \"%s\"\n",from);
		return (-1);
	}
	for (i = 0; i < 4; ++i)
	{
		if (utent->ut_addr_v6[i] != 0)
		{
			printf("Remote authentication request: %s\n", utent->ut_host);
			return (0);
		}
	}
	printf("Caller is local (good)\n");
	return (1);
}


