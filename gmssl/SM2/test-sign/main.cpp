
#include "main.h"





EC_KEY *  load_public_cert(const char *certfile)  
{  
	EC_KEY * eckey = NULL;
    BIO *b = BIO_new_file(certfile, "r");
	if (b == NULL) { printf("%s\n", ERR_error_string(ERR_get_error(), NULL));  return eckey; }

#if 1
	X509 *x = PEM_read_bio_X509(b, NULL, NULL, NULL);
	if (x == NULL) { printf("%s\n", ERR_error_string(ERR_get_error(), NULL)); BIO_free(b);  return eckey; }
	EVP_PKEY *pub_key = X509_get_pubkey(x);
	if (pub_key == NULL) { printf("%s\n", ERR_error_string(ERR_get_error(), NULL)); BIO_free(b);  return eckey; }
	
	 eckey = EVP_PKEY_get1_EC_KEY(pub_key);

	//EVP_PKEY_free(pub_key); //释放pkey
#else
	eckey = PEM_read_bio_EC_PUBKEY(b, NULL, NULL, NULL);   //error
	//eckey = d2i_EC_PUBKEY_bio(b, NULL);
	if(!eckey) {
		printf("%s\n", ERR_error_string(ERR_get_error(), NULL)); 
	}
#endif
	BIO_free(b);  
    return eckey;
}  



EC_KEY* load_private_key(const char *keyfile, const  char *passwd) {  
	
    EC_KEY* eckey = NULL;  

    BIO *bp = NULL;  
    bp = BIO_new_file(keyfile, "rb");   
    if(NULL == bp)  
    {  
        printf("open_private_key bio file new error!\n");  
	
        return eckey;
    }  
  
    eckey = PEM_read_bio_ECPrivateKey(bp, NULL, NULL,NULL);  
    if(eckey == NULL)  
    {  
        printf("open_private_key failed to PEM_read_bio_RSAPrivateKey!\n");  

        BIO_free(bp);  
        return eckey;
    }  
 	//if (eckey)
	//	EC_KEY_free(eckey);
	BIO_free(bp);
    return eckey;  
}

int main(){
	EC_KEY* pubkey=load_public_cert("sm2_sign.pem"); //error
	EC_KEY* prikey=load_private_key("sm2_sign.key",NULL);
	char dgst[] ="123";
	ECDSA_SIG *sig;
	int        ret;
	int   dgst_len = strlen(dgst);

	sig = ECDSA_do_sign((unsigned char*)dgst, dgst_len, prikey);// 签名
    if (sig == NULL)
        {
        /* error */
        perror("ECDSA_do_sign");
        }
    else
    {// 打印一下签名，r和s
        printf("Signature:\n\tr=%s\n\ts=%s\n", BN_bn2hex(sig->r), BN_bn2hex(sig->s));
    }
	ret = ECDSA_do_verify((unsigned char*)dgst, dgst_len, sig, pubkey);// 验证
	    
	if (ret == -1)
	{
		/* error */
		perror("ECDSA_do_verify");
	}
	else if (ret == 0)
	{
		/* incorrect signature */
		printf("Verified Failure\n");
	}
	else   /* ret == 1 */
	{
		/* signature ok */
		printf("Verified OK\n");
	}    

	if(pubkey)EC_KEY_free(pubkey);
	if(prikey)EC_KEY_free(prikey);

	return 0;
}


