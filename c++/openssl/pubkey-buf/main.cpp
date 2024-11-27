#include "main.h"

int generate_x509_req(int keytype,
	char *pubkey, int pubkeylen,
	char *cn, int cnlen,
	char *ext, int extlen,
	char *o, int olen,
	char *ou, int oulen,
	char *p10sign, int p10signlen,
	unsigned char *p10str, int *p10strlen
)
{
#define ERR_X509_REQ_new -1
	FILE *fp;
	int ret = 0;
	X509_REQ *pX509Req = NULL;
	int version;
	X509_NAME *pX509Name = NULL;
	X509_NAME_ENTRY *pX509NameEntry = NULL;
	int req_len;
	unsigned char *p10req = NULL;
	unsigned char *penc = NULL;
	X509_PUBKEY *x509_pubkey;

	info("enter GenerateP10Request...");

	pX509Req = X509_REQ_new();//new
	if (pX509Req == NULL)
	{
		info("ERR_X509_REQ_new error.");

		return ERR_X509_REQ_new;
	}

	info("keytype:%d", keytype);
	version = 1;
	ret = X509_REQ_set_version(pX509Req, version); //设置版本

	info("before return 1\n");

	pX509Name = X509_NAME_new();// new

	pX509NameEntry = X509_NAME_ENTRY_create_by_txt(&pX509NameEntry, "countryName", V_ASN1_UTF8STRING, (unsigned char *)"CN", strlen("CN"));
	X509_NAME_add_entry(pX509Name, pX509NameEntry, 0, -1);
	X509_NAME_ENTRY_free(pX509NameEntry);
	pX509NameEntry = NULL;

	if (cn != NULL)
	{
		pX509NameEntry = X509_NAME_ENTRY_create_by_txt(&pX509NameEntry, "commonName", V_ASN1_UTF8STRING, (unsigned char *)cn, cnlen);
		X509_NAME_add_entry(pX509Name, pX509NameEntry, 0, -1);
		X509_NAME_ENTRY_free(pX509NameEntry);
		pX509NameEntry = NULL;
	}

	if (ext != NULL)
	{
		pX509NameEntry = X509_NAME_ENTRY_create_by_txt(&pX509NameEntry, "2.5.29.17", V_ASN1_UTF8STRING, (unsigned char *)ext, extlen);
		X509_NAME_add_entry(pX509Name, pX509NameEntry, 0, -1);
		X509_NAME_ENTRY_free(pX509NameEntry);
		pX509NameEntry = NULL;
	}

	if (ou != NULL)
	{
		pX509NameEntry = X509_NAME_ENTRY_create_by_txt(&pX509NameEntry, "organizationalUnitName", V_ASN1_UTF8STRING, (unsigned char *)ou, oulen);
		X509_NAME_add_entry(pX509Name, pX509NameEntry, 0, -1);
		X509_NAME_ENTRY_free(pX509NameEntry);
		pX509NameEntry = NULL;
	}

	if (o != NULL)
	{
		pX509NameEntry = X509_NAME_ENTRY_create_by_txt(&pX509NameEntry, "organizationName", V_ASN1_UTF8STRING, (unsigned char *)o, olen);
		X509_NAME_add_entry(pX509Name, pX509NameEntry, 0, -1);
		X509_NAME_ENTRY_free(pX509NameEntry);
		pX509NameEntry = NULL;
	}

	info("before return 2\n");
	X509_REQ_set_subject_name(pX509Req, pX509Name);//设置subjectName

	x509_pubkey = X509_REQ_get_X509_PUBKEY(pX509Req);//获取pubkey

	info("before return 3: %p, %d\n", pubkey, pubkeylen);

	penc =(unsigned char *) OPENSSL_malloc(pubkeylen + 1);
	memcpy(penc, pubkey, pubkeylen);

	if (keytype == 1)									//RSA
	{
		X509_PUBKEY_set0_param(x509_pubkey, OBJ_nid2obj(EVP_PKEY_RSA), V_ASN1_NULL,
			OBJ_txt2obj("1.2.840.113549.1.1.5", 0), penc, pubkeylen);
		//依次调用，将类型地址放入
		//X509_PUBKEY_set0_param  if (!X509_ALGOR_set0(pub->algor, aobj, ptype, pval))
		//int X509_ALGOR_set0(X509_ALGOR *alg, ASN1_OBJECT *aobj, int ptype, void *pval) ASN1_TYPE_set(alg->parameter, ptype, pval);
		 
		//ASN1_TYPE_set(ASN1_TYPE *a, int type, void *value)    a->value.ptr = value;



		//rsa_pub_encode  //rsa_pub_decode
		
	}
	else if (keytype == 3)							//SM2
	{
		X509_PUBKEY_set0_param(x509_pubkey, OBJ_txt2obj("1.2.840.10045.2.1", 0), V_ASN1_OBJECT,
			OBJ_txt2obj("1.2.156.10197.1.301", 0), penc, pubkeylen);
		//static int eckey_pub_decode(EVP_PKEY *pkey, X509_PUBKEY *pubkey)  从 pubkey 获取公钥，设置到 pkey
		//    eckey_pub_decode,
		//static int eckey_pub_encode(X509_PUBKEY *pk, const EVP_PKEY *pkey) 从pkey中获取公钥，转换为字符缓冲区数据，配置到pk中
		//    eckey_pub_encode,
	}
	else
	{
		info("keytype is error.");
		X509_REQ_free(pX509Req);
		return 2;
	}

	info("before return 4\n");
	req_len = i2d_X509_REQ(pX509Req, NULL);  //获取长度
	p10req = (unsigned char *)malloc(req_len + 1);
	if (p10req == NULL) {
		info("memory alloc error.");
		X509_NAME_free(pX509Name);
		X509_REQ_free(pX509Req);
		return 3;
	}

	info("before return 5, %d\n", *p10strlen);
	memset(p10req, 0, req_len + 1);

	req_len = i2d_X509_REQ(pX509Req, &p10req);
	if (*p10strlen < req_len)
	{
		info("Buffer too small, %d.", req_len);
		X509_NAME_free(pX509Name);
		X509_REQ_free(pX509Req);
		return 4;
	}

	info("before return 6, %d\n", req_len);
	memcpy(p10str, p10req - req_len, req_len);
	*p10strlen = req_len;
	free(p10req - req_len);
	info("before return 7\n");
	fp = fopen("/tmp/tmp.p10", "wb");

	fwrite(p10str, *p10strlen, 1, fp);
	fclose(fp);

	if (pX509Name != NULL)
		X509_NAME_free(pX509Name);
	if (pX509Req != NULL)
		X509_REQ_free(pX509Req);

	info("before return 0\n");
	return 0;
}

void get() {
	int X509_PUBKEY_get0_param(ASN1_OBJECT **ppkalg,
		const unsigned char **pk, int *ppklen,
		X509_ALGOR **pa, X509_PUBKEY *pub);
}

int main() {
	int keytype = 1;

	char pubkey[2048] = { 0 };
	int pubkeylen = 1024+1;


	char cn[2048] = { 0 }; //comman name
	//int cnlen = 0;

	char ext[2048] = { 0 }; //ext
	//int extlen = 0;

	char o[2048] = { 0 }; //organization
	//int olen = 0;

	char ou[2048] = { 0 };// organization unit
	//int oulen = 0;

	char p10sign[2048] = { 0 };
	int p10signlen = 0;


	unsigned char p10str[2048] = { 0 };
	int p10strlen = 0;



	//init
	keytype = 1;
	strcpy(cn, "abc01");
	strcpy(o, "yiyuan");
	strcpy(ou, "kaifa");
	strcpy(ext, "ext001");
	generate_x509_req(keytype,
		pubkey, pubkeylen,
		cn, -1,
		ext, -1,
		o, -1,
		ou, -1,
		p10sign, p10signlen,
		p10str, &p10strlen);

	return 1;
}