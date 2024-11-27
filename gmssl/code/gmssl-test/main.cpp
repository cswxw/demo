#include "main.h"
struct option opts[] = {
    {"key",  required_argument, NULL, 'k'},   //private
    {"cert",    required_argument,       NULL, 'c'},  //cert  
    {"in",    required_argument,       NULL, 'i'},   //input file
	{"signfile",    required_argument,       NULL, 'f'},   //signfile file
    {"sign", no_argument,       NULL, 's'},    // to sign file 
    {"verfity", no_argument,       NULL, 'v'},    //to verfity file 
    {"pack", no_argument,       NULL, 'p'},    // pack envelope file
    {"unpack", no_argument,       NULL, 'u'},    //unpack envelope file 
    {NULL, 0, NULL, 0}  
};
void printHelp(){
	printf("Usage:\n");
	printf(" -k --key        private key file\n");
	printf(" -c --cert       cert file\n");
	printf(" -i --in         input file\n");
	printf(" -f --signfile   sign file\n");
	printf(" -s --sign       to sign file\n");
	printf(" -v --verfity    to verfity file\n");
	printf(" -p --pack       to pack input file to outputfile\n");
	printf(" -u --unpack     to unpack input file to outputdir\n");

}
UserOption user_options={0};
int getRealPath(char * in,char * out){
	if(in == NULL || out == NULL) return -1;
	if(realpath(in, out)){
		//printf("%s %s\n", in, out);
		return 1;
	}
	else{
		__dbg_printf("the file '%s' is not exist\n", in);    
		return -1;
	}
}
int isrFile(char *path)
{
	struct stat buf;
	int cc;
	int euid,egid;
	if(path == NULL) return -1;
	cc=stat(path,&buf);
	if(!cc) {
		
		if((buf.st_mode & S_IFMT) != S_IFREG) return 0;
		euid=geteuid(); 
		egid=getegid(); 
		if(euid==0) {
			if(buf.st_mode & S_IRUSR || buf.st_mode & S_IRGRP ||
			   buf.st_mode & S_IROTH)
				 return 1;
			else return 0;
		}
		if((buf.st_mode & S_IROTH)!=0) return 1;   //S_IROTH group has read permission
		if((buf.st_gid == egid) && ((buf.st_mode & S_IRGRP)!=0))   //S_IRGRP group has read permission 
				return 1;
		if((buf.st_uid == euid) && ((buf.st_mode & S_IRUSR)!=0))   //S_IRUSR owner has read permission
				return 1;

	}
	return cc;
}
int isDir(const char* dir_path){
	if(dir_path == NULL) return -1;
	DIR * dirp =opendir(dir_path);
	if(dirp == NULL) return -1;
	closedir(dirp);
	return 1;
}

int parsePath(UserOption* user){
	char temp[MAX_PATH];
	if(user->inPath !=NULL && strlen(user->inPath)> 0){
		strcpy(temp,user->inPath);
		getRealPath(temp,user->inPath);	
	}
	
	if(user->outPath !=NULL && strlen(user->outPath)> 0){
		strcpy(temp,user->outPath);
		getRealPath(temp,user->outPath);	
	}
	if(user->signPath !=NULL && strlen(user->signPath)> 0){
		strcpy(temp,user->signPath);
		getRealPath(temp,user->signPath);	
	}
	return 0;
}

