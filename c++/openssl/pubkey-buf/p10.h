#pragma once
#include "main.h"

typedef struct asn1_type_st {
	int type;
	union {
		char *ptr;
		ASN1_BOOLEAN boolean;
		ASN1_STRING *asn1_string;
		ASN1_OBJECT *object;
		ASN1_INTEGER *integer;
		ASN1_ENUMERATED *enumerated;
		ASN1_BIT_STRING *bit_string;
		ASN1_OCTET_STRING *octet_string;
		ASN1_PRINTABLESTRING *printablestring;
		ASN1_T61STRING *t61string;
		ASN1_IA5STRING *ia5string;
		ASN1_GENERALSTRING *generalstring;
		ASN1_BMPSTRING *bmpstring;
		ASN1_UNIVERSALSTRING *universalstring;
		ASN1_UTCTIME *utctime;
		ASN1_GENERALIZEDTIME *generalizedtime;
		ASN1_VISIBLESTRING *visiblestring;
		ASN1_UTF8STRING *utf8string;
		/*
		* set and sequence are left complete and still contain the set or
		* sequence bytes
		*/
		ASN1_STRING *set;
		ASN1_STRING *sequence;
		ASN1_VALUE *asn1_value;
	} value;
} ASN1_TYPE;

struct asn1_object_st {
	const char *sn, *ln;
	int nid;
	int length;
	const unsigned char *data;  /* data remains const after init */
	int flags;                  /* Should we free this one */
};
typedef struct asn1_object_st ASN1_OBJECT;
typedef struct ASN1_ENCODING_st {
	unsigned char *enc;         /* DER encoding */
	long len;                   /* Length of encoding */
	int modified;               /* set to 1 if 'enc' is invalid */
} ASN1_ENCODING;

/* Name from RFC 5280. */
struct X509_name_st {
	STACK_OF(X509_NAME_ENTRY) *entries; /* DN components */
	int modified;               /* true if 'bytes' needs to be built */
	BUF_MEM *bytes;             /* cached encoding: cannot be NULL */
								/* canonical encoding used for rapid Name comparison */
	unsigned char *canon_enc;
	int canon_enclen;
} /* X509_NAME */;
struct X509_algor_st {
	ASN1_OBJECT *algorithm;
	ASN1_TYPE *parameter;
} /* X509_ALGOR */;

struct X509_pubkey_st {
	X509_ALGOR *algor;
	ASN1_BIT_STRING *public_key;
	EVP_PKEY *pkey;
};

/* PKCS#10 certificate request */
struct X509_req_info_st {
	ASN1_ENCODING enc;          /* cached encoding of signed part */
	ASN1_INTEGER *version;      /* version, defaults to v1(0) so can be NULL */
	X509_NAME *subject;         /* certificate request DN */
	X509_PUBKEY *pubkey;        /* public key of request */
								/*
								* Zero or more attributes.
								* NB: although attributes is a mandatory field some broken
								* encodings omit it so this may be NULL in that case.
								*/
	STACK_OF(X509_ATTRIBUTE) *attributes;
};

struct X509_algor_st
{
	ASN1_OBJECT *algorithm;
	ASN1_TYPE *parameter;
} /* X509_ALGOR */;

typedef struct X509_req_st
{
	X509_REQ_INFO *req_info;
	X509_ALGOR *sig_alg;
	ASN1_BIT_STRING *signature;
	int references;
} X509_REQ;





/*
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
	ret = X509_REQ_set_version(pX509Req, version); //…Ë÷√∞Ê±æ

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
	X509_REQ_set_subject_name(pX509Req, pX509Name);

	x509_pubkey = X509_REQ_get_X509_PUBKEY(pX509Req);

	info("before return 3: %p, %d\n", pubkey, pubkeylen);

	penc =(unsigned char *) OPENSSL_malloc(pubkeylen + 1);
	memcpy(penc, pubkey, pubkeylen);

	if (keytype == 1)									//RSA
	{
		X509_PUBKEY_set0_param(x509_pubkey, OBJ_nid2obj(EVP_PKEY_RSA), V_ASN1_NULL,
			OBJ_txt2obj("1.2.840.113549.1.1.5", 0), penc, pubkeylen);
	}
	else if (keytype == 3)							//SM2
	{
		X509_PUBKEY_set0_param(x509_pubkey, OBJ_txt2obj("1.2.840.10045.2.1", 0), V_ASN1_OBJECT,
			OBJ_txt2obj("1.2.156.10197.1.301", 0), penc, pubkeylen);
	}
	else
	{
		info("keytype is error.");
		X509_REQ_free(pX509Req);
		return 2;
	}

	info("before return 4\n");
	req_len = i2d_X509_REQ(pX509Req, NULL);
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
*/