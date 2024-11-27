
#include "main.h"
#include <string.h>
#include <openssl/x509.h>
#include <openssl/rsa.h>


X509_NAME *parse_name(char *subject, long chtype, int multirdn)
{
    size_t buflen = strlen(subject)+1; /* to copy the types and values into. due to escaping, the copy can only become shorter */
    char *buf = OPENSSL_malloc(buflen);
    size_t max_ne = buflen / 2 + 1; /* maximum number of name elements */
    char **ne_types = OPENSSL_malloc(max_ne * sizeof (char *));
    char **ne_values = OPENSSL_malloc(max_ne * sizeof (char *));
    int *mval = OPENSSL_malloc (max_ne * sizeof (int));
    
    char *sp = subject, *bp = buf;
    int i, ne_num = 0;
    
    X509_NAME *n = NULL;
    int nid;
    
    if (!buf || !ne_types || !ne_values || !mval)
    {
        //BIO_printf(bio_err, "malloc error\n");
        goto error;
    }
    
    if (*subject != '/')
    {
        //BIO_printf(bio_err, "Subject does not start with '/'.\n");
        goto error;
    }
    sp++; /* skip leading / */
    
    /* no multivalued RDN by default */
    mval[ne_num] = 0;
    
    while (*sp)
    {
        /* collect type */
        ne_types[ne_num] = bp;
        while (*sp)
        {
            if (*sp == '\\') /* is there anything to escape in the type...? */
            {
                if (*++sp)
                    *bp++ = *sp++;
                else
                {
                    //BIO_printf(bio_err, "escape character at end of string\n");
                    goto error;
                }
            }
            else if (*sp == '=')
            {
                sp++;
                *bp++ = '\0';
                break;
            }
            else
                *bp++ = *sp++;
        }
        if (!*sp)
        {
            //BIO_printf(bio_err, "end of string encountered while processing type of subject name element #%d\n", ne_num);
            goto error;
        }
        ne_values[ne_num] = bp;
        while (*sp)
        {
            if (*sp == '\\')
            {
                if (*++sp)
                    *bp++ = *sp++;
                else
                {
                    //BIO_printf(bio_err, "escape character at end of string\n");
                    goto error;
                }
            }
            else if (*sp == '/')
            {
                sp++;
                /* no multivalued RDN by default */
                mval[ne_num+1] = 0;
                break;
            }
            else if (*sp == '+' && multirdn)
            {
                /* a not escaped + signals a mutlivalued RDN */
                sp++;
                mval[ne_num+1] = -1;
                break;
            }
            else
                *bp++ = *sp++;
        }
        *bp++ = '\0';
        ne_num++;
    }
    
    if (!(n = X509_NAME_new()))
        goto error;
    
    for (i = 0; i < ne_num; i++)
    {
        if ((nid=OBJ_txt2nid(ne_types[i])) == NID_undef)
        {
            //BIO_printf(bio_err, "Subject Attribute %s has no known NID, skipped\n", ne_types[i]);
            continue;
        }
        
        if (!*ne_values[i])
        {
            //BIO_printf(bio_err, "No value provided for Subject Attribute %s, skipped\n", ne_types[i]);
            continue;
        }
        
        if (!X509_NAME_add_entry_by_NID(n, nid, chtype, (unsigned char*)ne_values[i], -1,-1,mval[i]))
            goto error;
    }
    
    OPENSSL_free(ne_values);
    OPENSSL_free(ne_types);
    OPENSSL_free(buf);
    OPENSSL_free(mval);
    return n;
    
error:
    X509_NAME_free(n);
    if (ne_values)
        OPENSSL_free(ne_values);
    if (ne_types)
        OPENSSL_free(ne_types);
    if (mval)
        OPENSSL_free(mval);
    if (buf)
        OPENSSL_free(buf);
    return NULL;
}

X509_NAME *CreateDN(char *pbEmail, char *pbCN, char *pbOU, char *pbO, char *pbL, char *pbST, char *pbC)
{
    X509_NAME *pX509Name = NULL;
    if(pbCN == NULL)
    {
        return NULL;
    }
    
    if (!(pX509Name = X509_NAME_new()))
    {
        return NULL;
    }
    X509_NAME_add_entry_by_txt(pX509Name, "emailAddress", V_ASN1_UTF8STRING, pbEmail, -1, -1, 0);
    X509_NAME_add_entry_by_txt(pX509Name, "CN", V_ASN1_UTF8STRING, pbCN, -1, -1, 0);
    X509_NAME_add_entry_by_txt(pX509Name, "OU", V_ASN1_UTF8STRING, pbOU, -1, -1, 0);
    X509_NAME_add_entry_by_txt(pX509Name, "O", V_ASN1_UTF8STRING, pbO, -1, -1, 0);
    X509_NAME_add_entry_by_txt(pX509Name, "L", V_ASN1_UTF8STRING, pbL, -1, -1, 0);
    X509_NAME_add_entry_by_txt(pX509Name, "ST", V_ASN1_UTF8STRING, pbST, -1, -1, 0);
    X509_NAME_add_entry_by_txt(pX509Name, "C", V_ASN1_UTF8STRING, pbC, -1, -1, 0);
    return pX509Name;
}

