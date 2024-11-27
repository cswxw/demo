#include <stdio.h>

#include <libxml/parser.h>

#include <libxml/tree.h>

#pragma comment(lib,"WS2_32.lib")

#include <iostream>
using namespace std;

// require zlib-1.2.5  libxml2-2.7.8.win32  iconv-1.9.2.win32

/*
生成的数据:
<?xml version="1.0"?>
<root>
	<newNode1>newNode1 content</newNode1>
	<newNode2>newNode2 content</newNode2>
	<newNode3>newNode3 content</newNode3>
	<node2 attribute="yes">NODE CONTENT</node2>
	<son>
		<grandson>This is a grandson node</grandson>
	</son>
</root>

*/
int main(int argc ,char *argv[])
{

    //定义文档和节点指针

    xmlDocPtr doc = xmlNewDoc(BAD_CAST"1.0");

    xmlNodePtr root_node = xmlNewNode(NULL,BAD_CAST"root");

    //设置根节点

    xmlDocSetRootElement(doc,root_node);

    //在根节点中直接创建节点

    xmlNewTextChild(root_node, NULL, BAD_CAST "newNode1", BAD_CAST "newNode1 content");

    xmlNewTextChild(root_node, NULL, BAD_CAST "newNode2", BAD_CAST "newNode2 content");

    xmlNewTextChild(root_node, NULL, BAD_CAST "newNode3", BAD_CAST "newNode3 content");

    //创建一个节点，设置其内容和属性，然后加入根结点

    xmlNodePtr node = xmlNewNode(NULL,BAD_CAST"node2");

    xmlNodePtr content = xmlNewText(BAD_CAST"NODE CONTENT");

    xmlAddChild(root_node,node);

    xmlAddChild(node,content);

    xmlNewProp(node,BAD_CAST"attribute",BAD_CAST "yes");

    //创建一个儿子和孙子节点

    node = xmlNewNode(NULL, BAD_CAST "son");

    xmlAddChild(root_node,node);

    xmlNodePtr grandson = xmlNewNode(NULL, BAD_CAST "grandson");

    xmlAddChild(node,grandson);

    xmlAddChild(grandson, xmlNewText(BAD_CAST "This is a grandson node"));

    //存储xml文档

    int nRel = xmlSaveFile("CreatedXml.xml",doc);

    if (nRel != -1)

    {

       cout<<"一个xml文档被创建,写入"<<nRel<<"个字节"<<endl;

    }
	xmlChar *outbuf;
	int outlen;

	/*
	out_doc：需要输出成为一个buffer的XML文档
	doc_txt_ptr：输出文档的内存区。由该函数在内部申请。使用完成之后，必须调用xmlFree()函数来释放该内存块
	doc_txt_len：输出文档内存区的长度
	txt_encoding：输出文档的编码类型
	format：是否格式化。0表示不格式化，1表示需要格式化。注意只有当xmlIndentTreeOutput设置为1，或者xmlKeepBlanksDefault(0)时，format设置为1才能生效
	*/
	xmlDocDumpFormatMemoryEnc(doc,&outbuf,&outlen,"gb2312", 1);
	
	puts((char*)outbuf);
	xmlFree(outbuf);

	//printf("%s",buf)
    //释放文档内节点动态申请的内存

    xmlFreeDoc(doc);

    return 1;

}
