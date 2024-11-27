#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <internal/x509_int.h>
#include <openssl/pem.h>
#include <openssl/cms.h>
#include <openssl/e_os2.h>
#include <openssl/pkcs12.h>
#include <openssl/bio.h>
struct bio_st {
	const BIO_METHOD *method;
	/* bio, mode, argp, argi, argl, ret */
	long(*callback) (struct bio_st *, int, const char *, int, long, long);
	char *cb_arg;               /* first argument for the callback */
	int init;
	int shutdown;
	int flags;                  /* extra storage */
	int retry_reason;
	int num;
	void *ptr;
	struct bio_st *next_bio;    /* used by filter BIOs */
	struct bio_st *prev_bio;    /* used by filter BIOs */
	int references;
	uint64_t num_read;
	uint64_t num_write;
	CRYPTO_EX_DATA ex_data;
	CRYPTO_RWLOCK *lock;
};
void getCertInfo(char* path)
{
	BIO *bio = NULL;
	PKCS12 *p12 = NULL;
	EVP_PKEY *pKey = NULL;
	X509 *x509 = NULL;
	BIO *x509Bio = NULL;
	char *szCertInfo = NULL;

	bio = BIO_new_file(path, "r");
	if (!bio)
	{
		goto free_all;
	}

	p12 = d2i_PKCS12_bio(bio, NULL);
	if (!p12)
	{
		goto free_all;
	}

	if (PKCS12_parse(p12, NULL, &pKey, &x509, NULL) != 1)
	{
		goto free_all;
	}

	x509Bio = BIO_new(BIO_s_mem());
	X509_print(x509Bio, x509);
	if (x509Bio->num_write == 0)
	{
		goto free_all;
	}

	szCertInfo = (char*)malloc(x509Bio->num_write + 1);
	if (!szCertInfo)
	{
		goto free_all;
	}

	memset(szCertInfo, 0, x509Bio->num_write + 1);
	BIO_read(x509Bio, szCertInfo, x509Bio->num_write);




free_all:
	if (bio)
		BIO_free(bio);
	if (p12)
		PKCS12_free(p12);
	if (x509Bio)
		BIO_free(x509Bio);
	if (szCertInfo)
		free(szCertInfo);

}
void main3()
{
	OpenSSL_add_all_algorithms();

}