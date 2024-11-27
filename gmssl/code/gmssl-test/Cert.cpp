#include "Cert.h"

CCert::CCert(void)
{
	m_pub_key = NULL;
	m_pri_key = NULL;
	ERR_load_crypto_strings();
	OpenSSL_add_all_ciphers();
	OpenSSL_add_all_algorithms();

}

CCert::~CCert(void)
{
	if(m_pub_key != NULL){ EVP_PKEY_free(m_pub_key);m_pub_key = NULL;};  
	if(m_pri_key != NULL){EVP_PKEY_free(m_pri_key); m_pri_key = NULL;};
	EVP_cleanup();

	/* if you omit the next, a small leak may be left when you make use of the BIO (low level API) for e.g. base64 transformations */
	CRYPTO_cleanup_all_ex_data();

}
int CCert::load_public_cert(const char *certfile)  
{  
	if(m_pub_key != NULL){ EVP_PKEY_free(m_pub_key);m_pub_key = NULL;};  
    BIO *b = BIO_new_file(certfile, "r");
	if (b == NULL) { SHOW("%s\n", ERR_error_string(ERR_get_error(), NULL));  return -1; }
	X509 *x = PEM_read_bio_X509(b, NULL, NULL, NULL);
	if (x == NULL) { SHOW("%s\n", ERR_error_string(ERR_get_error(), NULL)); BIO_free(b);  return -1; }
	EVP_PKEY *pub_key = X509_get_pubkey(x);
	if (pub_key == NULL) { SHOW("%s\n", ERR_error_string(ERR_get_error(), NULL)); BIO_free(b);  return -1; }
	m_pub_key = pub_key;

	BIO_free(b);  
    return 0;
}  
int CCert::load_private_key(const char *keyfile, const  char *passwd) {  
	
	if(m_pri_key != NULL){EVP_PKEY_free(m_pri_key); m_pri_key = NULL;};
    EVP_PKEY* key = NULL;  
    RSA *rsa = RSA_new();  
    
    BIO *bp = NULL;  
    bp = BIO_new_file(keyfile, "rb");   
    if(NULL == bp)  
    {  
        SHOW("open_private_key bio file new error!\n");  
		RSA_free(rsa);  
        return -1;  
    }  
  
    rsa = PEM_read_bio_RSAPrivateKey(bp, &rsa, NULL, (void *)passwd);  
    if(rsa == NULL)  
    {  
        SHOW("open_private_key failed to PEM_read_bio_RSAPrivateKey!\n");  
        BIO_free(bp);  
        RSA_free(rsa);  
  
        return -1;  
    }  
  
    SHOW("open_private_key success to PEM_read_bio_RSAPrivateKey!\n");  

	
    key = EVP_PKEY_new();  
    if(NULL == key)  
    {  
        SHOW("open_private_key EVP_PKEY_new failed\n");  
		BIO_free(bp); 
        RSA_free(rsa); 
  
        return -1;  
    }  
  
    EVP_PKEY_assign_RSA(key, rsa);
	// TODO 是否需要释放 rsa
	m_pri_key = key;
    return 0;  
}

