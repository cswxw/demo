#ifndef _ENVELOPE_H_
#define _ENVELOPE_H_
#define SIGNATURE  0x011e02f2
typedef unsigned int DWORD;
#define MAX_PATH 260
struct SignFile{
	DWORD PointerToRawData;  //签名文件在文件中偏移
	DWORD filesize;         //签名文件大小
	char  fileName[MAX_PATH]; //文件名
}__attribute__ ((packed));
struct DataFile{
	DWORD  PointerToRawData;  //数据文件在文件中偏移
	DWORD filesize;         //数据文件大小
	char  fileName[MAX_PATH];  //文件名
}__attribute__ ((packed));

struct DataSignFile{
	SignFile dataFile;  //数据文件偏移
	DataFile signFile;  
};
struct Env_Header{
	DWORD Signature; //文件标记
	DWORD count;     //数据和签名文件对
	DWORD PointerToDataSignFile; //数据和签名文件结构体偏移  ignore
}__attribute__ ((packed));


#endif
