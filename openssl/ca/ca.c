/*
 ============================================================================
 Name        : ca.c
 Author      : lsc
 Version     :
 Copyright   : R & D Center of Internet of Things Security
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include "ca.h"

#define HOME "./"
#define CA_REQ  HOME "cafile/ca_req.pem"  //CA证书申请PEM文件存放路径
#define CA_REQ_PRINT  HOME "cafile/ca_req.txt"  //CA证书申请TXT文件存放路径

#define CA_RSA_PRIV  HOME "cafile/ca_rsa_priv.pem"  //CA密钥PEM文件存放路径
#define CA_RSA_PRIV_PRINT  HOME "cafile/ca_rsa_priv.txt"  //CA密钥TXT文件存放路径

#define CA_CERT  HOME "cafile/ca_cert.pem"        //CA自签名证书PEM文件存放路径
#define CA_CERT_PRINT  HOME "cafile/ca_cert.txt"  //CA自签名证书TXT文件存放路径


#define USER_REQ  HOME "userfile/user_req.pem"  //用户证书申请PEM文件存放路径
#define USER_REQ_PRINT  HOME "userfile/user_req.txt"  //用户证书申请TXT文件存放路径

#define USER_RSA_PRIV  HOME "userfile/user_rsa_priv.pem"        //用户密钥PEM文件存放路径
#define USER_RSA_PRIV_PRINT  HOME "userfile/user_rsa_priv.txt"  //用户密钥TXT文件存放路径

#define USER_CERT  HOME "userfile/lvshichao.pem"        //用户证书PEM文件存放路径
#define USER_CERT_PRINT  HOME "userfile/lvshichao.txt"  //用户证书TXT文件存放路径

#define CHK_NULL(x) if ((x)==NULL) exit (1)
#define CHK_ERR(err,s) if ((err)==-1) { perror(s); exit(1); }
#define CHK_SSL(err) if ((err)==-1) { ERR_print_errors_fp(stderr); exit(2); }



static int countcertauthresp=1;  //作用：测试asu运行fill_certificate_auth_resp_packet函数的次数
static int countcertsignresp=1;  //作用：测试asu运行fill_certificate_sign_resp_packet函数的次数




//访问user_table时要使用的信号量
pthread_mutex_t user_table_mutex;




/*************************************************

Function:    // CA_initial
Description: // CA(驻留在asu中)初始化：建立CA目录、文档，生成CA私钥和自签名证书
Calls:       // openssl指令
Called By:   // main()-the first time execution of asu.c;
Input:	     //	无
Output:      //	CA密钥文件、CA自签名证书
Return:      // void
Others:      // 基于c语言调用openssl的shell指令

*************************************************/
void CA_initial()
{
	//create the directory of CA
	system("mkdir -p ./demoCA/newcerts/");
	system("mkdir -p ./demoCA/private/");
	system("touch ./demoCA/index.txt");
	system("echo 01 > ./demoCA/serial");

	//build CA,generate CA's RSA key-pair,does not have password
	system("openssl genrsa -out ./demoCA/private/cakey.pem 1024");
	//generate CA's cert request,and self-signed certificate
	//system("openssl req -new -x509 -days 365 -key ./demoCA/private/cakey.pem -out ./demoCA/cacert.pem");
	system("openssl req -new -x509 -days 365 -key ./demoCA/private/cakey.pem -out ./demoCA/cacert.pem");
}

//void CA_initial()
//{
//	//create the directory of CA
//	printf("***********************************************\n 1) create the directory of CA (demoCA):");
//	system("mkdir -p ./demoCA/newcerts/");
//	system("mkdir -p ./demoCA/private/");
//	system("touch ./demoCA/index.txt");
//	system("echo 01 > ./demoCA/serial");
//
//	//build CA,generate CA's ECC key-pair,does not have password
//	printf("***********************************************\n 2) build CA,generate CA's ECC key-pair:");
//	system("openssl ecparam -out EccCAkey.pem -name prime256v1 -genkey");
//	//generate CA's cert request,and self-signed certificate
//	//system("openssl req -new -x509 -days 365 -key ./demoCA/private/cakey.pem -out ./demoCA/cacert.pem");
//	printf("***********************************************\n 3) generate CA's cert request:");
//	system("openssl req -key EccCAkey.pem -new -out EccCAreq.pem");
//
//	printf("***********************************************\n 4) generate CA's self-signed certificate:");
//	system("openssl x509 -req -in EccCAreq.pem -signkey EccCAkey.pem -out EccCAcert.pem");
//}



/*************************************************

Function:    // generate_keypair
Description: // 生成密钥对、数字证书签发请求文件
Calls:       // openssl指令
Called By:   // main();
Input:	     //	BYTE *userID-asu客户端的用户名(字符串)
Output:      //	asu客户端的密钥文件、数字证书签发请求文件
Return:      // void
Others:      // 基于c语言调用openssl的shell指令

*************************************************/

void generate_keypair(char * cakeyname)
{

	char tempcmd[200];
	memset(tempcmd, '\0', sizeof(tempcmd)); //初始化buf,以免后面写入乱码到文件中
	//generate user's ECC key-pair,does not have password
	sprintf(tempcmd,"openssl genrsa -out %s 1024",cakeyname);
	system(tempcmd);
}


