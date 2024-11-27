#include "Cert.h"
#include <unistd.h>
#include <errno.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <sys/mman.h>
#include <fcntl.h>
#include <dirent.h>  
#include <getopt.h>
#include <limits.h>
#include "Envelope.h"
#include <stddef.h>
#define MAX_PATH 260
typedef struct{
	char privateKeyPath[MAX_PATH];
	char certPath[MAX_PATH];
	char inPath[MAX_PATH];
	char signPath[MAX_PATH];
	char outPath[MAX_PATH];
	int  isSign;
    int  isVerfity;
    int  isEnvelopeEncode;
    int  isEnvelopeDecode;
} UserOption,*PUserOption;
CCert cert;
#define alignment_down(a, size) (a & (~(size-1)) )
#define alignment_up(a, size)   ((a+size-1) & (~ (size-1)))
