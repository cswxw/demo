#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <windows.h>
#include <stdlib.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/bio.h>
#include <openssl/aes.h>
#include <openssl/ec.h>

#include <openssl/engine.h>



#include <../crypto/rsa/rsa_locl.h>

#include <openssl/ossl_typ.h>
#include <../crypto/evp/evp_locl.h>
#include <../crypto/include/internal/evp_int.h>
#include <openssl/ossl_typ.h>
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Crypt32.lib")



	





#define LOG_DATA(d, l)\
fprintf(stdout,"======" #d " len:%d"  "  start ======\n",l); \
do\
{\
	int i;\
	for(i=0;i<l;i++)\
	{\
		if((i+1) % 16) \
			printf("%02X ", (unsigned char)d[i]);\
		else\
			printf("%02X\n", (unsigned char)d[i]);\
	}\
	if(i % 16) printf("\n");\
}\
while(0); \
fprintf(stdout,"======" #d "  end ======\n");


#define INFO(format,...) fprintf(stdout , "[info]" __FUNCTION__ " : " format,__VA_ARGS__); 




#define info(...) \
	printf(__VA_ARGS__); \
	printf("\r\n");





#ifdef __cplusplus
}
#endif 



#endif