#include "Smtp.h"
#include <iostream>
using namespace std;

int main()
{

	CSmtp smtp(
		25,								/*smtp�˿�*/
		"mail.xxxxxxx.net.cn",					/*smtp��������ַ*/
		"xxxxxxx@ideabank.net.cn",	/*��������ַ*/
		"xxxxxxx,.",								/*��������*/
		"xxxxxxx@deabank.net.cn",	/*Ŀ�������ַ*/
		"subject",							/*����*/
		"this is a test!"		/*�ʼ�����*/
	);
	/**
	//��Ӹ���ʱע��,\һ��Ҫд��\\����Ϊת���ַ���Ե��
	string filePath("D:\\�γ���Ʊ���.doc");
	smtp.AddAttachment(filePath);
	*/

	/*�����Ե���CSmtp::DeleteAttachment����ɾ������������һЩ�������Լ���ͷ�ļ���!*/
	//filePath = "C:\\Users\\��ܲ��\\Desktop\\sendEmail.cpp";
	//smtp.AddAttachment(filePath);

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
