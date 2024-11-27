/************************************************************************/  
/*                                 main.cpp                             */  
/************************************************************************/  
#include "main.h"
#include "VerifyDCChain.h"  
#include <iostream>  
using namespace std;  
  
void main(void)  
{  
    VerifyDCChain m_check;  
  
    // 注意此处加载证书链中证书文件名的顺序没有要求，  
    // 因为在X509_verify_cert()函数中会对证书链中的  
    // 证书进行排序  
    string certChains[4] = {"ca.crt"};  //, "4.crt", "3.crt", "2.crt"
    m_check.Init(certChains, 1);  
  
    if (1 == m_check.verify("server.crt"))  
    {  
        cout<<"OK!"<<endl;  
    }  
    else  
    {  
        cout<<"ERROR!"<<endl;  
    }     
}  