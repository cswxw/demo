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
    * ��ʼ��֤������ջm_chain 
    * @param[in] certChains ֤�����и���֤���ļ������� 
    * @param[in] num ֤������֤����� 
    */  
    int Init(const string* certChains, const int num);  
  
    /* 
    * ʹ�ø�����֤������֤Ҷ��֤�� 
    * @param[in] certFile ��Ҫ��֤��Ҷ��֤���ļ��� 
    */  
    int verify(const char* certFile);  
private:  
  
    /* 
    * ����֤���ļ� 
    * @param[in] certFile ��Ҫ���ص�֤���ļ��� 
    */  
    X509* load_certfile(const char* certFile);  
private:  
    X509* m_leaf;  
    STACK_OF(X509)* m_chain;      
};  
  
#endif  