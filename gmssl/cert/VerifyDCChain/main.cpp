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
  
    // ע��˴�����֤������֤���ļ�����˳��û��Ҫ��  
    // ��Ϊ��X509_verify_cert()�����л��֤�����е�  
    // ֤���������  
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