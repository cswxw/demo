#include "Smtp.h"
#include <iostream>
using namespace std;

int main()
{

	CSmtp smtp(
		465,								/*smtp�˿�*/
		"smtp.qq.com",					/*smtp��������ַ*/
		"xxx.com",	/*��������ַ*/
		"xxxxxxx",								/*��������*/
		"xxxxxx.com",	/*Ŀ�������ַ*/
		"",							/*����*/
		""		/*�ʼ�����*/
	);
	string str = "title";
	smtp.SetEmailTitle(str);
	str = "this is a test!";
	smtp.SetContent(str);

	/**
	//��Ӹ���ʱע��,\һ��Ҫд��\\����Ϊת���ַ���Ե��
	string filePath("D:\\�γ���Ʊ���.doc");
	smtp.AddAttachment(filePath);
	*/

	
	//filePath = "C:\\Users\\��ܲ��\\Desktop\\sendEmail.cpp";
	//smtp.AddAttachment(filePath);
	smtp.isUseSSL = 1;
	int err;
	if ((err = smtp.SendEmail_Ex()) != 0)
	{
		if (err == 1)
			cout << "����1: �������粻��ͨ������ʧ��!" << endl;
		if (err == 2)
			cout << "����2: �û�������,��˶�!" << endl;
		if (err == 3)
			cout << "����3: �û����������˶�!" << endl;
		if (err == 4)
			cout << "����4: ���鸽��Ŀ¼�Ƿ���ȷ���Լ��ļ��Ƿ����!" << endl;
	}
	system("pause");
	return 0;
}