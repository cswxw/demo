#include "main.h"


//测试sm3摘要算法
int sm3Test(){
    EVP_MD_CTX *mdctx;
    const EVP_MD *md;
    char mess1[] = "abc";
    char mess2[] = "HelloWorld";
    unsigned char md_value[EVP_MAX_MD_SIZE];
    int md_len, i;

    md = EVP_get_digestbyname("sm3");

    if(!md) {
           return -1;
    }

    mdctx = EVP_MD_CTX_new(); //allocates, initializes and returns a digest context.
    EVP_DigestInit_ex(mdctx, md, NULL);  //sets up digest context ctx to use a digest type from ENGINE impl. 
    EVP_DigestUpdate(mdctx, mess1, strlen(mess1)); //hashes cnt bytes of data at d into the digest context ctx. 
    EVP_DigestUpdate(mdctx, mess2, strlen(mess2)); 
    EVP_DigestFinal_ex(mdctx, md_value, (unsigned int *)&md_len);//retrieves the digest value from ctx and places it in md. 
    EVP_MD_CTX_free(mdctx);

    printf("Digest is: ");
    for (i = 0; i < md_len; i++)
           printf("%02x", md_value[i]);
    printf("\n");

    return 0;
}

//sms4 do_crypt
int do_crypt(FILE *in, FILE *out, int do_encrypt)
{
	/* Allow enough space in output buffer for additional block */
	unsigned char inbuf[1024], outbuf[1024 + EVP_MAX_BLOCK_LENGTH];
	int inlen, outlen;
	EVP_CIPHER_CTX *ctx;
	/* Bogus key and IV: we'd normally set these from
	* another source.
	*/
	unsigned char key[] = "0123456789abcdeF";  //自定义加密key  30313233343536373839616263646546
	unsigned char iv[] = "1234567887654321";   //加密的初始话矢量，初始化加密函数的变量。 31323334353637383837363534333231

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
	   inlen = fread(inbuf, 1, 1024, in);
	   if (inlen <= 0) break;
	   if(!EVP_CipherUpdate(ctx, outbuf, &outlen, inbuf, inlen))
	   {
			 /* Error */
			EVP_CIPHER_CTX_free(ctx);
			return 0;
	   }
	   fwrite(outbuf, 1, outlen, out);
   }
	if(!EVP_CipherFinal_ex(ctx, outbuf, &outlen))
   {
	   /* Error */
	   EVP_CIPHER_CTX_free(ctx);
	   return 0;
   }
	fwrite(outbuf, 1, outlen, out);

	EVP_CIPHER_CTX_free(ctx);
	return 1;
}
//测试sm4对称算法
void sms4Test(){
	FILE * in = fopen("D:\\systemp\\1.txt","r");
	FILE * out = fopen("D:\\systemp\\1.txt.encode","w+");
	FILE * out_de = fopen("D:\\systemp\\1.txt.decode","w");
	do_crypt(in,out,1); //加密

	//重置指针
	fseek(in,0,SEEK_SET);
	fseek(out,0,SEEK_SET);
	fseek(out_de,0,SEEK_SET);

	do_crypt(out,out_de,0); //解密
	fclose(in);
	fclose(out);
	fclose(out_de);
}
// 生成公钥文件和私钥文件，私钥文件带密码  
int generate_key_files(const char *pub_keyfile, const char *pri_keyfile,   
                       const  char *passwd, int passwd_len)  
{  
    RSA *rsa = NULL;  
	char rnd_seed[3]="ab";
    RAND_seed(rnd_seed, sizeof(rnd_seed));  
    rsa = RSA_generate_key(1024, RSA_F4, NULL, NULL);  
    if(rsa == NULL)  
    {  
        printf("RSA_generate_key error!\n");  
        return -1;  
    }  
  
    // 开始生成公钥文件  
    BIO *bp = BIO_new(BIO_s_file());  
    if(NULL == bp)  
    {  
        printf("generate_key bio file new error!\n");  
        return -1;  
    }  
  
    if(BIO_write_filename(bp, (void *)pub_keyfile) <= 0)  // set the file BIO b to use file name for writing  
    {  
        printf("BIO_write_filename error!\n");  
        return -1;  
    }  
  
    if(PEM_write_bio_RSAPublicKey(bp, rsa) != 1)  //int PEM_write_bio_RSAPublicKey(BIO *bp, RSA *x);
    {  
        printf("PEM_write_bio_RSAPublicKey error!\n");  
        return -1;  
    }  
      
    // 公钥文件生成成功，释放资源  
    printf("Create public key ok!\n");  
    BIO_free_all(bp);  //frees up an entire BIO chain, it does not halt if an error occurs freeing up an individual BIO in the chain. If a is NULL nothing is done.
  
    // 生成私钥文件  
    bp = BIO_new_file(pri_keyfile, "w+");  
        if(NULL == bp)  
    {  
        printf("generate_key bio file new error2!\n");  
        return -1;  
    }  
// int PEM_write_bio_RSAPrivateKey(BIO *bp, RSA *x, const EVP_CIPHER *enc,
//                               unsigned char *kstr, int klen,
//                               pem_password_cb *cb, void *u);  
    if(PEM_write_bio_RSAPrivateKey(bp, rsa,  
        EVP_des_ede3_ofb(), (unsigned char *)passwd,   
        passwd_len, NULL, NULL) != 1)  
    {  
        printf("PEM_write_bio_RSAPublicKey error!\n");  
        return -1;  
    }  
  
    // 释放资源  
    printf("Create private key ok!\n");  
    BIO_free_all(bp);  
    RSA_free(rsa);  
  
    return 0;  
}
// 打开公钥文件，返回EVP_PKEY结构的指针  
EVP_PKEY* open_public_key(const char *keyfile)  
{  
    EVP_PKEY* key = NULL;  
    RSA *rsa = NULL;  
  
    OpenSSL_add_all_algorithms();  
    BIO *bp = BIO_new(BIO_s_file());  //BIO_s_file() returns the BIO file method.
    BIO_read_filename(bp, keyfile);  
    if(NULL == bp)  
    {  
        printf("open_public_key bio file new error!\n");  
        return NULL;  
    }  

// RSA *PEM_read_bio_RSAPublicKey(BIO *bp, RSA **x,
//                                pem_password_cb *cb, void *u);
    rsa = PEM_read_bio_RSAPublicKey(bp, NULL, NULL, NULL);  
    if(rsa == NULL)  
    {  
        printf("open_public_key failed to PEM_read_bio_RSAPublicKey!\n");  
        BIO_free(bp);  
        RSA_free(rsa);  
  
        return NULL;  
    }  
  
    printf("open_public_key success to PEM_read_bio_RSAPublicKey!\n");  
    key = EVP_PKEY_new(); //allocates an empty EVP_PKEY structure which is used by OpenSSL to store private keys. 
    if(NULL == key)  
    {  
        printf("open_public_key EVP_PKEY_new failed\n");  
        RSA_free(rsa);  
  
        return NULL;  
    }  
  
    EVP_PKEY_assign_RSA(key, rsa);  
    return key;  
}  
// 打开私钥文件，返回EVP_PKEY结构的指针  
EVP_PKEY* open_private_key(const char *keyfile, const  char *passwd)  
{  
    EVP_PKEY* key = NULL;  
    RSA *rsa = RSA_new();  
    OpenSSL_add_all_algorithms();  
    BIO *bp = NULL;  
    bp = BIO_new_file(keyfile, "rb");   
    if(NULL == bp)  
    {  
        printf("open_private_key bio file new error!\n");  
  
        return NULL;  
    }  
  
    rsa = PEM_read_bio_RSAPrivateKey(bp, &rsa, NULL, (void *)passwd);  
    if(rsa == NULL)  
    {  
        printf("open_private_key failed to PEM_read_bio_RSAPrivateKey!\n");  
        BIO_free(bp);  
        RSA_free(rsa);  
  
        return NULL;  
    }  
  
    printf("open_private_key success to PEM_read_bio_RSAPrivateKey!\n");  
    key = EVP_PKEY_new();  
    if(NULL == key)  
    {  
        printf("open_private_key EVP_PKEY_new failed\n");  
        RSA_free(rsa);  
  
        return NULL;  
    }  
  
    EVP_PKEY_assign_RSA(key, rsa);  
    return key;  
}
// 使用密钥加密，这种封装格式只适用公钥加密，私钥解密，这里key必须是公钥  
int rsa_key_encrypt(EVP_PKEY *key, const unsigned char *orig_data, size_t orig_data_len,   
                    unsigned char *enc_data, size_t &enc_data_len)  
{  
    EVP_PKEY_CTX *ctx = NULL;  
    OpenSSL_add_all_ciphers();  
  
    ctx = EVP_PKEY_CTX_new(key, NULL);  
    if(NULL == ctx)  
    {  
        printf("ras_pubkey_encryptfailed to open ctx.\n");  
        EVP_PKEY_free(key);  
        return -1;  
    }  
  
    if(EVP_PKEY_encrypt_init(ctx) <= 0)    // initializes a public key algorithm context using key pkey for an encryption operation.
    {  
        printf("ras_pubkey_encryptfailed to EVP_PKEY_encrypt_init.\n");  
        EVP_PKEY_free(key);  
        return -1;  
    }  
	
    if(EVP_PKEY_encrypt(ctx,  
        enc_data,  
        &enc_data_len,  
        orig_data,  
        orig_data_len) <= 0)  // performs a public key encryption operation using ctx
    {  
        printf("ras_pubkey_encryptfailed to EVP_PKEY_encrypt.\n");  
        EVP_PKEY_CTX_free(ctx);  
        EVP_PKEY_free(key);  
  
        return -1;  
    }  
  
    EVP_PKEY_CTX_free(ctx);  
    EVP_PKEY_free(key);  
  
    return 0;  
} 
// 使用密钥解密，这种封装格式只适用公钥加密，私钥解密，这里key必须是私钥  
int rsa_key_decrypt(EVP_PKEY *key, const unsigned char *enc_data, size_t enc_data_len,   
                    unsigned char *orig_data, size_t &orig_data_len, const  char *passwd)  
{  
    EVP_PKEY_CTX *ctx = NULL;  
    OpenSSL_add_all_ciphers();  
  
    ctx = EVP_PKEY_CTX_new(key, NULL);  
    if(NULL == ctx)  
    {  
        printf("ras_prikey_decryptfailed to open ctx.\n");  
        EVP_PKEY_free(key);  
        return -1;  
    }  
  
    if(EVP_PKEY_decrypt_init(ctx) <= 0)  
    {  
        printf("ras_prikey_decryptfailed to EVP_PKEY_decrypt_init.\n");  
        EVP_PKEY_free(key);  
        return -1;  
    }  
  
    if(EVP_PKEY_decrypt(ctx,  
        orig_data,  
        &orig_data_len,  
        enc_data,  
        enc_data_len) <= 0)   // performs a public key decryption operation using ctx. 
    {  
        printf("ras_prikey_decryptfailed to EVP_PKEY_decrypt.\n");  
        EVP_PKEY_CTX_free(ctx);  
        EVP_PKEY_free(key);  
  
        return -1;  
    }  
  
    EVP_PKEY_CTX_free(ctx);  
    EVP_PKEY_free(key);  
    return 0;  
}

