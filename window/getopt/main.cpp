#include "getopt.h"               // 包含头文件
#if 0



//#pragma comment(lib,“getopt.lib”)   // 加载静态库文件（Windows）

#include <stdio.h>

#include <stdlib.h>


#include <string.h>
void display_usage(){
	printf("this is a help\n");
}

int main(int argc, char**argv)
{
	int opt, longIndex;
	struct globalArgs_t {

		int noIndex;                /* -I option */

		char *langCode;             /* -l option */

		const char *outFileName;    /* -o option */

		FILE *outFile;

		int verbosity;              /* -v option */

		char **inputFiles;          /* input files */

		int numInputFiles;          /* # of input files */

		int randomized;             /* --randomize option */

	} globalArgs;

	static const char *optString = "Il:o:vh?";



	static  option longOpts[] = {
		{ "no-index", no_argument, NULL, 'I' },
		{ "language", required_argument, NULL, 'l' },
		{ "output", required_argument, NULL, 'o' },
		{ "verbose", no_argument, NULL, 'v' },
		{ "randomize", no_argument, NULL, 0 },
		{ "help", no_argument, NULL, 'h' },
		{ NULL, no_argument, NULL, 0 }
	};
	while ((opt = getopt_long(argc, argv, optString, longOpts, &longIndex)) != -1) {
		
		switch (opt) {
		case 'I':
			globalArgs.noIndex = 1; /* true */
			break;
			
		case 'l':
			globalArgs.langCode = optarg;
			break;
		case 'o':
			globalArgs.outFileName = optarg;
			break;
		case 'v':
			globalArgs.verbosity++;
			break;
		case 'h':   /* fall-through is intentional */
		case '?':
			display_usage();
			break;
		case 0:     /* long option without a short arg */

			if (strcmp("randomize", longOpts[longIndex].name) == 0) {
				globalArgs.randomized = 1;

			}
			break;
		default:

			/* You won't actually get here. */

			break;

		}
		
	}

}

#endif

#if 1
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*
char *optarg：如果有参数，则包含当前选项参数字符串
int optind：argv的当前索引值。当getopt函数在while循环中使用时，剩下的字符串为操作数，下标从optind到argc-1。
int opterr：这个变量非零时，getopt()函数为“无效选项”和“缺少参数选项，并输出其错误信息。
int optopt：当发现无效选项字符之时，getopt()函数或返回 \’ ? \’ 字符，或返回字符 \’ : \’ ，并且optopt包含了所发现的无效选项字符
*/

int main(int argc, char *argv[])
{
	int flags = 0, opt;
	int nsecs = 0, tfnd = 0;

	while ((opt = getopt(argc, argv, "nt:")) != -1) {
		switch (opt) {
		case 'n':
			flags = 1;
			break;
		case 't':
			nsecs = atoi(optarg);
			tfnd = 1;
			break;
		default:
			printf("optopt = %c\n", (char)optopt);
			printf("opterr = %d\n", opterr);
			fprintf(stderr, "usage: %s [-t nsecs] [-n] name\n", argv[0]);
			exit(EXIT_FAILURE);
		}
	}
	printf("flags = %d; tfnd = %d; nsecs = %d; optind = %d\n", flags, tfnd, nsecs, optind);

	printf("optind = %d\n", optind);
	printf("argc = %d\n", argc);
#if 1 
	if (optind >= argc) {
		fprintf(stderr, "Expected argument after options\n");
		exit(1);
	}
#endif
	printf("name argument = %s\n", argv[optind]);
	/* Other code omitted */
	return 0;
}

#endif