int check_options(UserOption* user){
	if(user == NULL ) return -1;
	
	//privatekey or  cert
	if(isrFile(user->privateKeyPath) <= 0){
		__dbg_printf("[args] -k privatekey file path is invalid\n");
		return -1;
		
	}
	if(isrFile(user->certPath) <= 0){
		__dbg_printf("[args] -c cert file path is invalid\n");
		return -1;		
	}
	//isSign 	//isVerfity //isEnvelopeEncode //isEnvelopeDecode
	if(user->isSign == 1 || user->isEnvelopeEncode == 1 || user->isEnvelopeDecode == 1 || user->isVerfity == 1){
		if(isrFile(user->inPath) > 0){
			if(user->isEnvelopeDecode == 1){
				if(isDir(user->outPath) > 0){
					return 1;
				}
			}
			if(user->isVerfity == 1){
				if(isrFile(user->signPath) > 0 ){
					return 1;
				}else{
					__dbg_printf("[args] sign file path is invalid\n");
					return -1;				
				}
			}
			return 1;
		}else{
			__dbg_printf("[args] input file path is invalid\n");
			return -1;
		}
	}else{
		printf("[warning]nothing to do\n");
		return -1;
	}
}
unsigned int get_file_size(const char *path)  
{  
    unsigned long filesize = -1;  
    FILE *fp;  
    fp = fopen(path, "r");  
    if(fp == NULL)  
        return filesize;  
    fseek(fp, 0L, SEEK_END);  
    filesize = ftell(fp);  
    fclose(fp);  
    return (unsigned int)filesize;
}  
int  work(UserOption *user,CCert* obj){
	int ret=0;
	//sign
	if(user->isSign == 1){
		unsigned char filehash[EVP_MAX_MD_SIZE] ={0};
		unsigned int md_len=0;
		unsigned char encodefilehash[512] ={0};
		size_t encodelen=sizeof(encodefilehash);
		__dbg_printf("[start] get file sm3.\n");
		ret = obj->getFilesm3(user->inPath,filehash,&md_len);
		if(ret < 0){
			__dbg_printf("getFilesm3 failed\n");
			return -1;
		}
		__dbg_printf("[end] get file sm3.\n");
		__dbg_printf("[start]use public key to sign sm3 hash\n");
		ret = obj->rsa_key_encrypt(filehash,sizeof(filehash),encodefilehash,encodelen);   //加密结果128位
		if(ret < 0 ){
			__dbg_printf("[failed]use public key to sign sm3 hash\n");
			return -1;
		}
		strcpy(user->outPath,user->inPath);
		strcat(user->outPath,".sig");
		FILE *fp = fopen(user->outPath,"wb+");
		fwrite(encodefilehash,sizeof(char),encodelen,fp);
		fclose(fp);
		__dbg_printf("[end]use public key to sign sm3 hash\n");
		__dbg_printf("[info] sign file path:%s\n",user->outPath);
		return 1;
	}
	//verfity
	else if(user->isVerfity == 1){
		//read sign file
		unsigned char encodefilehash[512] ={0};
		size_t encodelen=sizeof(encodefilehash);
		FILE *signfp = fopen(user->signPath,"rb");
		if(signfp == NULL){
			__dbg_printf("open sign file failed!\n");
			return -1;
		}
		size_t nlen = fread(encodefilehash,sizeof(char),sizeof(encodefilehash),signfp);
		fclose(signfp);
		signfp = NULL;
		
		//use private key to decode sign file.
		unsigned char decodefilehash[512] ={0};
		size_t decodelen=sizeof(decodefilehash);
		__dbg_printf("[start] use private key to decode sign file.\n");
		ret = obj->rsa_key_decrypt(encodefilehash,nlen,decodefilehash,decodelen,NULL);
		if(ret < 0 ){
			__dbg_printf("[failed] use private key to decode sign file.");
			return -1;
		}
		__dbg_printf("[end] use private key to decode sign file.\n");
		unsigned char filehash[EVP_MAX_MD_SIZE] ={0};
		unsigned int md_len=0;
		__dbg_printf("[start] get file sm3.\n");
		ret = obj->getFilesm3(user->inPath,filehash,&md_len);
		if(ret < 0){
			__dbg_printf("getFilesm3 failed\n");
			return -1;
		}
		__dbg_printf("[end] get file sm3.\n");
		
		//check sign equal?
		__dbg_printf("[start]check if the signatures are equal\n");
		ret = ::memcmp(decodefilehash,filehash,md_len);
		if(ret !=0 ){
			__dbg_printf("\t[failed] sign not match!\n");
		}else{
			__dbg_printf("\t[success] sign is match!\n");
		}
		__dbg_printf("[end]check if the signatures are equal\n");
		return 1;
	}
	//isEnvelopeEncode
	else if(user->isEnvelopeEncode == 1){
		unsigned char filehash[EVP_MAX_MD_SIZE] ={0};
		unsigned int md_len=0;
		
		//sm3
		__dbg_printf("[start] get file sm3.\n");
		ret = obj->getFilesm3(user->inPath,filehash,&md_len);
		if(ret < 0){
			__dbg_printf("getFilesm3 failed\n");
			return -1;
		}
		__dbg_printf("[end] get file sm3.\n");

		//sm4
		__dbg_printf("[start] use sm4 to encode file.\n");
		char *filepath = user->inPath;
		char *encfilepath = user->outPath;
		strcpy(encfilepath,filepath);
		strcat(encfilepath,".encode");
		ret = obj->sm4_do_crypt(filepath,encfilepath,filehash,1);
		if( ret < 0 ){
			__dbg_printf("[failed] use sm4 to encode file.");
			return -1;
		}
		__dbg_printf("[end] use sm4 to encode file.\n");
		
		//使用公钥对摘要进行签名
		unsigned char encodefilehash[512] ={0};
		size_t encodelen=sizeof(encodefilehash);
		__dbg_printf("[start] use public key to encode sm4 key.\n");
		ret = obj->rsa_key_encrypt(filehash,sizeof(filehash),encodefilehash,encodelen);   //加密结果128位
		if(ret < 0 ){
			__dbg_printf("[failed] use public key to encode sm4 key.\n");
			return -1;
		}
		__dbg_printf("[end] use public key to encode sm4 key.\n");
		char enckeypath[MAX_PATH]={0};
		strcpy(enckeypath,filepath);
		strcat(enckeypath,".sigkey");
		
		//生成签名文件
		FILE* signfp=fopen(enckeypath,"wb");
		if(signfp == NULL ){
			__dbg_printf("create sign file failed!\n");
			return -1;
		}
		fwrite(encodefilehash,sizeof(char),encodelen,signfp);//128字节
		fclose(signfp);	
		signfp = NULL;
		
		/***********************************封装信封************************************************/
		//获取数据文件大小 和 签名文件大小
		DWORD dataSize = get_file_size(encfilepath);
		if(dataSize == 0){
			__dbg_printf("[err] 获取数据文件大小失败\n");
			return -1;
		}
		DWORD signSize = get_file_size(enckeypath);
		if(signSize == 0){
			__dbg_printf("[err] 获取签名文件大小失败\n");
			return -1;
		}
		
		Env_Header header;
		memset(&header,'\0',sizeof(header));
		header.Signature = SIGNATURE;
		header.count = 1;
		header.PointerToDataSignFile = sizeof(Env_Header);
		char envelopeFile[MAX_PATH];
		strcpy(envelopeFile,filepath);
		strcat(envelopeFile,".envelope");
		__dbg_printf("[info] create envelopeFile\n");
		FILE* hFile=fopen(envelopeFile,"wb");
		if(hFile == NULL){
			__dbg_printf("[err] create envelopeFile\n");
			return -1;
		}
		DWORD dwWrite=0;
		__dbg_printf("[info] write header to envelopeFile\n");
		if(!fwrite((const void *)&header,1,sizeof(header),hFile))
		{  
			__dbg_printf("[err] fwrite error\n");  
			fclose(hFile);
			return -1;  
		} 
		DataSignFile datasignFile;
		memset(&datasignFile,'\0',sizeof(datasignFile));
		//dataFile
		strcpy(datasignFile.dataFile.fileName,strrchr(filepath,'/')+1);
		datasignFile.dataFile.filesize = dataSize;
		datasignFile.dataFile.PointerToRawData = 0;

		//signFile
		strcpy(datasignFile.signFile.fileName,strrchr(enckeypath,'/')+1);
		datasignFile.signFile.filesize = signSize;
		datasignFile.signFile.PointerToRawData = 0;
		
		if(!fwrite((void *)&datasignFile,1,sizeof(datasignFile),hFile))
		{  
			__dbg_printf("[err] fwrite error\n"); 
			fclose(hFile);
			return -1;  
		}
		
		DWORD dataOffert=(DWORD)ftell(hFile);
		char databuff[1024];
		DWORD dwRead=0;
		//打开数据文件写入信封
		__dbg_printf("[info] write encode data file to envelope file\n");  
		FILE* hdataFile=fopen(encfilepath,"rb");
		if(hdataFile == NULL){
			__dbg_printf("[err] open encode data file error\n"); 
			fclose(hFile);
			return -1;
		}
		while((ret = fread(databuff,1,sizeof(databuff),hdataFile)) > 0 ){
			fwrite((void *)databuff,1,ret,hFile);
		}
		fclose(hdataFile);

		DWORD signOffert=(DWORD)ftell(hFile);
		
		//打开加密的密钥文件写入信封
		__dbg_printf("[info] write encode key file to envelope file\n");  
		FILE* hsignFile=fopen(enckeypath,"rb");
		if(hsignFile == NULL){
			__dbg_printf("[err] open encode sign file error\n"); 
			fclose(hFile);
			return -1;
		}
		while((ret = fread(databuff,1,sizeof(databuff),hsignFile)) > 0 ){
			fwrite((void *)databuff,1,ret,hFile);
		}
		fclose(hsignFile);

		datasignFile.dataFile.PointerToRawData = dataOffert;
		datasignFile.signFile.PointerToRawData = signOffert;
		//写入data偏移
		__dbg_printf("[info] update envelope file result\n");  
		fseek(hFile,header.PointerToDataSignFile + offsetof(DataSignFile,dataFile.PointerToRawData),SEEK_SET);
		fwrite((void*)&dataOffert,1,4,hFile);


		//写入sign偏移
		fseek(hFile, header.PointerToDataSignFile + offsetof(DataSignFile,signFile.PointerToRawData),SEEK_SET);
		fwrite((void*)&signOffert,1,4,hFile);
		fclose(hFile);
		__dbg_printf("[end] envelope file completed!\n");  
		__dbg_printf("[info] envelope file path: %s \n",envelopeFile);

		//clear

		//__dbg_printf("encfilepath:%s\n",encfilepath);
		//__dbg_printf("enckeypath:%s\n",enckeypath);
		remove(encfilepath);
		remove(enckeypath);
		
		/*******************************************************************************************/
		
		return 1;
	}
	//isEnvelopeDecode
	else if(user->isEnvelopeDecode == 1){
		char * saveDir = user->outPath;
		__dbg_printf("[info] open envelope file\n");
		int fd = open(user->inPath,O_RDONLY); 
		if(fd < 0 ){
			__dbg_printf("[err] open envelope file\n");
			return -1;
		}
		DWORD filelen = get_file_size(user->inPath);
		filelen = alignment_up(filelen,sizeof(long));

		char *pFile =(char*) mmap(NULL,filelen,PROT_READ,MAP_SHARED,fd,0);
		
		if(pFile == NULL){
			__dbg_printf("[err] mmap failed\n");
			return -1;
		}
		Env_Header * header = (Env_Header*)pFile;
		if(header->Signature != SIGNATURE){
			__dbg_printf("[err] input file is not envelope file");
			munmap(pFile,filelen); 
			close(fd);
			return -1;
		}
		__dbg_printf("[info] header->Signature:%X\n",header->Signature);
		__dbg_printf("[info] header->count:%d\n",header->count);
		__dbg_printf("[info] header->PointerToDataSignFile:%X\n",header->PointerToDataSignFile);
		
		DataSignFile* datasignFile = (DataSignFile*)(header->PointerToDataSignFile + pFile);

		char originFilePath[MAX_PATH];
		char encodeFilePath[MAX_PATH];


		DWORD dwWrite=0;
		for(DWORD i=0;i<header->count;i++){
			int ret = 0;
			__dbg_printf("[info] paring envelope file\n");
			//提取加密文件
			sprintf(originFilePath,"%s/%s",saveDir,datasignFile->dataFile.fileName);
			sprintf(encodeFilePath,"%s/%s.encode",saveDir,datasignFile->dataFile.fileName);

			FILE* hEncodeFile=fopen(encodeFilePath,"wb");
			if(hEncodeFile == NULL){	
				__dbg_printf("[err] fopen failed\n");
				fclose(hEncodeFile);
				break;
			}
			if(!fwrite((void*)(pFile+datasignFile->dataFile.PointerToRawData),1,datasignFile->dataFile.filesize,hEncodeFile)){
				__dbg_printf("[err] WriteFile error\n");  
				fclose(hEncodeFile);
				break;
			}
			//close
			fclose(hEncodeFile);
			unsigned char *encodefilehash = (unsigned char *)calloc(datasignFile->signFile.filesize,sizeof(char));
			
			//提取加密密钥
			memcpy(encodefilehash,pFile+datasignFile->signFile.PointerToRawData,datasignFile->signFile.filesize);

			
			//使用私钥解密密钥
			unsigned char decodefilehash[512] ={0};
			size_t decodelen=sizeof(decodefilehash);
			__dbg_printf("[info] use private key to decode sm4 key\n");
			ret = obj->rsa_key_decrypt(encodefilehash,datasignFile->signFile.filesize,decodefilehash,decodelen,NULL);
			if(ret < 0 ){
				__dbg_printf("[failed] use private key to decode sm4 key\n");
				free(encodefilehash);
				break;
			}
			
			//使用解密的密钥解密文件
			__dbg_printf("[start] use sm4 to decode data file\n");
		
			ret = obj->sm4_do_crypt(encodeFilePath,originFilePath,decodefilehash,0);
			if( ret < 0 ){
				__dbg_printf("[failed] use sm4 to decode data file\n");
				free(encodefilehash);
				break;
			}
			__dbg_printf("[end] use sm4 to decode data file\n");

			if(i+1 == header->count){
				//MessageBox(TEXT("解析信封结束"));
			}
			
			
			remove(encodeFilePath);
			free(encodefilehash);
			
		}
		__dbg_printf("[info] decode envelope file completed\n");
		//clear
		munmap(pFile,filelen); 
		close(fd);
	}
	return 0;
}



