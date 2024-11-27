

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include<windows.h>
#include <openssl/evp.h>
#include <openssl/x509.h>

#define CERT_PATH "x:\\xxx\\mycert"
#define ROOT_CERT "root.cer"
#define WIN71H "rsa_enc.cer"
#define WIN71Y "rsa_enc_2.cer"


#define GET_DEFAULT_CA_CERT(str) sprintf(str, "%s\\%s", CERT_PATH, ROOT_CERT)
#define GET_CUSTOM_CERT(str, path, name) sprintf(str, "%s\\%s", path, name)

#define MAX_LEGTH 4096


int my_load_cert(unsigned char *str, unsigned long *str_len,
	const char *verify_cert, const unsigned int cert_len)
{
	FILE *fp;
	fp = fopen(verify_cert, "rb");
	if (NULL == fp)
	{
		fprintf(stderr, "fopen fail\n");
		return -1;
	}

	*str_len = fread(str, 1, cert_len, fp);
	fclose(fp);
	return 0;
}

X509 *der_to_x509(const unsigned char *der_str, unsigned int der_str_len)
{
	X509 *x509;
	x509 = d2i_X509(NULL, &der_str, der_str_len);
	if (NULL == x509)
	{
		fprintf(stderr, "d2i_X509 fail\n");

		return NULL;
	}
	return x509;
}
void tGetX509Info(char * cerPath)
{
	unsigned char usrCertificate[4096];
	unsigned long usrCertificateLen;
	X509 *x509Cert = NULL;
	const unsigned char *pTmp = NULL;
	X509_NAME *issuer = NULL;
	X509_NAME *subject = NULL;
	int i;
	int entriesNum;
	X509_NAME_ENTRY *name_entry;
	ASN1_INTEGER *Serial = NULL;
	long Nid;
	ASN1_TIME *time;
	EVP_PKEY *pubKey;
	long Version;
	FILE *fp;
	//unsigned char derpubkey[1024];
	//int derpubkeyLen;
	unsigned char msginfo[1024];
	int msginfoLen;
	unsigned short *pUtf8 = NULL;
	int nUtf8;
	int rv;

	fp = fopen(cerPath, "rb");
	if (fp == NULL)
	{
		printf("open file err.\n");
		return;
	}
	usrCertificateLen = fread(usrCertificate, 1, 4096, fp);
	fclose(fp);
	pTmp = usrCertificate;
	x509Cert = d2i_X509(NULL, &pTmp, usrCertificateLen);
	if (x509Cert == NULL)
	{
		printf("d2i_X509 err.\n");
		return;
	}

	printf("---------------Certificate Info --------------\n");
	Version = X509_get_version(x509Cert);
	printf("X509 Version:%d\n", Version);
	Serial = X509_get_serialNumber(x509Cert);
	printf("serialNumber:");
	for (i = 0; i<Serial->length; i++)
	{
		printf("%02x", Serial->data[i]);
	}
	printf("\n--------------------------------------------\n");
	issuer = X509_get_issuer_name(x509Cert);
	entriesNum = sk_X509_NAME_ENTRY_num(issuer->entries);
	for (i = 0; i<entriesNum; i++)
	{
		name_entry = sk_X509_NAME_ENTRY_value(issuer->entries, i);
		Nid = OBJ_obj2nid(name_entry->object);
		if (name_entry->value->type == V_ASN1_UTF8STRING)
		{
			nUtf8 = 2 * name_entry->value->length;
			pUtf8=(unsigned short *)malloc(nUtf8);
			memset(pUtf8, 0, nUtf8);
			rv = MultiByteToWideChar(
				CP_UTF8,  // CodePage：指定执行转换的多字节字符所使用的字符集
				0,		//一组位标记，用以指出是否未转换成预作或宽字符（若组合形式存在），是否使用象形文字替代控制字符，以及如何处理无效字符。
				(char*)name_entry->value->data, //指向待转换的字符串的缓冲区。 
				name_entry->value->length, //指定由参数lpMultiByteStr指向的字符串中字节的个数。可以设置为-1，会自动判断lpMultiByteStr指定的字符串的长度
				(LPWSTR)pUtf8,  //指向接收被转换字符串的缓冲区。 
				nUtf8); //指定由参数lpWideCharStr指向的缓冲区的宽字节数。若此值为0，函数不会执行转换，而是返回目标缓存lpWideChatStr所需的宽字符数。
			rv = WideCharToMultiByte(
				CP_ACP,
				0,
				(LPCWCH)pUtf8,
				rv,
				(char*)msginfo,
				nUtf8,
				NULL,
				NULL);
			free(pUtf8);
			pUtf8 = NULL;
			msginfoLen = rv;
			msginfo[msginfoLen] = '\0';
		}
		else
		{
			msginfoLen = name_entry->value->length;
			memcpy(msginfo, name_entry->value->data, msginfoLen);
			msginfo[msginfoLen] = '\0';
		}
		switch (Nid)
		{
		case NID_countryName:
			printf("Issuer's countryName:%s\n", msginfo);
			break;
		case NID_stateOrProvinceName:
			printf("Issuer's ProvinceName:%s\n", msginfo);
			break;
		case NID_localityName:
			printf("Issuer's localityName:%s\n", msginfo);
			break;
		case NID_organizationName:
			printf("Issuer's organizationName:%s\n", msginfo);
			break;
		case NID_organizationalUnitName:
			printf("Issuer's organizationalUnitName:%s\n", msginfo);
			break;
		case NID_commonName:
			printf("Issuer's commonName:%s\n", msginfo);
			break;
		case NID_pkcs9_emailAddress:
			printf("Issuer's emailAddress:%s\n", msginfo);
			break;
		}
	}
	printf("-----------------\n");
	subject = X509_get_subject_name(x509Cert);
	entriesNum = sk_X509_NAME_ENTRY_num(subject->entries);
	for (i = 0; i<entriesNum; i++)
	{
		name_entry = sk_X509_NAME_ENTRY_value(subject->entries, i);
		Nid = OBJ_obj2nid(name_entry->object);
		if (name_entry->value->type == V_ASN1_UTF8STRING)
		{
			nUtf8 = 2 * name_entry->value->length;
			pUtf8=(unsigned short *)malloc(nUtf8);
			memset(pUtf8, 0, nUtf8);
			rv = MultiByteToWideChar(
				CP_UTF8,
				0,
				(char*)name_entry->value->data,
				name_entry->value->length,
				(LPWSTR)pUtf8,
				nUtf8);
			rv = WideCharToMultiByte(
				CP_ACP,
				0,
				(LPWSTR)pUtf8,
				rv,
				(char*)msginfo,
				nUtf8,
				NULL,
				NULL);
			free(pUtf8);
			pUtf8 = NULL;
			msginfoLen = rv;
			msginfo[msginfoLen] = '\0';
		}
		else
		{
			msginfoLen = name_entry->value->length;
			memcpy(msginfo, name_entry->value->data, msginfoLen);
			msginfo[msginfoLen] = '\0';
		}
		switch (Nid)
		{
		case NID_countryName:
			printf("Subject's countryName:%s\n", msginfo);
			break;
		case NID_stateOrProvinceName:
			printf("Subject's ProvinceName:%s\n", msginfo);
			break;
		case NID_localityName:
			printf("Subject's localityName:%s\n", msginfo);
			break;
		case NID_organizationName:
			printf("Subject's organizationName:%s\n", msginfo);
			break;
		case NID_organizationalUnitName:
			printf("Subject's organizationalUnitName:%s\n", msginfo);
			break;
		case NID_commonName:
			printf("Subject's commonName:%s\n", msginfo);
			break;
		case NID_pkcs9_emailAddress:
			printf("Subject's emailAddress:%s\n", msginfo);
			break;
		}
	}
	printf("-----------------\n");
	time = X509_get_notBefore(x509Cert);
	printf("Cert notBefore:%s\n", time->data);
	time = X509_get_notAfter(x509Cert);
	printf("Cert notAfter:%s\n", time->data);
	printf("---------------------------\n");
	pubKey = X509_get_pubkey(x509Cert);
	/*    pTmp=derpubkey;
	derpubkeyLen=i2d_PublicKey(pubKey,&pTmp);
	printf("PublicKey is:\n");
	for(i=0;i<derpubkeyLen;i++)
	{
	printf("%02x",derpubkey[i]);
	}
	*/
	printf("\n--------------------\n");
	X509_free(x509Cert);
	return;
}
int x509_verify()
{
	int ret;
	char cert[MAX_LEGTH];

	unsigned char user_der[MAX_LEGTH];
	unsigned long user_der_len;
	X509 *user = NULL;

	unsigned char ca_der[MAX_LEGTH];
	unsigned long ca_der_len;
	X509 *ca = NULL;

	X509_STORE *ca_store = NULL;
	X509_STORE_CTX *ctx = NULL;
	STACK_OF(X509) *ca_stack = NULL;

	/* x509初始化 */
	ca_store = X509_STORE_new();
	ctx = X509_STORE_CTX_new();

	/* root ca*/
	GET_DEFAULT_CA_CERT(cert);
	/* 从文件中读取 */
	my_load_cert(ca_der, &ca_der_len, cert, MAX_LEGTH);
	/* DER编码转X509结构 */
	ca = der_to_x509(ca_der, ca_der_len);
	/* 加入证书存储区 */
	ret = X509_STORE_add_cert(ca_store, ca);
	if (ret != 1)
	{
		fprintf(stderr, "X509_STORE_add_cert fail, ret = %d\n", ret);
		goto EXIT;
	}

	/* 需要校验的证书 */
	GET_CUSTOM_CERT(cert, CERT_PATH, WIN71H);
	tGetX509Info(cert);
	my_load_cert(user_der, &user_der_len, cert, MAX_LEGTH);
	user = der_to_x509(user_der, user_der_len);
	 // //证书上下文  证书库，存在证书链    待验证X509证书
	ret = X509_STORE_CTX_init(ctx, ca_store, user, ca_stack);
	if (ret != 1)
	{
		fprintf(stderr, "X509_STORE_CTX_init fail, ret = %d\n", ret);
		goto EXIT;
	}

	//openssl-1.0.1c/crypto/x509/x509_vfy.h
	ret = X509_verify_cert(ctx); ////根据返回值可以确认X509证书是否有效，也可以根据X509_STORE_CTX_get_error和X509_verify_cert_error_string函数来确认无效原因 
	if (ret != 1)
	{
		fprintf(stderr, "X509_verify_cert fail, ret = %d, error id = %d, %s\n",
			ret, ctx->error, X509_verify_cert_error_string(ctx->error));
		goto EXIT;
	}
EXIT:
	X509_free(user);
	X509_free(ca);

	X509_STORE_CTX_cleanup(ctx);
	X509_STORE_CTX_free(ctx);

	X509_STORE_free(ca_store);

	return ret == 1 ? 0 : -1;
}

int main()
{
	OpenSSL_add_all_algorithms();
	x509_verify();
	return 0;
}

