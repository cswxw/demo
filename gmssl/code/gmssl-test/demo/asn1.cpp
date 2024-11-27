#include <openssl/asn1.h>
int main(int argc , char* argv[])
{
	const char oid[]={"2.99999.3"};
	int i;
	unsigned char *buf;
	i=a2d_ASN1_OBJECT(NULL,0,oid,-1);
	if (i <= 0)
		return 0;
	buf=(unsigned char *)malloc(sizeof(unsigned char)*i);
	i=a2d_ASN1_OBJECT(buf,i,oid,-1);
	free(buf);
	return 0; }
