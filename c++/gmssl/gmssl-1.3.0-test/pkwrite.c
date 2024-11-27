#include <stdio.h>
#include <stdlib.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/bio.h>
#include <openssl/rand.h>

static int test_evp_pkey_encrypt(EVP_PKEY *pkey, int do_sm2, int verbose, unsigned char cbuf[512], size_t& cbuflen)
{
	int ret = 0;
	EVP_PKEY_CTX *pkctx = NULL;
	//int type = NID_sm2encrypt;
	int type = do_sm2 ? NID_sm_scheme : NID_secg_scheme;
	unsigned char msg[] = "hello world this is the message";
	size_t msglen = sizeof(msg);

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

	if (verbose > 1) {
		printf("original  message = %s\n", msg);
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

static int test_evp_pkey_decrypt(EVP_PKEY *pkey, int do_sm2, int verbose, unsigned char cbuf[512], size_t cbuflen)
{
	int ret = 0;
	EVP_PKEY_CTX *pkctx = NULL;
	//int type = NID_sm2encrypt;
	int type = do_sm2 ? NID_sm_scheme : NID_secg_scheme;
	unsigned char mbuf[512];
	size_t mbuflen = sizeof(mbuf);

	if (!(pkctx = EVP_PKEY_CTX_new(pkey, NULL))) {
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
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
	if (!EVP_PKEY_decrypt(pkctx, mbuf, &mbuflen, cbuf, cbuflen)) {
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
	}

	if (verbose > 1) {
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

static int test_sm2_encrypt(EVP_PKEY *pkey, unsigned char cbuf[512], size_t& cbuflen)
{
	int ret = 0;
	unsigned char msg[] = "hello world this is the message";
	size_t msglen = sizeof(msg);

	SM2_encrypt_with_recommended(cbuf, &cbuflen, msg, msglen, pkey->pkey.ec);

	if (1) {
		size_t i;
		printf("ciphertext (%zu bytes) = ", cbuflen);
		for (i = 0; i < cbuflen; i++) {
			printf("%02X", cbuf[i]);
		}
		printf("\n");
	}

	if (1) {
		printf("original  message = %s\n", msg);
	}

	ret = 1;
end:
	ERR_print_errors_fp(stderr);
	return ret;
}

static int test_sm2_decrypt(EVP_PKEY *pkey, unsigned char cbuf[512], size_t cbuflen)
{
	int ret = 0;
	unsigned char mbuf[512];
	size_t mbuflen = sizeof(mbuf);

	memset(mbuf, 0, sizeof(mbuf));
	if (!SM2_decrypt_with_recommended(mbuf, &mbuflen, cbuf, cbuflen, pkey->pkey.ec)) {
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

static int test_evp_pkey_sign(EVP_PKEY *pkey, unsigned char cbuf[512], size_t& cbuflen)
{
	int ret = 0;
	EVP_PKEY_CTX *pkctx = NULL;
	
	unsigned char msg[] = "hello world this is the message";
	size_t msglen = sizeof(msg);

	if (!(pkctx = EVP_PKEY_CTX_new(pkey, NULL))) {
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
	}

	if (!EVP_PKEY_sign_init(pkctx)) {
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
	}

	if (!EVP_PKEY_sign(pkctx, cbuf, &cbuflen, msg, msglen)) {
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
	}

	if (1) {
		size_t i;
		printf("signtext (%zu bytes) = ", cbuflen);
		for (i = 0; i < cbuflen; i++) {
			printf("%02X", cbuf[i]);
		}
		printf("\n");
	}

	if ( 1) {
		printf("original  message = %s\n", msg);
	}

	ret = 1;
end:
	ERR_print_errors_fp(stderr);
	EVP_PKEY_CTX_free(pkctx);
	return ret;
}

static int test_evp_pkey_verify_sign(EVP_PKEY *pkey, unsigned char cbuf[512], size_t cbuflen)
{
	int ret = 0;
	EVP_PKEY_CTX *pkctx = NULL;
	
	unsigned char msg[] = "hello world this is the message";
	size_t msglen = sizeof(msg);

	if (!(pkctx = EVP_PKEY_CTX_new(pkey, NULL))) {
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
	}

	if (!EVP_PKEY_verify_init(pkctx)) {
		fprintf(stderr, "error: %s %d\n", __FILE__, __LINE__);
		goto end;
	}

	if (!EVP_PKEY_verify(pkctx, cbuf, cbuflen, msg, msglen)) {
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

static void test_rand(int num)
{
	unsigned char buf[1024];
	int chunk = num;
	if (chunk > (int)sizeof(buf))
		chunk = sizeof buf;
	int ret = RAND_bytes(buf, chunk);
	if ( ret <= 0 )
	{
		printf("rand error\n");
	}
	else
	{
		for (int i = 0; i < chunk; i++)
        {
			printf("%02x", buf[i]);
		}
		printf("\n");
	}
}

int main(int argc, char **argv)
{	
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();

    X509 *cert = getCert("/opt/server/servercert.pem");
    EVP_PKEY *pkey = getPrivateKey("/opt/server/skey.pem");
    EVP_PKEY *pubkey = X509_get_pubkey(cert);

	X509 *caCert = getCert("/opt/gw/CA_gaowei_test/cacert.pem");
	EVP_PKEY *caPubkey = X509_get_pubkey(caCert);
	EVP_PKEY *capkey = getPrivateKey("/opt/gw/CA_gaowei_test/private/cakey.pem");
    
  /*  int err = X509_verify(cert, pkey);
    printf("server private key verify, err=%d, %s\n", err, X509_verify_cert_error_string(err));

	err = X509_verify(cert, pubkey);
    printf("server public key verify, err=%d, %s\n", err, X509_verify_cert_error_string(err));

	err = X509_verify(cert, capkey);
    printf("ca private key verify, err=%d, %s\n", err, X509_verify_cert_error_string(err));

    err = X509_verify(cert, caPubkey);
    printf("ca public key verify, err=%d, %s\n", err, X509_verify_cert_error_string(err));
*/
	{
		// 没效果，写不到文件中
		BIO* bp = BIO_new_file("/opt/server/public2.key", "wb");
		i2b_PublicKey_bio(bp, pkey);
		BIO_free(bp);
	}

	{
		BIO* bp = BIO_new_file("/opt/server/public3.key", "wb");
		i2b_PublicKey_bio(bp, pubkey);
		BIO_free(bp);
	}
	
	/*{
		// 打印出公钥、私钥信息
		BIO* bp = BIO_new_fp(stdout, BIO_NOCLOSE);
		EVP_PKEY_print_public(bp, pkey, 4, NULL);
		EVP_PKEY_print_private(bp, pkey, 4, NULL);
		EVP_PKEY_print_public(bp, pubkey, 4, NULL);
		EVP_PKEY_print_public(bp, capkey, 4, NULL);
		EVP_PKEY_print_private(bp, capkey, 4, NULL);
		EVP_PKEY_print_public(bp, caPubkey, 4, NULL);
		BIO_free(bp);
	}*/

	unsigned char cbuf[512] = {0};
	size_t cbuflen = sizeof(cbuf);

	// 使用EVP方式只能采取公钥加密，私钥解密的方式，反之运行会出错
	printf("\n========================================\n");
	printf("invoke evp, public key encrypt, private key decrypt\n");
	cbuflen = sizeof(cbuf);
    test_evp_pkey_encrypt(pubkey, 1, 2, cbuf, cbuflen);
    test_evp_pkey_decrypt(pkey, 1, 2, cbuf, cbuflen);
	
	printf("\n========================================\n");
	printf("invoke sm2, public key encrypt, private key decrypt\n");
	cbuflen = sizeof(cbuf);
    test_sm2_encrypt(pubkey, cbuf, cbuflen);
    test_sm2_decrypt(pkey, cbuf, cbuflen);
	
	// 无法用私钥加密，公钥解密
/*	printf("\n========================================\n");
	printf("invoke sm2, private key encrypt, public key decrypt\n");
	cbuflen = sizeof(cbuf);
    test_sm2_encrypt(pkey, cbuf, cbuflen);
    test_sm2_decrypt(pubkey, cbuf, cbuflen);
*/
	// 签名测试
	printf("\n========================================\n");	
	printf("private key sign, public key verify\n");
	cbuflen = sizeof(cbuf);
	test_evp_pkey_sign(pkey, cbuf, cbuflen);
	test_evp_pkey_verify_sign(pubkey, cbuf, cbuflen);
	// 其他公钥，验证失败
	test_evp_pkey_verify_sign(caPubkey, cbuf, cbuflen);
	
	// 公钥签名，私钥验证，会报错
/*	printf("\n========================================\n");	
	printf("public key sign, private key verify\n");
	cbuflen = sizeof(cbuf);
	test_evp_pkey_sign(pubkey, cbuf, cbuflen);
	test_evp_pkey_verify_sign(pkey, cbuf, cbuflen);
*/

	// 生成随机数
	printf("\n========================================\n");	
	for (int i = 0; i < 2; ++i)
	{
		printf("rand %d\n", i+1);
		test_rand(i+5);
	}
	RAND_write_file("/opt/rand");

    EVP_PKEY_free(pkey);
    EVP_PKEY_free(pubkey);
    EVP_PKEY_free(capkey);
    EVP_PKEY_free(caPubkey);
	X509_free(cert);
	X509_free(caCert);

    return 0;
}
