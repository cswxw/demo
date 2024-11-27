/*
update time : 2018-11-29
author : create by csw
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/bn.h>
#include <openssl/ec.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/engine.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/rand.h>
#include <openssl/sm2.h>
//#include "../crypto/sm2/sm2_lcl.h"

//look
//sm2evptest.c
//pkeyutl.c init_ctx()
//EVP_PKEY *load_pubkey(const char *file, int format, int maybe_stdin,const char *pass, ENGINE *e, const char *key_descrip)

BIO *bio_err ;
BIO *bio_stdout ;


 char g_pubkey[] = ""
"-----BEGIN PUBLIC KEY-----\r\n"
"MFkwEwYHKoZIzj0CAQYIKoEcz1UBgi0DQgAEu4kSnox0T5SND9KGGlpTvcQ5hiz0\r\n"
"Wm7kigVRTEc0+5wrSoyvnZYuYKLFSSzK2ZbMH+etX7UeRSiPplD63fChNw==\r\n"
"-----END PUBLIC KEY-----\r\n"
"";

 char g_prikey[] = ""
"-----BEGIN PRIVATE KEY-----\r\n"
"MIGHAgEAMBMGByqGSM49AgEGCCqBHM9VAYItBG0wawIBAQQgQnx5UCnYOJv558pc\r\n"
"rU9mgorePGbbF8/JxsR9N8LrKAmhRANCAAS7iRKejHRPlI0P0oYaWlO9xDmGLPRa\r\n"
"buSKBVFMRzT7nCtKjK+dli5gosVJLMrZlswf561ftR5FKI+mUPrd8KE3\r\n"
"-----END PRIVATE KEY-----\r\n"
"";


//TODO   sign verify

static X509 *getCert(const char* filepath)
{
	FILE *fp = NULL;
	if ((fp = fopen(filepath, "rb")) == NULL) {
        fprintf(stderr, "Error opening file %s\n", filepath);
        exit(1);
    }

    X509 *cert = PEM_read_X509(fp, NULL, NULL, NULL);
	fclose(fp);
	return cert;
}
static EVP_PKEY *getPEMPublicKey(const char* filepath)
{
	EVP_PKEY *pkey = NULL;
	BIO* key = NULL;
	key = BIO_new_file(filepath,"r");
  if (!key) {
      BIO_printf(bio_stdout, "Can't open file %s\n", filepath);
      return NULL;
  }
	pkey = PEM_read_bio_PUBKEY(key, NULL, 0, NULL);
	BIO_free(key);
	return pkey;
}

static EVP_PKEY *getPEMPublicKeyFromBuf(const char* buf,size_t len)
{
	EVP_PKEY *pkey = NULL;
	BIO* key = NULL;
	key = BIO_new(BIO_s_mem());
	BIO_write(key,buf,len);
	BIO_seek(key,0);
	pkey = PEM_read_bio_PUBKEY(key, NULL, 0, NULL);
	BIO_free(key);
	return pkey;
}

static EVP_PKEY *getPEMPrivateKeyFromBuf(const char* buf,size_t len)
{
	EVP_PKEY *pkey = NULL;
	BIO* key = NULL;
	key = BIO_new(BIO_s_mem());
	BIO_write(key,buf,len);
	BIO_seek(key,0);
	pkey = PEM_read_bio_PrivateKey(key, NULL, 0, NULL);
	BIO_free(key);
	return pkey;
}

static EVP_PKEY* getPrivateKey(const char* filepath)
{
	FILE *fp = NULL;
	if ((fp = fopen(filepath, "rb")) == NULL) {
        fprintf(stderr, "Error opening file %s\n", filepath);
        exit(1);
    }

    EVP_PKEY *pkey = PEM_read_PrivateKey(fp, NULL, NULL, NULL);
	fclose(fp);
	return pkey;
}

static int test_sm2_encrypt(EVP_PKEY *pkey, unsigned char *cbuf, size_t buflen,unsigned char * out ,size_t* poutlen)
{
	int ret = 0;

	if(SM2_encrypt_with_recommended(cbuf, buflen, out, poutlen, EVP_PKEY_get0_EC_KEY(pkey)) != 1){
		ERR_print_errors_fp(stderr);
		printf("SM2_encrypt_with_recommended failed \n");
		ret = -1; goto end;
	}
	size_t i;
	printf("ciphertext (%d bytes) = \n", *poutlen);
	for (i = 0; i < *poutlen; i++) {
		printf("%02X ", out[i]);
		if(i % 16 == 15){
			printf("\n");
		}
	}
	printf("\n");


	ret = 1;
end:
	ERR_print_errors_fp(stderr);
	return ret;
}

static int test_sm2_decrypt(EVP_PKEY *pkey, unsigned char *cbuf, size_t cbuflen)
{
	int ret = 0;
	unsigned char mbuf[1024];
	size_t mbuflen = sizeof(mbuf);

	memset(mbuf, 0, sizeof(mbuf));
	if (!SM2_decrypt_with_recommended(cbuf, cbuflen,mbuf, &mbuflen,  EVP_PKEY_get0_EC_KEY(pkey))) {
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
	}

	if ( 1) {
		printf("decrypted message = %s\n", mbuf);
	}

	ret = 1;
end:
	ERR_print_errors_fp(stderr);
	return ret;
}


static int test_evp_pkey_sign(EVP_PKEY *pkey, unsigned char *cbuf, size_t cbuflen,unsigned char * out, size_t *poutlen)
{
	int ret = 0;
	EVP_PKEY_CTX *pkctx = NULL;

	if (!(pkctx = EVP_PKEY_CTX_new(pkey, NULL))) {
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
	}

	if (!EVP_PKEY_sign_init(pkctx)) {
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
	}

	if (!EVP_PKEY_sign(pkctx, out, poutlen, cbuf, cbuflen)) {
		ERR_print_errors_fp(stderr);
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
	}
	size_t i;
	printf("sign data ciphertext (%d bytes) = \n", *poutlen);
	for (i = 0; i < *poutlen; i++) {
		printf("%02X ", out[i]);
		if(i % 16 == 15){
			printf("\n");
		}
	}
	printf("\n");

	ret = 1;
end:
	ERR_print_errors_fp(stderr);
	EVP_PKEY_CTX_free(pkctx);
	return ret;
}


static int test_evp_pkey_verify_sign(EVP_PKEY *pkey,unsigned char* buf,size_t buflen ,unsigned char *signbuf, size_t signlen)
{
	int ret = 0;
	EVP_PKEY_CTX *pkctx = NULL;

	if (!(pkctx = EVP_PKEY_CTX_new(pkey, NULL))) {
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
	}

	if (!EVP_PKEY_verify_init(pkctx)) {
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
	}

	if (!EVP_PKEY_verify(pkctx, signbuf, signlen, buf, buflen)) {
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
	}
	else
	{
		printf("verify sign OK\n");
	}

	ret = 1;
end:
	ERR_print_errors_fp(stderr);
	EVP_PKEY_CTX_free(pkctx);
	return ret;
}




static int test_evp_pkey_sign(EVP_PKEY *pkey, int do_sm2, int verbose)
{
	int ret = 0;
	EVP_PKEY_CTX *pkctx = NULL;
	int type = do_sm2 ? NID_sm_scheme : NID_secg_scheme;
	unsigned char dgst[EVP_MAX_MD_SIZE] = "hello world";
	size_t dgstlen;
	unsigned char sig[256];
	size_t siglen;


	if (!(pkctx = EVP_PKEY_CTX_new(pkey, NULL))) {
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
	}

	/* EVP_PKEY_sign() */

	if (!EVP_PKEY_sign_init(pkctx)) {
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
	}

	if (!EVP_PKEY_CTX_set_ec_sign_type(pkctx, type)) {
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
	}

	dgstlen = 32;
	memset(sig, 0, sizeof(sig));
	siglen = sizeof(sig);
	if (!EVP_PKEY_sign(pkctx, sig, &siglen, dgst, dgstlen)) {
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
	}

	if (verbose > 1) {
		size_t i;
		printf("signature (%zu bytes) = ", siglen);
		for (i = 0; i < siglen; i++) {
			printf("%02X", sig[i]);
		}
		printf("\n");
	}

	if (!EVP_PKEY_verify_init(pkctx)) {
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
	}

	if (!EVP_PKEY_CTX_set_ec_sign_type(pkctx, type)) {
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
	}

	if (1 != EVP_PKEY_verify(pkctx, sig, siglen, dgst, dgstlen)) {
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
	}

	if (verbose) {
		printf("%s(%s) passed\n", __FUNCTION__, OBJ_nid2sn(type));
	}

	ret = 1;