int main(int argc, char **argv) {
    char *configfile = NULL;
    int opt = 0; 
	if(argc < 2){
		printHelp();
		return -1;
	}
    //while ((opt = getopt(argc, argv, "f:hv")) != -1) {
    while ((opt = getopt_long(argc, argv, "k:c:i:o:f:svpu", opts, NULL)) != -1) {
		printf("opt:%c\n",opt);
		if(optarg != NULL) printf("\t[optarg]: %s\n",optarg);
        switch (opt) {
        case 'k': 
            strncpy(user_options.privateKeyPath,optarg,MAX_PATH -1);
            break;
        case 'c': 
			strncpy(user_options.certPath,optarg,MAX_PATH -1);
			break;
        case 'i': 
			strncpy(user_options.inPath,optarg,MAX_PATH -1);
			break;
        case 'o': 
			strncpy(user_options.outPath,optarg,MAX_PATH -1);
			break;
		case 'f':
			strncpy(user_options.signPath,optarg,MAX_PATH -1);
			break;
        case 's': 
			user_options.isSign = 1;
			break;
        case 'v': 
			user_options.isVerfity = 1;
			break;
        case 'p': 
			user_options.isEnvelopeEncode = 1;
			break;
        case 'u': 
			user_options.isEnvelopeDecode = 1;
			break;
		case '?':
			printf("unknown arg\n");
			break;
        default:
			printHelp();
            return -1;
        }
    }
	parsePath(&user_options);
	if(check_options(&user_options)<=0) return -1;
	//load private public 
	if(cert.load_private_key(user_options.privateKeyPath,NULL)<0){
		printf("load private key failed!\n");
		return -1;
	}
	if(cert.load_public_cert(user_options.certPath) < 0){
		printf("load public key failed!\n");
		return -1;
	}
	
	
	
	if(work(&user_options,&cert) < 0){
		printf("operation failed!\n");
		return -1;
	}
	
	printf("finished\n");
	
	
    return 0;
}



