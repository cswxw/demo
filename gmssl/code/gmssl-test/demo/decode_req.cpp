#include <openssl/pem.h>
int main()
{
	BIO *in;
	X509_REQ *req=NULL,**req2=NULL;
	FILE *fp;
	unsigned char buf[1024],*p;
	int len;
	in=BIO_new_file("certreq.txt","r");
	req=PEM_read_bio_X509_REQ(in,NULL,NULL,NULL);
	if(req==NULL)
	{
		printf("DER解码错误!\n");
	}
	else
	{
		printf("DER解码成功!\n");
	}
	fp=fopen("certreq2.txt","r");
	len=fread(buf,1,1024,fp);
	fclose(fp);
	p=buf;
	req2=(X509_REQ **)malloc(sizeof(X509_REQ *));
	d2i_X509_REQ(req2,(const unsigned char**)&p,len);
	if(*req2==NULL)
	{
		printf("DER解码错误!\n");
	}
	else
	{
		printf("DER解码成功!\n");
	}
	X509_REQ_free(*req2);
	free(req2);
	return 0;
}
