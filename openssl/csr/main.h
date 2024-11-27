#ifndef __MAIN_H__
#define __MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <errno.h>

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
#include <windows.h>


#include <../rsa/rsa_locl.h>

#include <openssl/ossl_typ.h>
#include <../evp/evp_locl.h>
#include <internal/evp_int.h>

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"Crypt32.lib")



struct X509_name_st {
    STACK_OF(X509_NAME_ENTRY) *entries; /* DN components */
    int modified;               /* true if 'bytes' needs to be built */
    BUF_MEM *bytes;             /* cached encoding: cannot be NULL */
    /* canonical encoding used for rapid Name comparison */
    unsigned char *canon_enc;
    int canon_enclen;
} /* X509_NAME */ ;
#undef X509_NAME
typedef struct X509_name_st X509_NAME;

typedef struct X509_name_entry_st
	{
	ASN1_OBJECT *object;
	ASN1_STRING *value;
	int set;
	int size; 	/* temp variable */
	} X509_NAME_ENTRY;


/* This flag in the RSA_METHOD enables the new rsa_sign, rsa_verify functions.
 */
#define RSA_FLAG_SIGN_VER		0x0040



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



#ifdef __cplusplus
}
#endif 



#endif