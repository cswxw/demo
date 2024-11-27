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



#include <../rsa/rsa_locl.h>


#include <../evp/evp_locl.h>
#include <internal/evp_int.h>
#include <openssl/evp.h>
#include <../crypto/ec/ec_lcl.h>

#include <openssl/ec.h>
#include <windows.h>
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

#define ERR(format,...) fprintf(stdout , "[err ]" __FUNCTION__ " : " format,__VA_ARGS__); 



#define zsys_debug(...) \
	printf(__VA_ARGS__); \
	printf("\r\n");

void bn_hex_printf(BIGNUM * a)  ;
void bn_printf2(BIGNUM * a)  ;



#ifdef __cplusplus
}
#endif 




# define B_FORMAT_TEXT   0x8000
# define FORMAT_PEM     (5 | B_FORMAT_TEXT)
# define FORMAT_ASN1     4                      /* ASN.1/DER */
# define FORMAT_TEXT    (1 | B_FORMAT_TEXT)     /* Generic text */
BIO *bio_err = NULL;

static void genkey(){
	char * curve_name = "sm2p256v1";
	BIO *out = NULL;
	int i;
	int nid;
	int outformat = FORMAT_PEM;
	int text = 1; //-text
	int noout = 0; //-noout
	int asn1_flag = OPENSSL_EC_NAMED_CURVE;

	EC_GROUP *group = NULL;

	point_conversion_form_t form = POINT_CONVERSION_UNCOMPRESSED;
	
	nid = OBJ_sn2nid(curve_name);
	out = BIO_new_file("123.key","w");

	group = EC_GROUP_new_by_curve_name(nid);
	if (group == NULL) {
		BIO_printf(bio_err, "unable to create curve (%s)\n", curve_name);
		goto end;
	}
	EC_GROUP_set_asn1_flag(group, asn1_flag);
	EC_GROUP_set_point_conversion_form(group, form);
	if (group == NULL) {
		BIO_printf(bio_err, "unable to load elliptic curve parameters\n");
		ERR_print_errors(bio_err);
		goto end;
	}
	//参数-text
	if (text) {
		if (!ECPKParameters_print(out, group, 0))
			goto end;
	}

	if (!noout) {
		if (outformat == FORMAT_ASN1){
			//i = i2d_ECPKParameters_bio(out, group);
		}else
			i = PEM_write_bio_ECPKParameters(out, group);
		if (!i) {
			BIO_printf(bio_err, "unable to write elliptic "
					   "curve parameters\n");
			ERR_print_errors(bio_err);
			goto end;
		}
	}

	if (1) {
		EC_KEY *eckey = EC_KEY_new();

		if (eckey == NULL)
			goto end;

		

		if (EC_KEY_set_group(eckey, group) == 0) {
			BIO_printf(bio_err, "unable to set group when generating key\n");
			EC_KEY_free(eckey);
			ERR_print_errors(bio_err);
			goto end;
		}

		if (!EC_KEY_generate_key(eckey)) {
			BIO_printf(bio_err, "unable to generate key\n");
			EC_KEY_free(eckey);
			ERR_print_errors(bio_err);
			goto end;
		}
		
		if (outformat == FORMAT_ASN1)
			i = i2d_ECPrivateKey_bio(out, eckey);
		else
			i = PEM_write_bio_ECPrivateKey(out, eckey, NULL,
										   NULL, 0, NULL, NULL);
		EC_KEY_free(eckey);
	}
end:
	BIO_free(out);
	return;
}
void print_name(BIO *out, const char *title, X509_NAME *nm,
                unsigned long lflags)
{
    char *buf;
    char mline = 0;
    int indent = 0;

    if (title)
        BIO_puts(out, title);
    if ((lflags & XN_FLAG_SEP_MASK) == XN_FLAG_SEP_MULTILINE) {
        mline = 1;
        indent = 4;
    }
    if (lflags == XN_FLAG_COMPAT) {
        buf = X509_NAME_oneline(nm, 0, 0);
        BIO_puts(out, buf);
        BIO_puts(out, "\n");
        OPENSSL_free(buf);
    } else {
        if (mline)
            BIO_puts(out, "\n");
        X509_NAME_print_ex(out, nm, indent, lflags);
        BIO_puts(out, "\n");
    }
}