/*************************************************

Function:    // generate_keypair_and_certrequest
Description: // 生成密钥对、数字证书签发请求文件
Calls:       // openssl指令
Called By:   // main();
Input:	     //	BYTE *userID-asu客户端的用户名(字符串)
Output:      //	asu客户端的密钥文件、数字证书签发请求文件
Return:      // void
Others:      // 基于c语言调用openssl的shell指令

*************************************************/

void generate_keypair_and_certrequest(BYTE *userID)
{

	char tempcmd[200];
	memset(tempcmd, '\0', sizeof(tempcmd)); //初始化buf,以免后面写入乱码到文件中
	//generate user's ECC key-pair,does not have password
	sprintf(tempcmd,"openssl genrsa -out %skey.pem 512",userID);
	system(tempcmd);

	//generate user's cert request,require the value of some attributes are the same as CA
	sprintf(tempcmd,"openssl req -new -days 365 -key %skey.pem -out %sreq.pem",userID, userID);
	system(tempcmd);
}

//void generate_keypair_and_certrequest(BYTE *userID)
//{
//
//	char tempcmd[200];
//	memset(tempcmd, '\0', sizeof(tempcmd)); //初始化buf,以免后面写入乱码到文件中
//	//generate user's ECC key-pair,does not have password
//	printf("***********************************************\n 5) generate user's ECC key-pair:");
//	sprintf(tempcmd,"openssl ecparam -out Ecc%skey.pem -name prime256v1 -genkey",userID);
//	system(tempcmd);
//
//	//generate user's cert request,require the value of some attributes are the same as CA
//	printf("***********************************************\n 6) generate user's cert request,require the value of some attributes are the same as CA:");
//	sprintf(tempcmd,"openssl req -key Ecc%skey.pem -new -out Ecc%sreq.pem",userID, userID);
//	system(tempcmd);
//}

/*************************************************

Function:    // CA_sign_cert
Description: // CA(驻留在asu中)使用自己的私钥来为asu客户端签发数字证书
Calls:       // openssl指令
Called By:   // main();
Input:	     //	BYTE *userID-asu客户端的用户名(字符串)
Output:      //	asu客户端的密钥文件、数字证书签发请求文件
Return:      // TRUE-证书签发成功，FALSE-证书签发失败
Others:      // 基于c语言调用openssl的shell指令

*************************************************/

BOOL CA_sign_cert(BYTE *userID)
{
	FILE *stream;
	int err;
	char usercername[30], currentdir[50], tempstring[200];
	memset(usercername, '\0', sizeof(usercername)); //初始化usercername,以免后面写入乱码到文件中
	memset(tempstring, '\0', sizeof(tempstring)); //初始化tempstring,以免后面写入乱码到文件中
	memset(currentdir, '\0', sizeof(currentdir)); //初始化tempstring,以免后面写入乱码到文件中
	stream = popen("pwd", "r"); //get current directory
	fread(currentdir, sizeof(char), sizeof(currentdir), stream); //将刚刚FILE* stream的数据流读取到currentdir
	currentdir[strlen(currentdir) - 1] = '\0';
	sprintf(usercername, "%sreq.pem", userID);

	sprintf(tempstring,"openssl ca -in %sreq.pem -out %s/demoCA/newcerts/%scert.pem",
			userID, currentdir, userID);     //基于c语言调用openssl的shell指令

	err = system(tempstring);
	if (err < 0)
		return FALSE;
	else
		return TRUE;
}
//BOOL CA_sign_cert(BYTE *userID)
//{
//	FILE *stream;
//	int err;
//	char usercername[30], currentdir[50], tempstring[200];
//	memset(usercername, '\0', sizeof(usercername)); //初始化usercername,以免后面写入乱码到文件中
//	memset(tempstring, '\0', sizeof(tempstring)); //初始化tempstring,以免后面写入乱码到文件中
//	memset(currentdir, '\0', sizeof(currentdir)); //初始化tempstring,以免后面写入乱码到文件中
//	stream = popen("pwd", "r"); //get current directory
//	fread(currentdir, sizeof(char), sizeof(currentdir), stream); //将刚刚FILE* stream的数据流读取到currentdir
//	currentdir[strlen(currentdir) - 1] = '\0';
//	sprintf(usercername, "Ecc%sreq.pem", userID);
//
//	printf("***********************************************\n 7) CA sign the user's cert, using CA's private key:");
//	sprintf(tempstring,"openssl x509 -req -in %s -CA EccCAcert.pem -CAkey EccCAkey.pem -out %s/demoCA/newcerts/Ecc%scert.pem -CAcreateserial",
//			usercername, currentdir, userID);     //基于c语言调用openssl的shell指令
//
//	err = system(tempstring);
//	if (err < 0 || (err == 256))
//		return FALSE;
//	else
//		return TRUE;
//}