X509_NAME* genX509_NAME(){
	char *pbEmail = "abc@qq.com";
	char *pbCN = "abc01";
	char *pbOU = "kaifa";
	char *pbO = "yiyuan";
	char *pbL = "wuxi";
	char *pbST = "jiangsu";
	char *pbC = "CN";


	X509_NAME * xname = CreateDN(pbEmail, pbCN, pbOU, pbO, pbL, pbST, pbC);
	return xname;
}


int GenCSR(char *pbDN, int nDNLen, char *pCSR, size_t nCSRSize)
{
    char szAltName[] = "DNS:www.jinhill.com";
    char szComment[] = "Create by Jinhill";
    char szKeyUsage[] = "digitalSignature, nonRepudiation";
    char szExKeyUsage[] = "serverAuth, clientAuth";
    
    X509_REQ        *pX509Req = NULL;
    int             iRV = 0;
    long            lVer = 3;
    X509_NAME       *pX509DN = NULL;
    EVP_PKEY        *pEVPKey = NULL;
    RSA             *pRSA = NULL;
    X509_NAME_ENTRY *pX509Entry = NULL;
    char            szBuf[255] = {0};
    char            mdout[20];
    int             nLen, nModLen;
    int             bits = 2048;
    unsigned long   E = RSA_3;
    unsigned char   *pDer = NULL;
    unsigned char   *p = NULL;
    FILE            *fp = NULL;
    const EVP_MD    *md = NULL;
    X509            *pX509 = NULL;
    BIO             *pBIO = NULL;
    BIO             *pPemBIO = NULL;
    BUF_MEM         *pBMem = NULL;
    
	
    //STACK_OF(X509_EXTENSION) *pX509Ext;
    
    if(pbDN == NULL)
    {
        return -1;
    }
    //pX509DN = parse_name(pbDN, V_ASN1_UTF8STRING, 0);
    pX509DN = genX509_NAME();
    pX509Req = X509_REQ_new();
    
    iRV = X509_REQ_set_version(pX509Req, 0L);
    
    // subject pX509Name
    iRV = X509_REQ_set_subject_name(pX509Req, pX509DN);
    
    /* pub key */
    pEVPKey = EVP_PKEY_new();
    pRSA = RSA_generate_key(bits, E, NULL, NULL);
    EVP_PKEY_assign_RSA(pEVPKey, pRSA);
    iRV = X509_REQ_set_pubkey(pX509Req, pEVPKey);
    
    /* attribute */
	/*
    strcpy(szBuf, szAltName);
    nLen = strlen(szBuf);
    iRV = X509_REQ_add1_attr_by_txt(pX509Req, "subjectAltName", V_ASN1_UTF8STRING, szBuf, nLen);
    
    strcpy(szBuf, szKeyUsage);
    nLen = strlen(szBuf);
    iRV = X509_REQ_add1_attr_by_txt(pX509Req, "keyUsage", V_ASN1_UTF8STRING, szBuf, nLen);
    
    
    strcpy(szBuf, szExKeyUsage);
    nLen = strlen(szBuf);
    iRV = X509_REQ_add1_attr_by_txt(pX509Req, "extendedKeyUsage", V_ASN1_UTF8STRING, szBuf, nLen);
    
    
    strcpy(szBuf, szComment);
    nLen = strlen(szBuf);
    iRV = X509_REQ_add1_attr_by_txt(pX509Req, "nsComment", V_ASN1_UTF8STRING, szBuf, nLen);
    */
    
    md = EVP_sha1();
    iRV = X509_REQ_digest(pX509Req, md, mdout, &nModLen);
    iRV = X509_REQ_sign(pX509Req, pEVPKey, md);
    if(!iRV)
    {
        printf("sign err!\n");
        X509_REQ_free(pX509Req);
        return -1;
    }
    
     //写入文件PEM格式
      pBIO = BIO_new_file("certreq.req", "w");
      PEM_write_bio_X509_REQ(pBIO, pX509Req, NULL, NULL);
      BIO_free(pBIO);
    
    //返回PEM字符
    pPemBIO = BIO_new(BIO_s_mem());
    PEM_write_bio_X509_REQ(pPemBIO, pX509Req, NULL, NULL);
    BIO_get_mem_ptr(pPemBIO,&pBMem);
    if(pBMem->length <= nCSRSize)
    {
        memcpy(pCSR, pBMem->data, pBMem->length);
    }
    BIO_free(pPemBIO);
    
    /* DER编码 */
    //nLen = i2d_X509_REQ(pX509Req, NULL);
    //pDer = (unsigned char *)malloc(nLen);
    //p = pDer;
    //nLen = i2d_X509_REQ(pX509Req, &p);
    //free(pDer);
    
    //  验证CSR
    OpenSSL_add_all_algorithms();
    iRV = X509_REQ_verify(pX509Req, pEVPKey);
    if(iRV<0)
    {
        printf("verify err.\n");
    }
    
    X509_REQ_free(pX509Req);
    return nCSRSize;
}


nt test(){
	char pbDN[255] = "/CN=www.jinhill.com/O=Beijing Jinhill Inc./C=CN";
	int nDNLen = 255;
	char pCSR[4096]={0};
	size_t nCSRSize = 4096;
	
	GenCSR(pbDN, nDNLen, pCSR, nCSRSize);

	return 1;
}