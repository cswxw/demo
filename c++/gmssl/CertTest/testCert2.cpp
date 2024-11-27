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
		printf("��ȡ֤�����\n");
		return;
	}
	char Cert[4096] = { 0 };
	char msginfo[4096] = { 0 };
	int msginfoLen;
	size_t Certlen = fread(Cert, 1, 4096, fp);
	fclose(fp);
	//�ж��Ƿ�ΪDER������û�֤�飬��ת��ΪX509�ṹ��
	char *pTmp = Cert;
	X509 *usrCert = d2i_X509(NULL, (const unsigned char **)&pTmp, Certlen);
	if (usrCert == NULL)
	{
		BIO *b;
		/* �ж��Ƿ�ΪPEM��ʽ������֤�� */
		b = BIO_new_file(certfile, "r");
		usrCert = PEM_read_bio_X509(b, NULL, NULL, NULL);
		BIO_free(b);
		if (usrCert == NULL)
		{
			printf("ת����ʽ����");
			return;
		}
	}
	//����֤��
	X509_NAME *issuer = NULL;//X509_NAME�ṹ�壬����֤��䷢����Ϣ
	X509_NAME *subject = NULL;//X509_NAME�ṹ�壬����֤��ӵ������Ϣ
							  //��ȡ֤��汾
	long Version = X509_get_version(usrCert);
	//��ȡ֤��䷢����Ϣ��X509_NAME�ṹ�屣���˶�����Ϣ���������ҡ���֯�����š�ͨ������mail�ȡ�
	issuer = X509_get_issuer_name(usrCert);
	//��ȡX509_NAME��Ŀ����
	int entriesNum = sk_X509_NAME_ENTRY_num(issuer->entries);
	//ѭ����ȡ����Ŀ��Ϣ
	for (int i = 0; i<entriesNum; i++)
	{
		//��ȡ��I����Ŀֵ
		X509_NAME_ENTRY * name_entry = sk_X509_NAME_ENTRY_value(issuer->entries, i);
		//��ȡ����ID
		int Nid = OBJ_obj2nid(name_entry->object);
		//�ж���Ŀ���������
		if (name_entry->value->type == V_ASN1_UTF8STRING)
			//��UTF8��������ת���ɿɼ��ַ�
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
		//����NID��ӡ����Ϣ
		switch (Nid)
		{
		case NID_countryName://����
			printf("issuer 's countryName:%s\n", msginfo);

			break;
		case NID_stateOrProvinceName://ʡ
			printf("issuer 's ProvinceName:%s\n", msginfo);

			break;
		case NID_localityName://����
			printf("issuer 's localityName:%s\n", msginfo);

			break;
		case NID_organizationName://��֯
			printf("issuer 's organizationName:%s\n", msginfo);

			break;
		case NID_organizationalUnitName://��λ
			printf("issuer 's organizationalUnitName:%s\n", msginfo);

			break;
		case NID_commonName://ͨ����
			printf("issuer 's commonName:%s\n", msginfo);


			break;
		case NID_pkcs9_emailAddress://Mail
			printf("issuer 's emailAddress:%s\n", msginfo);


			break;
		}//end switch
	}
	//��ȡ֤��������Ϣ����ǰ�����ƣ��ڴ�ʡ��
	subject = X509_get_subject_name(usrCert);


		//��ȡ֤����Ч����
	ASN1_TIME * time = X509_get_notBefore(usrCert);
	printf("Cert notBefore:%s\n", time->data);


	//��ȡ֤���������
	time = X509_get_notAfter(usrCert);
	printf("Cert notAfter:%s\n", time->data);

	//��ȡ֤�鹫Կ
	EVP_PKEY * pubKey = X509_get_pubkey(usrCert);
	char pTmp2[4096] = { 0 };
	//��֤�鹫ԿתΪDER���������
	int derpubkeyLen = i2d_PublicKey(pubKey, (unsigned char **)&pTmp2);
	printf("PublicKey is: \n");
	for (int i = 0; i < derpubkeyLen; i++)
	{

		//printf("%02x", pTmp2[i]);

	}

	//�ͷŽṹ���ڴ�
	X509_free(usrCert);
}
