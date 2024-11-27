/************************************************************************/  
/*                          VerifyDCChain.cpp                           */  
/************************************************************************/  
  
#include "VerifyDCChain.h"  
#include "main.h"
VerifyDCChain::VerifyDCChain():m_leaf(NULL), m_chain(NULL)  
{  
   // CRYPTO_malloc_init();   
    OpenSSL_add_all_algorithms();  
}  
  
VerifyDCChain::~VerifyDCChain()  
{  
    if(m_leaf != NULL)   
    {  
        X509_free(m_leaf);  
    }  
    if (m_chain !=NULL)  
    {  
        sk_X509_free(m_chain);  
    }  
}  
  
int VerifyDCChain::Init(const string* certChains, const int num)  
{  
    int ret = 0;  
    X509* temp = new X509;  
    m_chain = sk_X509_new_null();  
  
    // ע��˴�����֤������֤���˳��û��Ҫ����Ϊ  
    // ��X509_verify_cert()�����л��֤�����е�֤��  
    // ��������  
    for (int i = 0; i < num; i++)  
    {  
        temp = load_certfile(certChains[i].c_str());  
        sk_X509_push(m_chain, temp);  
    }  
    return 1;  
}  
  
int VerifyDCChain::verify(const char* certFile)  
{  
    int ret = 0;  
    X509_STORE *store=NULL;  
    X509_STORE_CTX ctx;  
    m_leaf = new X509();  
  
    //����X509_store���������洢֤�顢�����б��  
    store=X509_STORE_new();  
  
    // ����Ҷ��֤��  
    m_leaf = load_certfile(certFile);  
  
    //������֤��� ����֤��Щ�� X509_V_FLAG_CRL_CHECK_ALL��ʾȫ����֤  
    X509_STORE_set_flags(store,X509_V_FLAG_CRL_CHECK_ALL);  
    //��ʼ��CTX ����������ν�������� �����ռ����Ҫ����Ϣ���� ���Խ�����֤  
    // �˴�X509_STORE_CTX_init���һ������ΪNULL����ʾ������֤�鳷���б�CPL  
    if(!X509_STORE_CTX_init(&ctx,store ,m_leaf,NULL))  
    {  
        ret = 0;  
        goto end;  
    }  
  
    if(m_chain == NULL)  
    {  
        cout<<"����֤����ʧ��!\n"<<endl;  
        ret = 0;  
        goto end;  
    }  
    else  
    {  
        //��֤��������CTX  
        X509_STORE_CTX_trusted_stack(&ctx, m_chain);  
    }  
  
    //֤����ʽ��֤  
    if(1 == X509_verify_cert(&ctx))  
        ret = 1;  
    else  
        ret = 0;  
end:  
    X509_STORE_CTX_cleanup(&ctx);  
    if(store)X509_STORE_free(store);  
    return ret;  
}  
  
X509* VerifyDCChain::load_certfile(const char* certFile)  
{  
    X509* cert = NULL;  
    BIO* in = NULL;  
  
    if(certFile==NULL)  
        goto end;  
    in = BIO_new_file(certFile,"r");  
    if(in==NULL)  
        goto end;  
    //��IO��������PEM��ʽ���뵽X509����  
    cert = PEM_read_bio_X509(in,NULL,NULL,NULL);  
    if(cert == NULL)  
        goto end;  
end:  
    if(in)BIO_free(in);  
    return cert;  
}  
  