end:
	EVP_PKEY_CTX_free(pkctx);
	return ret;
}


static int test_evp_pkey_encrypt(EVP_PKEY *pkey, int do_sm2, int verbose)
{
	int ret = 0;
	EVP_PKEY_CTX *pkctx = NULL;
	int type = do_sm2 ? NID_sm_scheme : NID_secg_scheme;
	unsigned char msg[] = "hello world this is the message";
	size_t msglen = sizeof(msg);
	unsigned char cbuf[512];
	size_t cbuflen = sizeof(cbuf);
	unsigned char mbuf[512];
	size_t mbuflen = sizeof(mbuf);

	if (!(pkctx = EVP_PKEY_CTX_new(pkey, NULL))) {
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
	}

	/* EVP_PKEY_encrypt() */

	if (!EVP_PKEY_encrypt_init(pkctx)) {
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
	}

	if (!EVP_PKEY_CTX_set_ec_enc_type(pkctx, type)) {
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
	}

					
	/* we need to set the sm2 encrypt params (hash = sm3) */
									

	cbuflen = sizeof(cbuf);
	if (!EVP_PKEY_encrypt(pkctx, cbuf, &cbuflen, msg, msglen)) {
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
	}

	if (verbose > 1) {
		size_t i;
		printf("ciphertext (%zu bytes) = ", cbuflen);
		for (i = 0; i < cbuflen; i++) {
			printf("%02X", cbuf[i]);
		}
		printf("\n");
	}

	if (!EVP_PKEY_decrypt_init(pkctx)) {
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
	}

	if (!EVP_PKEY_CTX_set_ec_enc_type(pkctx, type)) {
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
	}

	memset(mbuf, 0, sizeof(mbuf));
	mbuflen = sizeof(mbuf);
	if (!EVP_PKEY_decrypt(pkctx, mbuf, &mbuflen, cbuf, cbuflen)) {
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
	}

	if (verbose > 1) {
		printf("original  message = %s\n", msg);
		printf("decrypted message = %s\n", mbuf);
	}

	if (verbose) {
		printf("%s(%s) passed\n", __FUNCTION__, OBJ_nid2sn(type));
	}

	ret = 1;
end:
	ERR_print_errors_fp(stderr);
	EVP_PKEY_CTX_free(pkctx);
	return ret;
}


