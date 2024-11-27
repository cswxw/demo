


int  showFileBase64 (const char * infile,int  with_new_line)
{
	//BIO * bmem = NULL;
	BIO * in = NULL;
	BIO * out = NULL;
	BIO * b64 = NULL;
 	//BUF_MEM * bptr = NULL;
 	char buf[4096]={0};
 	int ret = 0;
 	
	b64 = BIO_new(BIO_f_base64());
	if(!with_new_line) {
		BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	}
	//bmem = BIO_new(BIO_s_mem());
	out = BIO_new_fp(stdout, BIO_NOCLOSE);
	in=BIO_new_file(infile,"r");
	ret =BIO_read(in,buf,sizeof buf);
	printf(" BIO_read :%d\n",ret);
	b64 = BIO_push(b64, out);
	printf("===========%s base64 start==================\n",infile);
	BIO_write(b64, buf, ret );
	
	//BIO_flush(b64);
	printf("\n===========%s base64 end==================\n",infile);

	BIO_free_all(b64);
	return 1;
}