static int callb(int ok, X509_STORE_CTX *ctx)
{
    int err;
    X509 *err_cert;

    /*
     * it is ok to use a self signed certificate This case will catch both
     * the initial ok == 0 and the final ok == 1 calls to this function
     */
    err = X509_STORE_CTX_get_error(ctx);
    if (err == X509_V_ERR_DEPTH_ZERO_SELF_SIGNED_CERT)
        return 1;

    /*
     * BAD we should have gotten an error.  Normally if everything worked
     * X509_STORE_CTX_get_error(ctx) will still be set to
     * DEPTH_ZERO_SELF_....
     */
    if (ok) {
        BIO_printf(bio_err,
                   "error with certificate to be certified - should be self signed\n");
        return 0;
    } else {
        err_cert = X509_STORE_CTX_get_current_cert(ctx);
        print_name(bio_err, NULL, X509_get_subject_name(err_cert), 0);
        BIO_printf(bio_err,
                   "error with certificate - error %d at depth %d\n%s\n", err,
                   X509_STORE_CTX_get_error_depth(ctx),
                   X509_verify_cert_error_string(err));
        return 1;
    }
}
# define SERIAL_RAND_BITS        64
int rand_serial(BIGNUM *b, ASN1_INTEGER *ai)
{
    BIGNUM *btmp;
    int ret = 0;

    if (b)
        btmp = b;
    else
        btmp = BN_new();

    if (btmp == NULL)
        return 0;

    if (!BN_pseudo_rand(btmp, SERIAL_RAND_BITS, 0, 0))
        goto error;
    if (ai && !BN_to_ASN1_INTEGER(btmp, ai))
        goto error;

    ret = 1;

 error:

    if (btmp != b)
        BN_free(btmp);

    return ret;
}


int set_cert_times(X509 *x, const char *startdate, const char *enddate,
                   int days)
{
    if (startdate == NULL || strcmp(startdate, "today") == 0) {
        if (X509_gmtime_adj(X509_getm_notBefore(x), 0) == NULL)
            return 0;
    } else {
        if (!ASN1_TIME_set_string(X509_getm_notBefore(x), startdate))
            return 0;
    }
    if (enddate == NULL) {
        if (X509_time_adj_ex(X509_getm_notAfter(x), days, 0, NULL)
            == NULL)
            return 0;
    } else if (!ASN1_TIME_set_string(X509_getm_notAfter(x), enddate)) {
        return 0;
    }
    return 1;
}


EVP_PKEY* load_private_key(const char *keyfile, const  char *passwd) {  
	
	EVP_PKEY *pkey = NULL;
    EC_KEY* eckey = NULL;  

    BIO *bp = NULL;  
    bp = BIO_new_file(keyfile, "rb");   
    if(NULL == bp)  
    {  
        printf("open_private_key bio file new error!\n");  
	
        return pkey;
    }  
  
	pkey = PEM_read_bio_PrivateKey(bp, NULL,NULL,NULL);
#if 0
    eckey = PEM_read_bio_ECPrivateKey(bp, NULL, NULL,NULL);  
    if(eckey == NULL)  
    {  
        printf("open_private_key failed to PEM_read_bio_RSAPrivateKey!\n");  

        BIO_free(bp);  
        return eckey;
    }
#endif
 	//if (eckey)
	//	EC_KEY_free(eckey);
	BIO_free(bp);
    return pkey;  
}


/* self sign */
static int sign(X509 *x, EVP_PKEY *pkey, int days, int clrext,
                const EVP_MD *digest, CONF *conf, const char *section)
{

    if (!X509_set_issuer_name(x, X509_get_subject_name(x)))
        goto err;
    if (!set_cert_times(x, NULL, NULL, days))
        goto err;
    if (!X509_set_pubkey(x, pkey))
        goto err;
    if (clrext) {
        while (X509_get_ext_count(x) > 0)
            X509_delete_ext(x, 0);
    }
    if (conf) {
#if 0
        X509V3_CTX ctx;
        X509_set_version(x, 2); /* version 3 certificate */
        X509V3_set_ctx(&ctx, x, x, NULL, NULL, 0);
        X509V3_set_nconf(&ctx, conf);
        if (!X509V3_EXT_add_nconf(conf, &ctx, section, x))
            goto err;
#endif
    }
    if (!X509_sign(x, pkey, digest))
        goto err;
    return 1;
 err:
    ERR_print_errors(bio_err);
    return 0;
}

