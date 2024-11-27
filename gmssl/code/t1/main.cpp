#include "Cert.h"


int main(int argc,char *argv[]){
	char dataFile[]="d:\\systemp\\123.p";
	char encodeFile[]="d:\\systemp\\123.p.encode";
	char decodeFile[]="d:\\systemp\\123.p.decode.pdf";
	CCert obj;
	unsigned char filehash[EVP_MAX_MD_SIZE] ={0};
	unsigned char encodefilehash[512] ={0};
	size_t encodelen=sizeof(encodefilehash);
	unsigned char decodefilehash[512] ={0};
	size_t decodelen=sizeof(decodefilehash);
	unsigned int md_len=0;
	//test sm3
	if(obj.getFilesm3(dataFile,filehash,&md_len) >=0){
		for(unsigned int i=0;i<md_len;i++){
			printf("%02x", filehash[i]);
		}
		printf("\n");
	}
	//sm4加密文件
	obj.sm4_do_crypt(dataFile,encodeFile,filehash,1);
	obj.sm4_do_crypt(encodeFile,decodeFile,filehash,0);

	//rsa
	obj.load_private_key("D:\\systemp\\cert\\rsa_private.key",NULL);
	obj.load_public_cert("D:\\systemp\\cert\\cert.crt");
	obj.rsa_key_encrypt(filehash,sizeof(filehash),encodefilehash,encodelen);   //加密结果128位
	obj.rsa_key_decrypt(encodefilehash,encodelen,decodefilehash,decodelen,NULL);
	printf("memcmp : %d\n",memcmp(filehash,decodefilehash,sizeof(filehash)));



	return 0;
}