int main(){
	unsigned char buf[]= "1234567812345678";
	unsigned char out[1024]={0};
	size_t outlen = 1024;
	
	unsigned char signdata[1024]={0};
	size_t signlen = 1024;
	
	size_t buflen = strlen((char*)buf);
	BIO *bio_err = BIO_new_fp(stderr, BIO_NOCLOSE | BIO_FP_TEXT);
	BIO *bio_stdout = BIO_new_fp(stdout, BIO_NOCLOSE | BIO_FP_TEXT);
	//EVP_PKEY* privKey = getPrivateKey("/root/cert/privatekey.pem");
	EVP_PKEY* privKey = getPEMPrivateKeyFromBuf(g_prikey,strlen(g_prikey));
	
	//EVP_PKEY* pubKey = getPEMPublicKey("/root/cert/publickey.pem");
	EVP_PKEY* pubKey = getPEMPublicKeyFromBuf(g_pubkey,strlen(g_pubkey));
	
	
	//加密
	test_sm2_encrypt(pubKey, buf, buflen,out,&outlen);
	test_sm2_decrypt(privKey,out, outlen);
	
	
	test_evp_pkey_sign(privKey,1,2);
	
	test_evp_pkey_encrypt(privKey,1,2);
	
	test_evp_pkey_sign(privKey,buf,buflen,signdata,&signlen);
	test_evp_pkey_verify_sign(pubKey,buf,buflen,signdata,signlen);
	
	
	BIO_free(bio_stdout);
	BIO_free(bio_err);
	
	EVP_PKEY_free(privKey);
	EVP_PKEY_free(pubKey);
	
	
	return 0;
}