void rootCert(){
	X509_STORE *ctx = NULL;
	BIO* out = NULL , *in=NULL;
	int i=0;
	
	ASN1_INTEGER *sno = NULL;


	EVP_PKEY  *fkey = NULL,*Upkey = NULL;

	X509_REQ *req = NULL;
	
	X509 *x = NULL, *xca = NULL;

	int days = 30;
	
	const EVP_MD *digest = NULL;

	unsigned long nmflag = 0;

	EVP_PKEY *pkey;

	char * keyfile = "123.key";

	

	//从req中获取公钥数据
	in = BIO_new_file("123.req","r");
	req = PEM_read_bio_X509_REQ(in, NULL, NULL, NULL);
	 BIO_free(in);
    if (req == NULL) {
        ERR_print_errors(bio_err);
        goto end;
    }
    if ((pkey = X509_REQ_get0_pubkey(req)) == NULL) {
        BIO_printf(bio_err, "error unpacking public key\n");
        goto end;
    }
	//验证请求签名
	i = X509_REQ_verify(req, pkey);
	if (i < 0) {
		BIO_printf(bio_err, "Signature verification error\n");
		ERR_print_errors(bio_err);
		goto end;
	}
    if (i == 0) {
        BIO_printf(bio_err,
                   "Signature did not match the certificate request\n");
        goto end;
    } else
        BIO_printf(bio_err, "Signature ok\n");
	
	nmflag = 0x0082031f;///??
    print_name(bio_err, "subject=", X509_REQ_get_subject_name(req),
               nmflag);

	ctx = X509_STORE_new();
    if (ctx == NULL)
        goto end;
	 X509_STORE_set_verify_cb(ctx, callb);

    if (!X509_STORE_set_default_paths(ctx)) {
        ERR_print_errors(bio_err);
        goto end;
    }

	//生成证书结构
    if ((x = X509_new()) == NULL)
        goto end;
	
	//序列号
    if (sno == NULL) {
        sno = ASN1_INTEGER_new();
        if (sno == NULL || !rand_serial(NULL, sno))
            goto end;
        if (!X509_set_serialNumber(x, sno))
            goto end;
        ASN1_INTEGER_free(sno);
        sno = NULL;
    } else if (!X509_set_serialNumber(x, sno))
        goto end;
	
	//颁发者
    if (!X509_set_issuer_name(x, X509_REQ_get_subject_name(req)))
        goto end;
	//证书subject
    if (!X509_set_subject_name(x, X509_REQ_get_subject_name(req)))
        goto end;
	//过期时间
    if (!set_cert_times(x, NULL, NULL, days))
        goto end;

    if (fkey)
        X509_set_pubkey(x, fkey);
    else {
        pkey = X509_REQ_get0_pubkey(req);
        X509_set_pubkey(x, pkey);
    }


	 OBJ_create("2.99999.3", "SET.ex3", "SET x509v3 extension 3");
	 if(0)
	 {
		 char *alias = "this is alias";
		X509_alias_set1(x, (unsigned char *)alias, -1);
	 }
	

	//加载root私钥
    if (Upkey == NULL) {
        Upkey = load_private_key(keyfile, NULL);
        if (Upkey == NULL)
            goto end;
	}
	digest = EVP_get_digestbyname("sm3");
	if (!sign(x, Upkey, days, NULL, digest, NULL, NULL))
		goto end;
	
	out = BIO_new_file("123.crt","w");
	i = PEM_write_bio_X509(out, x);
    if (!i) {
        BIO_printf(bio_err, "unable to write certificate\n");
        ERR_print_errors(bio_err);
        goto end;
    }
end:
	BIO_free(out);
	//TODO free

	 return;

}

int main(){
	ERR_load_crypto_strings();
	OpenSSL_add_all_ciphers();
	OpenSSL_add_all_algorithms();
    bio_err = BIO_new_fp(stderr, BIO_NOCLOSE | BIO_FP_TEXT);

	//genkey();

	rootCert();
	return 0;
}



