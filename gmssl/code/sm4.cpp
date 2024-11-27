//sms4 do_crypt
int do_crypt_2(unsigned char *in,int inlen,unsigned char *out,int* poutlen,int isEnc)
{
	unsigned char key[32] = {0x48, 0x6e, 0x89, 0x2a, 0x2e, 0x88, 0x43, 0xd6, 0xb0, 0xef, 0x1e, 0xaa, 0x3f, 0xa1, 0x9a, 0x37,
							0xea, 0xe3, 0x49, 0x56, 0xa2, 0x24, 0xda, 0xe8, 0xa7, 0x3c, 0xa7, 0xdb, 0xf8, 0x87, 0x54, 0x6b,
							  };
	unsigned char iv[16] = {0x55, 0x2a, 0x0d, 0xca, 0xc5, 0xaa, 0x49, 0x24, 0x41, 0x7c, 0x9d, 0xb4, 0xb9, 0x8b, 0x53, 0x83};
	int encLen = 0;
	int outlen = 0;


	EVP_CIPHER_CTX *ctx;
	ctx = EVP_CIPHER_CTX_new();

	if(isEnc == 1){
		//加密
		EVP_CipherInit_ex(ctx, EVP_aes_256_ecb(), NULL, key, iv, 1);
	}else{
		//解密
		EVP_CipherInit_ex(ctx, EVP_aes_256_ecb(), NULL, key, iv, 0);
	}

	if(!EVP_CipherUpdate(ctx, out, &outlen, in, inlen))
   {
		 /* Error */
		EVP_CIPHER_CTX_free(ctx);
		return 0;
   }
	encLen = outlen;
	if(!EVP_CipherFinal(ctx, out+outlen, &outlen))
	{
	   /* Error */
	   EVP_CIPHER_CTX_free(ctx);
	   return 0;
	}
	encLen += outlen;
	EVP_CIPHER_CTX_free(ctx);
	*poutlen = encLen;
	return 1;
}

