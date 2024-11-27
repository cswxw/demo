#ifndef _ENVELOPE_H_
#define _ENVELOPE_H_
#define SIGNATURE  0x011e02f2
typedef unsigned int DWORD;
#define MAX_PATH 260
struct SignFile{
	DWORD PointerToRawData;  //ǩ���ļ����ļ���ƫ��
	DWORD filesize;         //ǩ���ļ���С
	char  fileName[MAX_PATH]; //�ļ���
}__attribute__ ((packed));
struct DataFile{
	DWORD  PointerToRawData;  //�����ļ����ļ���ƫ��
	DWORD filesize;         //�����ļ���С
	char  fileName[MAX_PATH];  //�ļ���
}__attribute__ ((packed));

struct DataSignFile{
	SignFile dataFile;  //�����ļ�ƫ��
	DataFile signFile;  
};
struct Env_Header{
	DWORD Signature; //�ļ����
	DWORD count;     //���ݺ�ǩ���ļ���
	DWORD PointerToDataSignFile; //���ݺ�ǩ���ļ��ṹ��ƫ��  ignore
}__attribute__ ((packed));


#endif
