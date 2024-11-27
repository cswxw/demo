#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/cms.h>
#include <openssl/ssl.h>
#include <openssl/e_os2.h>
void encodeCert(char *pDecode, int *len) {
	char strData[100] = "this is a test!";
	RSA* rsa = RSA_new();
	BIO *b = BIO_new_file("cert.crt", "r");
	if (b == NULL) { printf("%s\n", ERR_error_string(ERR_get_error(), NULL));  return; }
	X509 *x = PEM_read_bio_X509(b, NULL, NULL, NULL);
	if (x == NULL) { printf("%s\n", ERR_error_string(ERR_get_error(), NULL));  return; }
	EVP_PKEY *k = X509_get_pubkey(x);
	if (k == NULL) { printf("%s\n", ERR_error_string(ERR_get_error(), NULL));  return; }
	rsa = EVP_PKEY_get1_RSA(k);
	if (rsa == NULL) { printf("%s\n", ERR_error_string(ERR_get_error(), NULL));  return; }
	//int verson = X509_get_version(x);

	int nLen = RSA_size(rsa);
	printf("RSA_size:%d\n", nLen);
	//char* pDecode = new char[nLen + 1];
	*len = nLen;
	int ret = RSA_public_encrypt(strlen(strData), (const unsigned char*)strData, (unsigned char*)pDecode, rsa, RSA_PKCS1_PADDING);
	if (ret >= 0)
	{
		printf("encode:%s\n", pDecode);
	}
	else
	{
		printf("%s\n", ERR_error_string(ERR_get_error(), NULL));
	}
}
void decodeCert(char *encodeData, int dataLen) {
	FILE* hPriKeyFile = fopen("rsa_private.pem", "r");
	if (hPriKeyFile == NULL)
	{

		return;
	}
	//fclose(hPriKeyFile);

	RSA* pRSAPriKey = RSA_new();
	if (PEM_read_RSAPrivateKey(hPriKeyFile, &pRSAPriKey, 0, 0) == NULL)
	{
		return;
	}
	int nLen = RSA_size(pRSAPriKey);
	char* pDecode = new char[nLen + 1];

	int ret = RSA_private_decrypt(dataLen, (const unsigned char*)encodeData, (unsigned char*)pDecode, pRSAPriKey, RSA_PKCS1_PADDING);
	if (ret >= 0)
	{
		printf("[decode]:%s\n", pDecode);
	}
	delete[] pDecode;
	RSA_free(pRSAPriKey);
	fclose(hPriKeyFile);
	CRYPTO_cleanup_all_ex_data();
}

int main(int arc, char *argv[]) {
	char encodeData[4096] = { 0 };
	int encodeDataLen = 0;
	/* Load the human readable error strings for libcrypto */
	ERR_load_crypto_strings();

	/* Load all digest and cipher algorithms */
	OpenSSL_add_all_algorithms();

	SSL_library_init();

	encodeCert(encodeData, &encodeDataLen);
	decodeCert(encodeData, encodeDataLen);
	return 0;
}
