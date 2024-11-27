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
  
    // 注意此处加载证书链中证书的顺序没有要求，因为  
    // 在X509_verify_cert()函数中会对证书链中的证书  
    // 进行排序  
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
  
    //创建X509_store对象，用来存储证书、撤销列表等  
    store=X509_STORE_new();  
  
    // 载入叶子证书  
    m_leaf = load_certfile(certFile);  
  
    //设置验证标记 都验证那些项 X509_V_FLAG_CRL_CHECK_ALL表示全部验证  
    X509_STORE_set_flags(store,X509_V_FLAG_CRL_CHECK_ALL);  
    //初始化CTX 这个类就是所谓的上下文 该类收集完必要的信息数据 可以进行验证  
    // 此处X509_STORE_CTX_init最后一个参数为NULL，表示不加载证书撤销列表CPL  
    if(!X509_STORE_CTX_init(&ctx,store ,m_leaf,NULL))  
    {  
        ret = 0;  
        goto end;  
    }  
  
    if(m_chain == NULL)  
    {  
        cout<<"加载证书链失败!\n"<<endl;  
        ret = 0;  
        goto end;  
    }  
    else  
    {  
        //将证书链存入CTX  
        X509_STORE_CTX_trusted_stack(&ctx, m_chain);  
    }  
  
    //证书链式验证  
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
    //将IO中数据以PEM格式读入到X509对象  
    cert = PEM_read_bio_X509(in,NULL,NULL,NULL);  
    if(cert == NULL)  
        goto end;  
end:  
    if(in)BIO_free(in);  
    return cert;  
}  
  