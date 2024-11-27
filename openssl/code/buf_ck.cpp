#ifdef USE_SSL
	char certBuf[]= "..............";
	char privateKey[]= "..............";
	
	
	SSL_CTX *ctx = NULL;
	X509 *x = NULL;
	RSA *rsa = NULL;
	EVP_PKEY *pkey = EVP_PKEY_new();
	// SSL 库初始化 
	SSL_library_init();
	// 载入所有 SSL 算法 
	OpenSSL_add_all_algorithms();
	// 载入所有 SSL 错误消息 
	SSL_load_error_strings();

	ctx = SSL_CTX_new(SSLv23_server_method());
	if (ctx == NULL) {
	    ERR_print_errors_fp(stdout);
	    ret = -1;goto err;
	}
	
	BIO * cert = NULL;
	cert = BIO_new(BIO_s_mem());
	BIO_write(cert,certBuf,strlen(certBuf));
	BIO_seek(cert,0);
	if (!PEM_read_bio_X509(cert, &x, 0, NULL))
	{
		ERR_print_errors_fp(stdout);
		BIO_free(cert);
		cert = NULL;
		ret = -1;goto err;
	}
	if (SSL_CTX_use_certificate(ctx, x) <= 0) {
	  ERR_print_errors_fp(stdout);
	  BIO_free(cert);
	  cert = NULL;
	  ret = -1;goto err;
	}
	BIO_free(cert);
	cert = NULL;
	
	//私钥
	
	BIO * in = NULL;
	in = BIO_new(BIO_s_mem());
	BIO_write(in,privateKey,strlen(privateKey));
	BIO_seek(in,0);
	
	rsa=PEM_read_bio_RSAPrivateKey(in,NULL,
			NULL,NULL);
	if(rsa == NULL){
		ERR_print_errors_fp(stdout);
		BIO_free(in);
		in = NULL;
		ret = -1;goto err;
	}
	BIO_free(in);
	in=NULL;
	
	
	EVP_PKEY_set1_RSA(pkey, rsa);
	
  if (SSL_CTX_use_PrivateKey(ctx,pkey)<=0) {
    ERR_print_errors_fp(stdout);
    ret = -1;goto err;
	}

	
	// 检查用户私钥是否正确 
	if (!SSL_CTX_check_private_key(ctx)) {
    ERR_print_errors_fp(stdout);
    exit(1);
	}
#endif


