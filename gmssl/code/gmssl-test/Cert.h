#ifndef _MY_CERT_H_
#define _MY_CERT_H_
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

#include <openssl/rand.h>
#include <openssl/rsa.h>

#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/bn.h>
#include "MyDebug.h"

class CCert
{
private:
	EVP_PKEY* m_pub_key;
	EVP_PKEY* m_pri_key;
public:
	int load_public_cert(const char *certfile);  
	int load_private_key(const char *keyfile, const  char *passwd) ;

	int rsa_key_encrypt(const unsigned char *orig_data, size_t orig_data_len,   
                    unsigned char *enc_data, size_t &enc_data_len);

	int rsa_key_decrypt(const unsigned char *enc_data, size_t enc_data_len,   
                    unsigned char *orig_data, size_t &orig_data_len, const  char *passwd);
	
	//filehash
	//# define EVP_MAX_MD_SIZE                 64/* longest known is SHA512 */
	int getFilesm3(char * filenpath,unsigned char* filehash,unsigned int* md_len);

	
	// do_encrypt 1
	// do_encrypt 0
	int sm4_do_crypt(char *filein, char *fileout,unsigned char *key,int do_encrypt);

	CCert(void);
	virtual ~CCert(void);
};

#endif
