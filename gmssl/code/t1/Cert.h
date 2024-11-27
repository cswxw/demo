#ifndef _MY_CERT_H_
#define _MY_CERT_H_
#include <windows.h>
#include <wincrypt.h>
#pragma comment(lib,"Crypt32.lib")
#pragma comment(lib,"Ws2_32.lib")
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
	// 公钥和私钥加载
	int load_public_cert(const char *certfile);  //打开证书中的公钥
	int load_private_key(const char *keyfile, const  char *passwd) ;// 打开私钥
	//这种封装格式只适用公钥加密,这里key必须是公钥  
	int rsa_key_encrypt(const unsigned char *orig_data, size_t orig_data_len,   
                    unsigned char *enc_data, size_t &enc_data_len);
	//这种封装格式只适用私钥解密,这里key必须是私钥 
	int rsa_key_decrypt(const unsigned char *enc_data, size_t enc_data_len,   
                    unsigned char *orig_data, size_t &orig_data_len, const  char *passwd);
	
	//filehash 32字节
	//# define EVP_MAX_MD_SIZE                 64/* longest known is SHA512 */
	int getFilesm3(char * filenpath,unsigned char* filehash,unsigned int* md_len);	//获取文件摘要

	
	//sm4 对称算法加密/解密
	// do_encrypt 1加密
	// do_encrypt 0解密
	int sm4_do_crypt(char *filein, char *fileout,unsigned char *key,int do_encrypt);

	CCert(void);
	virtual ~CCert(void);
};

#endif