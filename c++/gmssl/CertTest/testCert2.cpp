#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <internal/x509_int.h>
#include <openssl/pem.h>
#include <openssl/cms.h>
#include <openssl/ssl.h>
#include <openssl/e_os2.h>

void showCert()
{
	char certfile[] = "F:\\temp\\cert.crt";
	FILE *fp = fopen(certfile, "rb");
	if (fp == NULL)
	{
		printf("读取证书错误\n");
		return;
	}
	char Cert[4096] = { 0 };
	char msginfo[4096] = { 0 };
	int msginfoLen;
	size_t Certlen = fread(Cert, 1, 4096, fp);
	fclose(fp);
	//判断是否为DER编码的用户证书，并转化为X509结构体
	char *pTmp = Cert;
	X509 *usrCert = d2i_X509(NULL, (const unsigned char **)&pTmp, Certlen);
	if (usrCert == NULL)
	{
		BIO *b;
		/* 判断是否为PEM格式的数字证书 */
		b = BIO_new_file(certfile, "r");
		usrCert = PEM_read_bio_X509(b, NULL, NULL, NULL);
		BIO_free(b);
		if (usrCert == NULL)
		{
			printf("转化格式错误！");
			return;
		}
	}
	//解析证书
	X509_NAME *issuer = NULL;//X509_NAME结构体，保存证书颁发者信息
	X509_NAME *subject = NULL;//X509_NAME结构体，保存证书拥有者信息
							  //获取证书版本
	long Version = X509_get_version(usrCert);
	//获取证书颁发者信息，X509_NAME结构体保存了多项信息，包括国家、组织、部门、通用名、mail等。
	issuer = X509_get_issuer_name(usrCert);
	//获取X509_NAME条目个数
	int entriesNum = sk_X509_NAME_ENTRY_num(issuer->entries);
	//循环读取各条目信息
	for (int i = 0; i<entriesNum; i++)
	{
		//获取第I个条目值
		X509_NAME_ENTRY * name_entry = sk_X509_NAME_ENTRY_value(issuer->entries, i);
		//获取对象ID
		int Nid = OBJ_obj2nid(name_entry->object);
		//判断条目编码的类型
		if (name_entry->value->type == V_ASN1_UTF8STRING)
			//把UTF8编码数据转化成可见字符
		{
			int nUtf8 = 2 * name_entry->value->length;
			unsigned short * pUtf8 = (unsigned short *)malloc(nUtf8);
			memset(pUtf8, 0, nUtf8);
			int rv = MultiByteToWideChar(
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
		//根据NID打印出信息
		switch (Nid)
		{
		case NID_countryName://国家
			printf("issuer 's countryName:%s\n", msginfo);

			break;
		case NID_stateOrProvinceName://省
			printf("issuer 's ProvinceName:%s\n", msginfo);

			break;
		case NID_localityName://地区
			printf("issuer 's localityName:%s\n", msginfo);

			break;
		case NID_organizationName://组织
			printf("issuer 's organizationName:%s\n", msginfo);

			break;
		case NID_organizationalUnitName://单位
			printf("issuer 's organizationalUnitName:%s\n", msginfo);

			break;
		case NID_commonName://通用名
			printf("issuer 's commonName:%s\n", msginfo);


			break;
		case NID_pkcs9_emailAddress://Mail
			printf("issuer 's emailAddress:%s\n", msginfo);


			break;
		}//end switch
	}
	//获取证书主题信息，与前面类似，在此省略
	subject = X509_get_subject_name(usrCert);


		//获取证书生效日期
	ASN1_TIME * time = X509_get_notBefore(usrCert);
	printf("Cert notBefore:%s\n", time->data);


	//获取证书过期日期
	time = X509_get_notAfter(usrCert);
	printf("Cert notAfter:%s\n", time->data);

	//获取证书公钥
	EVP_PKEY * pubKey = X509_get_pubkey(usrCert);
	char pTmp2[4096] = { 0 };
	//把证书公钥转为DER编码的数据
	int derpubkeyLen = i2d_PublicKey(pubKey, (unsigned char **)&pTmp2);
	printf("PublicKey is: \n");
	for (int i = 0; i < derpubkeyLen; i++)
	{

		//printf("%02x", pTmp2[i]);

	}

	//释放结构体内存
	X509_free(usrCert);
}