/*************************************************

Function:    // init_server_socket
Description: // 初始化asu(扮演服务器角色)的server_socket
Calls:       // socket API
Called By:   // main();
Input:	     //	无
Output:      //	无
Return:      // server_socket
Others:      //

*************************************************/
int init_server_socket()
{
    struct sockaddr_in server_addr;

    // 接收缓冲区
    int nRecvBuf = 32*1024; //设置为32K
    //发送缓冲区
    int nSendBuf = 32*1024; //设置为32K

    bzero(&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htons(INADDR_ANY);
    server_addr.sin_port = htons(CHAT_SERVER_PORT);

    int server_socket = socket(AF_INET,SOCK_STREAM,0);

    setsockopt(server_socket,SOL_SOCKET,SO_RCVBUF,(const BYTE *)&nRecvBuf,sizeof(int));
    setsockopt(server_socket,SOL_SOCKET,SO_SNDBUF,(const BYTE *)&nSendBuf,sizeof(int));



    if( server_socket < 0)
    {
        printf("Create Socket Failed!");
        exit(1);
    }

    if( bind(server_socket,(struct sockaddr*)&server_addr,sizeof(server_addr)))
    {
        printf("Server Bind Port : %d Failed!", CHAT_SERVER_PORT);
        exit(1);
    }

    if ( listen(server_socket, 5) )
    {
        printf("Server Listen Failed!");
        exit(1);
    }
    return server_socket;
}


int send_to_peer(int new_server_socket, BYTE *send_buffer, int send_len)
{

	int length = send(new_server_socket,send_buffer,send_len,0);
	printf("---- send %d bytes -----\n",length);

    if(length <0)
    {
        printf("Socket Send Data Failed Or Closed\n");
        close(new_server_socket);
        return FALSE;
    }
	else
		return TRUE;
}

int recv_from_peer(int new_server_socket, BYTE *recv_buffer, int recv_len)
{
	int length = recv(new_server_socket,recv_buffer, recv_len,0);
	if (length < 0)
	{
		printf("Receive Data From Server Failed\n");
		return FALSE;
	}else if(length < recv_len)
	{
		printf("Receive data from server less than required.\n");
		return FALSE;
	}else if(length > recv_len)
	{
		printf("Receive data from server more than required.\n");
		return FALSE;
	}
	else
	{
		printf("receive data succeed, %d bytes.\n",length);
		return TRUE;
	}

}


/*************************************************

Function:    // getpubkeyfromcert
Description: // 从数字证书(PEM文件)中读取公钥
Calls:       // openssl中读PEM文件的API
Called By:   // fill_certificate_auth_resp_packet
Input:	     //	用户证书的用户名certnum
Output:      //	数字证书公钥
Return:      // EVP_PKEY *pubKey
Others:      // 用户证书的用户名certnum最好是用字符串形式，但是目前是int值，有待改进

*************************************************/
EVP_PKEY *getpubkeyfromcert(int certnum)
{
	EVP_PKEY *pubKey;

	BIO * key = NULL;
	X509 * Cert = NULL; //X509证书结构体，保存CA证书
	key = BIO_new(BIO_s_file());

	char certname[60];
	memset(certname, '\0', sizeof(certname)); //初始化certname,以免后面写如乱码到文件中
	if (certnum == 0)
		sprintf(certname, "./cacerts/cacert.pem");
	else
		sprintf(certname, "./cert/usercert%d.pem", certnum);

	BIO_read_filename(key,certname);
	if (!PEM_read_bio_X509(key, &Cert, 0, NULL))
	{
		/* Error 读取证书失败！*/
		printf("读取证书失败!\n");
		return NULL;
	}

	pubKey = EVP_PKEY_new();
	//获取证书公钥
	pubKey = X509_get_pubkey(Cert);
	return pubKey;
}




/*************************************************

Function:    // getprivkeyfromprivkeyfile
Description: // CA(驻留在ASU中)从cakey.pem中提取CA的私钥，以便后续进行ASU的签名
Calls:       // openssl读取私钥PEM文件相关函数
Called By:   // fill_certificate_auth_resp_packet
Input:	     //	无
Output:      //	CA(驻留在ASU中)的私钥
Return:      // EVP_PKEY *privKey
Others:      //

*************************************************/

EVP_PKEY * getprivkeyfromprivkeyfile(int userID)
{
	EVP_PKEY * privKey;
	FILE* fp;
	RSA* rsa;

	char keyname[40];

	if (userID == 0)
		sprintf(keyname, "./private/cakey.pem");                   //asu密钥文件
	else
		sprintf(keyname, "./private/userkey%d.pem", userID);       //ae或asue密钥文件
	fp = fopen(keyname, "r");

	if (NULL == fp)
	{
		fprintf(stderr, "Unable to open %s for RSA priv params\n", "./pricate/cakey.pem");
		return NULL;
	}

	rsa = RSA_new();
	if ((rsa = PEM_read_RSAPrivateKey(fp, &rsa, NULL, NULL)) == NULL)
	{
		fprintf(stderr, "Unable to read private key parameters\n");
		return NULL;
	}
	fclose(fp);

	// print
//	printf("Content of CA's Private key PEM file\n");
//	RSA_print_fp(stdout, rsa, 0);
//	printf("\n");

	privKey = EVP_PKEY_new();
	if (EVP_PKEY_set1_RSA(privKey, rsa) != 1) //保存RSA结构体到EVP_PKEY结构体
	{
		printf("EVP_PKEY_set1_RSA err\n");
		RSA_free (rsa);
		return NULL;
	} else
	{
		RSA_free (rsa);
		return privKey;
	}
}

/*************************************************

Function:    // getprivkeyfromkeyfile
Description: // 从密钥文件中提取出私钥的RSA结构体，以便后续进行公钥的提取以及私钥的签名操作
Calls:       // openssl读取私钥PEM文件函数、从PEM文件读取私钥RSA函数
Called By:   //
Input:	     //	userID-用户名，0-CA，非零-用户编号
Output:      //	私钥的RSA指针
Return:      // RSA *
Others:      // 本函数不要与getprivkeyfromprivkeyfile混淆，本函数为了2013.8.15认证服务其签发证书的演示所填加。

*************************************************/

RSA * getprivkeyfromkeyfile()
{
	EVP_PKEY * Key;
	FILE* fp;
	//RSA rsa_struct;
	RSA* rsa;

	char keyname[40];

	sprintf(keyname, "./private/cakey.pem");

	BIO * in = BIO_new_file(keyname, "rb");
	if (in == NULL )
		return FALSE;
	rsa = PEM_read_bio_RSAPrivateKey(in, NULL, NULL, NULL ); //提取私钥
	BIO_free(in);
	return rsa;
}


BOOL getUserCertData(char * username, BYTE buf[], int *len)
{
	FILE *fp;
	char certname[40];
	memset(certname, '\0', sizeof(certname));//初始化certname,以免后面写如乱码到文件中

	sprintf(certname, "./userfile/%s_cert.pem",username);                //eclipse调试或运行

	fp = fopen(certname, "rb");
	if (fp == NULL)
	{
		printf("reading the cert file failed!\n");
		return FALSE;
	}
	*len = fread(buf, 1, 5000, fp);
	//printf("用户证书长度为%d字节\n", *len);
	fclose(fp);
	//printf("将证书保存到缓存buffer成功!\n");

	return TRUE;
}


/*************************************************

Function:    // CA_gen_cert_request
Description: // CA生成自签名证书请求文件
Calls:       // X509_REQ相关函数
Called By:   //
Input:	     //	userID---待读取文件编号，0-CA，非零-用户编号
Output:      //	证书请求文件(txt和PEM文件)
Return:      // void
Others:      //

*************************************************/

void CA_gen_cert_request()
{
	X509_REQ *req;
	int ret = 0;
	long version = 3;
	X509_NAME *name;
	char mdout[20]; //bytes不要超过30位
	int mdlen = 0;
	const EVP_MD *md;
	BIO *b;

	RSA *rsa;
	EVP_PKEY * privkey,* pubkey;

	//初始化申请
	req = X509_REQ_new();
	ret = X509_REQ_set_version(req, version);

	//填写申请者相关信息
	char countryname[10] = "CN";
	char provincename[10] = "JS";
	char organizationname[10] = "CIOTC";
	char commonname[50] = "WLWAQCA";

	name = X509_REQ_get_subject_name(req);
	X509_NAME_add_entry_by_NID(name, NID_countryName, V_ASN1_PRINTABLESTRING,
			(unsigned char*) countryname, strlen(countryname), -1, 0);
	X509_NAME_add_entry_by_NID(name, NID_stateOrProvinceName,
			V_ASN1_PRINTABLESTRING, (unsigned char*) provincename,
			strlen(provincename), -1, 0);
	X509_NAME_add_entry_by_NID(name, NID_organizationName,
			V_ASN1_PRINTABLESTRING, (unsigned char*) organizationname,
			strlen(organizationname), -1, 0);
	X509_NAME_add_entry_by_NID(name, NID_commonName, V_ASN1_PRINTABLESTRING,
			(unsigned char*) commonname, strlen(commonname), -1, 0);


	/*提取CA的私钥*/

	rsa = RSA_new();
	rsa = getprivkeyfromkeyfile();

	if (rsa == NULL )
	{
		printf("从CA密钥文件提取CA私钥失败！\n");
		return;
	}
	privkey = EVP_PKEY_new();
	EVP_PKEY_assign_RSA(privkey, rsa);
	if (privkey == NULL )
	{
		printf("RSA->EVP_PKEY 转化私钥格式失败！\n");
	}


	/*提取CA的公钥*/
	RSA *tem = RSAPublicKey_dup(rsa);
	if (tem == NULL )
	{
		printf("从CA密钥文件提取CA公钥失败！\n");
	}

	pubkey = EVP_PKEY_new();
	EVP_PKEY_assign_RSA(pubkey, tem);
	if (pubkey == NULL )
	{
		printf("RSA->EVP_PKEY 转化公钥格式失败！\n");
	}

	ret = X509_REQ_set_pubkey(req, pubkey);
	printf("CA自签名证书请求文件中的公钥字段添加完成！\n");


	//hash 算法
	md = EVP_sha1();
	ret = X509_REQ_digest(req, md, (unsigned char *) mdout, &mdlen); //摘要
	ret = X509_REQ_sign(req, privkey, md); //用私钥签名
	if (!ret)
	{
		printf("CA自签名证书请求文件中的私钥签名失败！\n");
		X509_REQ_free(req);
		return;
	}
	// 写入文件,PEM和TXT格式
	else
	{
		printf("CA自签名证书请求文件中的私钥签名成功\n");
		//将申请证书可视化打印输出出来，输出保存到txt文件
		b = BIO_new_file(CA_REQ_PRINT, "w");
		X509_REQ_print(b, req);
		BIO_free(b);

		//将申请证书保存起来，保存到PEM文件
		b = BIO_new_file(CA_REQ, "w");
		PEM_write_bio_X509_REQ(b, req);
		BIO_free(b);

		//CA用自己的公钥来验证证书请求的签名字段
		ret = X509_REQ_verify(req, pubkey);
		if (ret == 0)
		{
			printf("CA自签名证书请求文件中的私钥签名字段验证失败\n");
		}
		//printf("CA自签名证书请求文件中的私钥签名字段验证结果为：%d\n", ret);
		printf("CA自签名证书请求文件中的私钥签名字段验证成功！\n");
		X509_REQ_free(req);

		printf("CA自签名证书请求文件生成成功！\n");
		return;
	}
}


//该函数用于放入证书的持有者信息
BOOL SetOwnerName( X509 *m_pxCert,char * szCountry, char * szProvince, char * szOrganization, char * szCommon )
{
	X509_NAME * name = X509_get_subject_name(m_pxCert);

	if (!X509_NAME_add_entry_by_NID(name,NID_countryName,V_ASN1_PRINTABLESTRING,(unsigned char*)szCountry,strlen(szCountry),-1,0))
		return FALSE;
	if (!X509_NAME_add_entry_by_NID(name,NID_stateOrProvinceName,V_ASN1_PRINTABLESTRING,(unsigned char*)szProvince,strlen(szProvince),-1,0))
		return FALSE;
	if (!X509_NAME_add_entry_by_NID(name,NID_organizationName,V_ASN1_PRINTABLESTRING,(unsigned char*)szOrganization,strlen(szOrganization),-1,0))
		return FALSE;
	if (!X509_NAME_add_entry_by_NID(name,NID_commonName,V_ASN1_PRINTABLESTRING,(unsigned char*)szCommon,strlen(szCommon),-1,0))
		return FALSE;
	return TRUE;
}

//该函数用于放入证书的颁发者
 SetIssuerName( X509* m_pxCert,char * szCountry, char * szProvince, char * szOrganization, char * szCommon )
{
	X509_NAME * name = X509_get_issuer_name(m_pxCert);

	//写入颁发者信息
	if (!X509_NAME_add_entry_by_NID(name,NID_countryName,V_ASN1_PRINTABLESTRING,(unsigned char*)szCountry,strlen(szCountry),-1,0))
		return FALSE;
	if (!X509_NAME_add_entry_by_NID(name,NID_stateOrProvinceName,V_ASN1_PRINTABLESTRING,(unsigned char*)szProvince,strlen(szProvince),-1,0))
		return FALSE;
	if (!X509_NAME_add_entry_by_NID(name,NID_organizationName,V_ASN1_PRINTABLESTRING,(unsigned char*)szOrganization,strlen(szOrganization),-1,0))
		return FALSE;
	if (!X509_NAME_add_entry_by_NID(name,NID_commonName,V_ASN1_PRINTABLESTRING,(unsigned char*)szCommon,strlen(szCommon),-1,0))
		return FALSE;
	return TRUE;
}

 //该函数用于保存PEM证书文件
 BOOL SaveCertificate(X509 *m_pxCert,const char * certPath)
 {
 	BIO	* out = BIO_new_file(certPath,"wb");
 	int res;
 	res=PEM_write_bio_X509(out,m_pxCert);
 	BIO_free(out);
 	return (res > 0);
 }

 /*************************************************

 Function:    // gen_cert
 Description: // 生成证书
 Calls:       // X509_REQ相关函数以及X509证书字段设置函数
 Called By:   //
 Input:	      // userID---待读取文件编号，0-CA，非零-用户编号
 Output:      // 证书文件(txt和PEM文件)
 Return:      // void
 Others:      //

 *************************************************/

 void gen_cert(char * username)
 {
 	BIO *certReq;
 	X509_REQ *req = NULL;

 	char *caname="CA";

 	char certreqname[40];
 	memset(certreqname, '\0', sizeof(certreqname));//初始化certreqname,以免后面写入乱码到文件中

 	char certname[40];
 	memset(certname, '\0', sizeof(certreqname));//初始化certname,以免后面写入乱码到文件中

 	if(memcmp(username,caname,strlen(caname))== 0)
 	{
 		sprintf(certreqname, "./cafile/ca_req.pem");
 		sprintf(certname, "./cafile/ca_cert.pem");
 	}

 	else
 	{
 		sprintf(certreqname, "./userfile/%s_req.pem", username);
 		sprintf(certname, "./userfile/%s_cert.pem", username);
 	}


 	//将PEM格式的证书存为X509证书格式
 	certReq = BIO_new_file(certreqname, "r");
 	req = PEM_read_bio_X509_REQ(certReq, NULL, NULL, NULL );

 	if (req == NULL )
 	{
 		printf("读取证书请求文件失败");
 		return;
 	}
 	else
 		printf("\n开始读取证书请求文件信息!\n");


 	//读取证书请求文件中的版本号
 	long version;
 	version = X509_REQ_get_version(req);

 	//读取申请者信息
 	X509_NAME *subject;
 	subject=X509_REQ_get_subject_name(req);


 	X509_NAME_ENTRY *name_entry;
 	unsigned char msginfo[1024];
 	int msginfoLen;
 	int entriesNum;
 	int i;
 	long Nid;

 	char country[30];
 	char provincename[30];
 	char organizationname[30];
 	char commonname[30];

 	entriesNum=sk_X509_NAME_ENTRY_num(subject->entries);

 	printf("\n读取证书申请者基本信息如下：*********************************************\n");
 	//循环读取各条目值
 	for (i = 0; i < entriesNum; i++)
 	{
 		//获取第I个条目值
 		name_entry = sk_X509_NAME_ENTRY_value(subject->entries,i);
 		Nid = OBJ_obj2nid(name_entry->object);
 		msginfoLen = name_entry->value->length;
 		memcpy(msginfo, name_entry->value->data, msginfoLen);
 		msginfo[msginfoLen] = '\0';

 		switch (Nid)
 		{
 		case NID_countryName:
 			printf("国家：%s\n", msginfo);
 			sprintf(country, "%s", msginfo);
 			break;
 		case NID_stateOrProvinceName:
 			printf("省份：%s\n", msginfo);
 			sprintf(provincename, "%s", msginfo);
 			break;
 		case NID_organizationName:
 			printf("组织：%s\n", msginfo);
 			sprintf(organizationname, "%s", msginfo);
 			break;
 		case NID_commonName:
 			printf("通用名：%s\n", msginfo);
 			sprintf(commonname, "%s", msginfo);
 			break;
 		} //end switch
 	} //end for
 	printf("\n读取证书申请者基本信息结束！*********************************************\n");

 	/*开始生成证书*/
 	X509 *m_pxCert;	// X509证书实体
 	EVP_PKEY *m_pSignKey = NULL;
 	EVP_PKEY *m_pOwnerKey = NULL;
 	EVP_PKEY *pub_key = NULL;
 	RSA *rsa = RSA_new();
 	long nSerial = 1;
 	int nDays = 1;
 	m_pxCert = X509_new();


 	/*设置公钥*/
 	pub_key = X509_REQ_get_pubkey(req);	//从请求文件(req文件)中提取公钥
 	if (pub_key == NULL )
 	{
 		printf("从证书请求文件中公钥提取失败\n");
 		return;
 	}	//end if
 	else
 		printf("\n从证书请求文件中提取公钥成功!\n");

 	/*CA利用从证书请求文件中提取的用户公钥来验证证书请求文件的签名字段，对用户进行POP(proof of possession of private key)检查*/
 	int ret;
 	ret = X509_REQ_verify(req, pub_key);
 	if (ret == 0)
 	{
 		printf("证书请求文件签名字段验证失败！\n");
 	}
 	//printf("证书请求文件签名字段验证结果为：%d\n", ret);
 	printf("证书请求文件申请者签名字段验证成功！CA开始签发证书！\n");


 	//设置序列号和版本号
	X509_set_version(m_pxCert, version - 1);	//为啥减1才行呢？？？
	ASN1_INTEGER_set(X509_get_serialNumber(m_pxCert), nSerial);

	//设置有效期
	X509_gmtime_adj(X509_get_notBefore(m_pxCert), (long) 0);
	X509_gmtime_adj(X509_get_notAfter(m_pxCert), nDays * 24 * 60 * 60);

	//调用自己写的函数(不是openssl的API函数)来设置持有者信息
	if (!SetOwnerName(m_pxCert, country, provincename, organizationname,
			commonname))
	{
		printf("证书中添加证书持有者信息失败!\n");
		return;
	}
	else
		printf("证书中添加证书持有者信息成功!\n");

	//调用自己写的函数(不是openssl的API函数)来设置颁发者信息
	if (!SetIssuerName(m_pxCert, "CN", "JS", "CIOTC", "WLWAQCA"))
	{
		printf("证书中添加证书颁发者信息失败!\n");
		return;
	}
	else
		printf("证书中添加证书颁发者信息成功!\n");




 	//CA将从证书请求文件中提取的用户公钥添加到证书的公钥字段
 	if (!X509_set_pubkey(m_pxCert, pub_key))
 	{
 		printf("证书中添加公钥字段失败!\n");
 		return;
 	}	//end if
 	else
 		printf("证书中添加公钥字段成功！\n");

 	/*提取CA的私钥*/
 	RSA *ca_rsa;
 	ca_rsa = RSA_new();

 	//从CA密钥文件提取私钥
 	ca_rsa = getprivkeyfromkeyfile(0);
 	if (ca_rsa == NULL )
 	{
 		printf("CA从文件提取CA私钥失败！\n");
 		return;
 	}
 	else
 		printf("CA从文件提取CA私钥成功！CA用该密钥为证书签名!\n");

 	/*提取CA的公钥，后续利用CA公钥来验证证书中CA的签名字段是否正确*/
 	RSA *capubrsa = RSAPublicKey_dup(ca_rsa);
 	EVP_PKEY *capubkey = EVP_PKEY_new();
 	EVP_PKEY_assign_RSA(capubkey, capubrsa);

 //	if (pubkey == NULL )
 //	{
 //		printf("提取公钥失败！\n");
 //	}

 	/*将读取的rsa私钥打印输出到TXT文件*/
 	BIO * rsa_priv = BIO_new_file(CA_RSA_PRIV_PRINT, "w");
 	RSA_print(rsa_priv, ca_rsa, 0);
 	BIO_free(rsa_priv);


 	/*CA使用CA私钥为证书签名*/
 	m_pSignKey = EVP_PKEY_new();
 	RSA *signrsa = RSA_new();
 	signrsa = RSAPrivateKey_dup(ca_rsa);
 	EVP_PKEY_assign_RSA(m_pSignKey, signrsa);
 	X509_sign(m_pxCert, m_pSignKey, EVP_sha1());

 	//将生成的证书打印输出到文件
 	BIO *b_cert;

 	b_cert = BIO_new_file(certreqname, "w");

// 	if (user_ID == 0)
// 		b_cert = BIO_new_file(CA_CERT_PRINT, "w");
// 	else
// 		b_cert = BIO_new_file(USER_CERT_PRINT, "w");

 	X509_print(b_cert, m_pxCert);
 	BIO_free(b_cert);

 	int flag = (X509_verify(m_pxCert, capubkey));
 	if (flag != 1)
 	{
 		printf("证书签名验证不通过\n");
 		return;
 	}
// 	else
// 		printf("证书签名验证通过\n");

 	//保存证书

 	if (!SaveCertificate(m_pxCert, certname))
 	{
		printf("保存证书失败！\n");
	} //end if

// 	if(user_ID == 0)
// 	{
// 		if (!SaveCertificate(m_pxCert, CA_CERT))
// 		{
// 			printf("保存证书失败！\n");
// 		}//end if
// 	}
// 	else
// 	{
// 		if (!SaveCertificate(m_pxCert, USER_CERT))
// 		{
// 			printf("保存证书失败！\n");
// 		}//end if
// 	}
 	printf("CA签发证书成功!证书保存成功!\n");

 	return;
 }


 BOOL writeCertRequFile(char * username, BYTE buf[], int len)
 {
 	FILE *fp;
 	char certrequname[40];
 	memset(certrequname, '\0', sizeof(certrequname));//初始化certname,以免后面写入乱码到文件中
 	sprintf(certrequname, "./userfile/%s_req.pem", username);

// 	printf("证书签发请求文件名为: %s\n", certrequname);

 	fp = fopen(certrequname, "w");
 	if (fp == NULL)
 	{
 		printf("open cert file failed!\n");
 		return FALSE;
 	}
 	int res = fwrite(buf, 1, len, fp);
 	printf("证书签发请求文件长度为%d字节\n", len);
 	fclose(fp);
// 	printf("将接收到的证书签发请求文件数据写入文件成功!\n");
 	printf("证书签发请求文件名为: %s\n", certrequname);

 	return TRUE;
 }





/*************************************************

Function:    // fill_certificate_auth_resp_packet
Description: // 按照认证协议中的证书认证响应分组格式来填充分组
Calls:       // getpubkeyfromcert，verify_sign，X509_Cert_Verify，getprivkeyfromprivkeyfile，gen_sign
Called By:   // fill_certificate_auth_resp_packet
Input:	     //	input---待生成签名的整个数据包(分组)
                sign_input_len---待生成签名的有效数据字段的长度，并非整个input长度
                sign_value---保存生成的字段
                sign_output_len---生成的签名字段的长度
                privKey---生成签名所使用的私钥
Output:      //	生成签名操作结果，TRUE or FALSE
Return:      // TRUE or FALSE
Others:      //

*************************************************/

int fill_certificate_sign_resp_packet(certificate_sign_requ *recv_certificate_sign_requ_buffer,certificate_sign_resp *send_certificate_sign_resp_buffer)
{

	BYTE cert_buffer[5000];
	int cert_buffer_len;

	printf("\n---第一部分(2)：CA为用户签发证书过程(接收数字证书请求文件、签发数字证书)begin---\n");

	if(!writeCertRequFile(recv_certificate_sign_requ_buffer->username,recv_certificate_sign_requ_buffer->certificate_sign_requ_buffer,recv_certificate_sign_requ_buffer->certificate_sign_requ_buffer_len))
	{
		printf("CA将接收到的用户证书请求数据保存到文件失败！\n");
	}
	else
		printf("CA将接收到的用户证书请求数据保存到文件成功！\n");

	gen_cert(recv_certificate_sign_requ_buffer->username);

	printf("\n---第一部分(2)：CA为用户签发证书过程(接收数字证书请求文件、签发数字证书)end-----\n");


	getUserCertData(recv_certificate_sign_requ_buffer->username,cert_buffer, &cert_buffer_len);

	memcpy(send_certificate_sign_resp_buffer->usercer.cer_X509,cert_buffer,cert_buffer_len);
	send_certificate_sign_resp_buffer->usercer.cer_length = cert_buffer_len;

//	if(!writeCertRequFile(2,send_certificate_sign_resp_buffer->usercer.cer_X509,send_certificate_sign_resp_buffer->usercer.cer_length))
//	{
//		printf("将用户的证书数据保存到PEM文件失败！\n");
//	}

	//填充wai_packet_head
	send_certificate_sign_resp_buffer->wai_packet_head.version = 1;
	send_certificate_sign_resp_buffer->wai_packet_head.type = 1;
	send_certificate_sign_resp_buffer->wai_packet_head.subtype = ISSUE_CERTIFICATE;
	send_certificate_sign_resp_buffer->wai_packet_head.reserved = 0;
	send_certificate_sign_resp_buffer->wai_packet_head.length = sizeof(certificate_sign_resp);
	send_certificate_sign_resp_buffer->wai_packet_head.packetnumber = 254;
	send_certificate_sign_resp_buffer->wai_packet_head.fragmentnumber = 0;
	send_certificate_sign_resp_buffer->wai_packet_head.identify = 0;


	//利用全局变量countcertsignresp来打印ASU中的fill_certificate_sign_resp_packet函数运行的次数，该部分打印如感觉没必要可删除
	printf("ASU中的fill_certificate_sign_resp_packet函数运行的次数为第%d次！\n",countcertsignresp);
	countcertsignresp++;

	return TRUE;

}













void process_request(int client_ae_socket, certificate_sign_requ * recv_certificate_sign_requ_buffer,int recv_buffer_len)
{
	certificate_sign_resp send_certificate_sign_resp_buffer;

	BYTE subtype;

	subtype = recv_certificate_sign_requ_buffer->wai_packet_head.subtype;     //WAI协议分组基本格式包头的第三个字节是分组的subtype字段，用来区分不同的分组


    switch(subtype)
    {
	case CERTIFICATE_SIGN_REQU:

		if(!(fill_certificate_sign_resp_packet(recv_certificate_sign_requ_buffer,&send_certificate_sign_resp_buffer)))
		{
			printf("fill certificate sign resp packet failed!\n");
		}
	    send_to_peer(client_ae_socket, (BYTE *)&send_certificate_sign_resp_buffer, sizeof(certificate_sign_resp));
		break;

//	case XXX:其他case留作以后通信分组使用
//		XXX---其他case处理语句
//		break;
    }
}



void * talk_to_ae(void * new_asu_server_socket_to_client_ae)
{
	int recv_buffer_len;
	int new_asu_server_socket = (int)new_asu_server_socket_to_client_ae;

	certificate_sign_requ recv_certificate_sign_requ_buffer;

//	BYTE recv_buffer[15000];

	memset((BYTE *)&recv_certificate_sign_requ_buffer, 0, sizeof(certificate_sign_requ));

	recv_buffer_len = recv(new_asu_server_socket, (BYTE *)&recv_certificate_sign_requ_buffer,sizeof(certificate_sign_requ), 0);

	printf("\n********************************************************************************\n");
	printf("CA接收到用户%d字节的用户证书签发请求分组!!!\n",recv_buffer_len);
	printf("********************************************************************************\n");

	if (recv_buffer_len < 0)
	{
		printf("Server Recieve Data Failed!\n");
		close(new_asu_server_socket);
		pthread_exit(NULL);
	}
	if (recv_buffer_len == 0)
	{
		close(new_asu_server_socket);
		pthread_exit(NULL);
	}

	process_request(new_asu_server_socket, (BYTE *)&recv_certificate_sign_requ_buffer, recv_buffer_len);



	close(new_asu_server_socket);
	pthread_exit(NULL);


}


int main(int argc, char **argv)
{

	char *cakeyname="cakey.pem";
	char *caname="CA";
	OpenSSL_add_all_algorithms();

	//**************************************演示清单第一部分离线证书签发等操作 begin***************************************************

	//演示清单第一部分(1),CA私钥生成、CA自签名证书请求、CA自签名证书签发过程演示,为了节省时间，密钥生成部分也可以考虑提前生成

	//***openssl命令生成CA密钥演示部分 begin

	generate_keypair(cakeyname);

	//stop for keyboard
	getchar();

	//***openssl命令生成CA密钥演示部分 end

	//***openssl函数生成CA自签名证书请求、CA自签名证书生成演示部分 begin
	printf("***演示清单第一部分：证书签发全过程(密钥文件、数字证书请求、数字证书)***\n");
	printf("\n---第一部分(1):CA自签名证书签发begin---\n");
	CA_gen_cert_request();
	gen_cert(caname);
	printf("\n---第一部分(1):CA自签名证书签发end---\n");
	//***openssl函数生成演示部分 end

	//演示清单第一部分(2),用户私钥生成、CA为用户签发证书过程演示在fill_certificate_sign_resp_packet函数中完成


	//**************************************演示清单第一部分离线证书签发等操作 end********************************************************


	//**************************************演示清单第二部分WAPI的WAI认证过程演示 begin***************************************************
	pthread_mutex_init(&user_table_mutex, NULL);
	int asu_server_socket = init_server_socket();

	pthread_t child_thread;
	pthread_attr_t child_thread_attr;
	pthread_attr_init(&child_thread_attr);
	pthread_attr_setdetachstate(&child_thread_attr, PTHREAD_CREATE_DETACHED);

	while (1)
	{
		struct sockaddr_in client_addr;
		socklen_t length = sizeof(client_addr);
		int new_asu_server_socket = accept(asu_server_socket,
				(struct sockaddr*) &client_addr, &length);
		if (new_asu_server_socket < 0)
		{
			printf("Server Accept Failed!\n");
			break;
		}
		if (pthread_create(&child_thread, &child_thread_attr, talk_to_ae,(void *) new_asu_server_socket) < 0)
			printf("pthread_create Failed : %s\n", strerror(errno));
	}
   //**************************************演示清单第二部分WAPI的WAI认证过程演示 end***************************************************
}
