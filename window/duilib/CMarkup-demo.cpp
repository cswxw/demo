#include "../Test/Markup.h"
#include <stdio.h>
#include <iostream>
#include <string>
using namespace std;


#define _CREATE
//#define _ADD
//#define _DELETE
//#define _UPDATE
//#define _READ

void print(CMarkup * xml){
	printf("-------------[start]------------------------\n");
	string strXML = xml->GetDoc();//读文件内容到字符串 (MCD_STR or std::string or CString)
    cout << strXML ;
	printf("-------------[end]------------------------\n");
}
int main(){
	CMarkup xml;
#ifdef _CREATE
    //创建一个 XML Document
    
    xml.AddElem("ORDER");//unicode 需要加: _T()或_TEXT
    xml.IntoElem();
    xml.AddElem("ITEM");
    xml.IntoElem();
    xml.AddElem("SN", "132487A-J");
    xml.AddElem("NAME", "crank casing");
    xml.AddElem("QTY", "1");
    xml.Save("d:\\UserInfo.xml");//保存到文件
    //-------------------------
    print(&xml);
#endif
#ifdef _ADD
    //增加元素和属性
    //添加在最后面, 使用的是AddElem; 添加在最前面, 使用InsertElem。
   // CMarkup xml;
	xml
    if (!xml.Load("d:\\UserInfo.xml"));
    //  return 1;
    xml.AddElem("ORDER");
    xml.IntoElem(); // 进入 ORDER

    xml.AddElem("ITEM");
    xml.IntoElem(); // 进入 ITEM
    xml.AddElem("SN", "4238764-A"); //添加元素
    xml.AddElem("NAME", "bearing");//添加元素
    xml.AddElem("QTY", "15");//添加元素
    xml.OutOfElem(); // 退出 ITEM 
    xml.AddElem("SHIPMENT");
    xml.IntoElem(); // 进入 SHIPMENT
    xml.AddElem("POC");//添加元素
    xml.SetAttrib("type", "non-emergency");//添加属性
    xml.IntoElem(); // 进入 POC
    xml.AddElem("NAME", "John Smith");//添加元素
    xml.AddElem("TEL", "555-1234");//添加元素
    xml.Save("d:\\UserInfo.xml");
	print(&xml);
#endif
#ifdef _DELETE
    //删除元素： 删除SN = 132487A - J的项目。
    //CMarkup xml;
    xml.Load("d:\\UserInfo.xml");
    string strUserID = _T("");
    xml.ResetMainPos();
    if (xml.FindChildElem("ITEM"))
    {
        xml.IntoElem();
        string str_sn;
        xml.FindChildElem("SN");
        str_sn = xml.GetChildData();
        if (str_sn == "132487A-J")
        {
            xml.RemoveElem();
            xml.Save("d:\\UserInfo.xml");
        }
    }
	print(&xml);
#endif
#ifdef _UPDATE
    //修改元素和属性---------
   // CMarkup xml;
    if (xml.Load("d:\\UserInfo.xml"))//格式不正确可能导致load失败
    {
        string strUserID = _T("");
        xml.ResetMainPos();
        if (xml.FindChildElem("SHIPMENT"))
        {
            xml.IntoElem();
            if (xml.FindChildElem("POC"))
            {
                xml.IntoElem();
                string str_type = xml.GetAttrib("type");

                xml.SetAttrib("type", "change");
                strUserID = xml.GetData();

                if (xml.FindChildElem("TEL"))
                {
                    xml.IntoElem();
                    xml.SetData("99999999");
                    xml.Save("d:\\UserInfo.xml");
                    return 0;
                }
            }
        }
    }
	print(&xml);
#endif
#ifdef _READ
    //查找元素
    //CMarkup xml;
    xml.Load("d:\\UserInfo.xml");//From a file with Load 
    //xml.SetDoc(strXML);//Or from an XML string with SetDoc
    xml.FindElem(); // root ORDER element
    xml.IntoElem(); // inside ORDER
    while (xml.FindElem("ITEM"))
    {
        xml.IntoElem();
        xml.FindElem("SN");
        MCD_STR strSN = xml.GetData();
        cout << strSN << endl;
        xml.FindElem("QTY");
        int nQty = atoi(MCD_2PCSZ(xml.GetData()));//MCD_2PCSZ is defined in Markup.h to return the string's const pointer.
        cout << nQty << endl;
        xml.OutOfElem();
    }
    //查找元素1
    xml.IntoElem();
    xml.FindElem("SN");
    MCD_STR strSN = xml.GetData();
    xml.ResetMainPos();
    xml.FindElem("QTY");
    int nQty = atoi(MCD_2PCSZ(xml.GetData()));
    cout << nQty << endl;
    xml.OutOfElem();
    //查找元素2
    xml.ResetPos(); // top of document
    xml.FindElem(); // ORDER element is root
    xml.IntoElem(); // inside ORDER
    while (xml.FindElem("ITEM"))
    {
        xml.FindChildElem("SN");
        if (xml.GetChildData() == "4238764-A"){
            cout << "Found!" << endl;
            break; // found
        }
        else{
            cout << "Not Found!" << endl;
        }
    }
	print(&xml);
#endif  
    return 0;

}