//测试公钥和私钥加解密
int rsaTest()  
{  
    char origin_text[] = "hello world!";  
    char enc_text[512] = "";  
    char dec_text[512] = "";  
    size_t enc_len = 512;  
    size_t dec_len = 512;  
  
    // 生成公钥和私钥文件  
    //generate_key_files("D:\\systemp\\gen\\publickey.pem","D:\\systemp\\gen\\privatekey.pem","123",3);  
  

	//pem
    //EVP_PKEY *pub_key = open_public_key("D:\\systemp\\gen\\publickey.pem");  
	//cert public

	BIO *b = BIO_new_file("D:\\systemp\\cert\\cert.crt", "r");
	if (b == NULL) { printf("%s\n", ERR_error_string(ERR_get_error(), NULL));  return -1; }
	X509 *x = PEM_read_bio_X509(b, NULL, NULL, NULL);
	if (x == NULL) { printf("%s\n", ERR_error_string(ERR_get_error(), NULL));  return -1; }
	EVP_PKEY *pub_key = X509_get_pubkey(x);
	if (pub_key == NULL) { printf("%s\n", ERR_error_string(ERR_get_error(), NULL));  return -1; }

	//pem 
    //EVP_PKEY *pri_key = open_private_key("D:\\systemp\\gen\\privatekey.pem","123");  
	//private key
	EVP_PKEY *pri_key = open_private_key("D:\\systemp\\cert\\rsa_private.key",NULL);  

    printf("origin text:%s\n",origin_text);  
    rsa_key_encrypt(pub_key, (const unsigned char *)&origin_text, sizeof(origin_text), (unsigned char *)enc_text, enc_len);  
	printf("encode text:%s\n",enc_text);
    rsa_key_decrypt(pri_key, (const unsigned char *)enc_text, enc_len,   
        (unsigned char *)dec_text, dec_len, "123");  
	printf("decode text:%s\n",dec_text);
    return 0;  
}  
void rsa_callback(int a, int b, void *c){
	printf("[rsa_callback param] %d %d %x\n",a,b,c);
}
//生成rsa 私钥
int generate_rsa_private_key(){
	RSA *r;
	int bits=1024,ret;
	unsigned long e=RSA_3;

	BIGNUM *bne;
	r=RSA_generate_key(bits,e,rsa_callback,NULL);
	RSA_print_fp(stdout,r,11); //打印私钥信息
	RSA_free(r);


	bne=BN_new();  //新生成一个BIGNUM结构
	ret=BN_set_word(bne,e);  //设置大数bne为整数w
	r=RSA_new();  //初始化一个RSA结构
	ret=RSA_generate_key_ex(r,bits,bne,NULL);
	if(ret!=1)
	{
		printf("RSA_generate_key_ex err!\n");
		return -1;
	}
	RSA_print_fp(stdout,r,11); //打印私钥信息
	//RSA_free(r); //释放一个RSA结构
	//BN_free(bne);//释放一个BIGNUM结构
	return 0;
}

void encodeCert(char *pDecode, int *len) {
	char strData[100] = "this is a test!";
	RSA* rsa = RSA_new();
	BIO *b = BIO_new_file("D:\\systemp\\cert\\cert.crt", "r");
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
	FILE* hPriKeyFile = fopen("D:\\systemp\\cert\\rsa_private.key", "r");
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
	pDecode[nLen]='\0';
	if (ret >= 0)
	{
		printf("[decode]:%s\n", pDecode);
	}
	delete[] pDecode;
	RSA_free(pRSAPriKey);
	fclose(hPriKeyFile);
	CRYPTO_cleanup_all_ex_data();
}
void certTest(){
	char encodeData[4096]={0};
	int encodeDataLen;
	encodeCert(encodeData, &encodeDataLen);
	decodeCert(encodeData, encodeDataLen);
}
int main2(int argc, char *argv[]){
	//sm3Test();
	//sms4Test();
	rsaTest();
	//generate_rsa_private_key();
	//certTest();
	return 0;
}