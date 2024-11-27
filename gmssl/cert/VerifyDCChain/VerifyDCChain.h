/************************************************************************/  
/*                          VerifyDCChain.h                             */  
/************************************************************************/  
#ifndef VERIFYDCCHAIN_H_  
#define VERIFYDCCHAIN_H_  
 
#include <openssl/bio.h>  
#include <openssl/err.h>  
#include <openssl/x509.h>  
#include <openssl/x509v3.h>  
#include <openssl/pem.h>  
#include <openssl/crypto.h> 
#include <string>  
#include <iostream>  
using namespace std;  
  
class VerifyDCChain  
{  
public:  
    VerifyDCChain();  
    ~VerifyDCChain();  
  
    /* 
    * 初始化证书链堆栈m_chain 
    * @param[in] certChains 证书链中各个证书文件名数组 
    * @param[in] num 证书链中证书个数 
    */  
    int Init(const string* certChains, const int num);  
  
    /* 
    * 使用给定的证书链验证叶子证书 
    * @param[in] certFile 需要验证的叶子证书文件名 
    */  
    int verify(const char* certFile);  
private:  
  
    /* 
    * 加载证书文件 
    * @param[in] certFile 需要加载的证书文件名 
    */  
    X509* load_certfile(const char* certFile);  
private:  
    X509* m_leaf;  
    STACK_OF(X509)* m_chain;      
};  
  
#endif  