int CCert::getFilesm3(char * filenpath,unsigned char* filehash,unsigned int* md_len){
	const EVP_MD *md;
	EVP_MD_CTX *mdctx;

	char buf[512*2]={0};
	unsigned int  len=0;
	FILE * fp=fopen(filenpath,"rb");
	if(fp == NULL){
		return -1;
	}
	md = EVP_get_digestbyname("sm3");
	if(!md) {
		fclose(fp);
		return -1;
    }
    mdctx = EVP_MD_CTX_new(); //allocates, initializes and returns a digest context.
    EVP_DigestInit_ex(mdctx, md, NULL);  //sets up digest context ctx to use a digest type from ENGINE impl.
	size_t nread=0;
	while( nread = fread(buf,sizeof(char),sizeof(buf),fp))  
    {  
        EVP_DigestUpdate(mdctx, buf, nread); //hashes cnt bytes of data at d into the digest context ctx. 
    }  
    EVP_DigestFinal_ex(mdctx, filehash, (unsigned int *)&len);//retrieves the digest value from ctx and places it in md. 
	*md_len=len;
    EVP_MD_CTX_free(mdctx);
	fclose(fp);
	return 0;
}
int CCert::sm4_do_crypt(char *filein, char *fileout,unsigned char *key,int do_encrypt){
	/* Allow enough space in output buffer for additional block */
	unsigned char inbuf[1024], outbuf[1024 + EVP_MAX_BLOCK_LENGTH];
	int inlen, outlen;
	EVP_CIPHER_CTX *ctx;
	unsigned char iv[] = "1234567887654321";   // 31323334353637383837363534333231
	
	/* Bogus key and IV: we'd normally set these from
	* another source.
	*/
	//unsigned char key[] = "0123456789abcdeF";  // key  30313233343536373839616263646546
	//unsigned char key[16];
	//memcpy(key,mkey,sizeof(key));
	FILE *in=fopen(filein,"rb");
	if(in == NULL){
		return -1;
	}
	FILE *out=fopen(fileout,"wb+");
	if(out == NULL){
		fclose(in);
		return -1;
	}


	
	/* Don't set key or IV right away; we want to check lengths */
	ctx = EVP_CIPHER_CTX_new();  //creates a cipher context.
	EVP_CipherInit_ex(ctx, EVP_sms4_cbc(), NULL, NULL, NULL,
		   do_encrypt);
	OPENSSL_assert(EVP_CIPHER_CTX_key_length(ctx) == 16);
	OPENSSL_assert(EVP_CIPHER_CTX_iv_length(ctx) == 16);

	/* Now we can set key and IV */
	EVP_CipherInit_ex(ctx, NULL, NULL, key, iv, do_encrypt);

	for(;;)
   {
	   inlen = fread(inbuf, 1, 16, in);
	   if (inlen <= 0) break;
	   if(!EVP_CipherUpdate(ctx, outbuf, &outlen, inbuf, inlen))
	   {
			 /* Error */
			EVP_CIPHER_CTX_free(ctx);
			fclose(in);
			fclose(out);
			return -1;
	   }
	   fwrite(outbuf, 1, outlen, out);
   }
	if(!EVP_CipherFinal_ex(ctx, outbuf, &outlen))
   {
	   /* Error */
	   EVP_CIPHER_CTX_free(ctx);
	   fclose(in);
	   fclose(out);
	   return -1;
   }
	fwrite(outbuf, 1, outlen, out);

	EVP_CIPHER_CTX_free(ctx);
	fclose(in);
	fclose(out);
	return 0;
}

int CCert::rsa_key_encrypt(const unsigned char *orig_data, size_t orig_data_len,   
					unsigned char *enc_data, size_t &enc_data_len){
    EVP_PKEY_CTX *ctx = NULL;   
	 
	EVP_PKEY *key = m_pub_key;
    ctx = EVP_PKEY_CTX_new(key, NULL);  
    if(NULL == ctx)  
    {  
        SHOW("ras_pubkey_encryptfailed to open ctx.\n");  
        //EVP_PKEY_free(key);  
        return -1;  
    }  
  
    if(EVP_PKEY_encrypt_init(ctx) <= 0)    // initializes a public key algorithm context using key pkey for an encryption operation.
    {  
        SHOW("ras_pubkey_encryptfailed to EVP_PKEY_encrypt_init.\n");  
        //EVP_PKEY_free(key);  
        return -1;  
    }  
	
    if(EVP_PKEY_encrypt(ctx,  
        enc_data,  
        &enc_data_len,  
        orig_data,  
        orig_data_len) <= 0)  // performs a public key encryption operation using ctx
    {  
		SHOW("%s\n", ERR_error_string(ERR_get_error(), NULL));
        EVP_PKEY_CTX_free(ctx);  
       // EVP_PKEY_free(key);  
  
        return -1;  
    }  
  
    EVP_PKEY_CTX_free(ctx);  
    //EVP_PKEY_free(key);  
  
    return 0;  
}

int CCert::rsa_key_decrypt(const unsigned char *enc_data, size_t enc_data_len,   
					   unsigned char *orig_data, size_t &orig_data_len, const  char *passwd){
    EVP_PKEY_CTX *ctx = NULL;   
	EVP_PKEY *key = m_pri_key;

    ctx = EVP_PKEY_CTX_new(key, NULL);  
    if(NULL == ctx)  
    {  
        SHOW("ras_prikey_decryptfailed to open ctx.\n");  
        //EVP_PKEY_free(key);  
        return -1;  
    }  
  
    if(EVP_PKEY_decrypt_init(ctx) <= 0)  
    {  
        SHOW("ras_prikey_decryptfailed to EVP_PKEY_decrypt_init.\n");  
        //EVP_PKEY_free(key);  
        return -1;  
    }  
	
    if(EVP_PKEY_decrypt(ctx,  
        orig_data,  
        &orig_data_len,  
        enc_data,  
        enc_data_len) <= 0)   // performs a public key decryption operation using ctx. 
    {  
        SHOW("ras_prikey_decryptfailed to EVP_PKEY_decrypt.\n");  
        EVP_PKEY_CTX_free(ctx);  
        //EVP_PKEY_free(key);  
  
        return -1;  
    }  
  
    EVP_PKEY_CTX_free(ctx);  
    
    return 0;  
}
