#include "main.h"


int SM3Test(ENGINE *e,unsigned char * data,int size){
	const EVP_MD *md;
	EVP_MD_CTX *mdctx;

	char buf[512*2]={0};
	char hash[512*2]={0};
	unsigned int  len=0;
	
	md = EVP_get_digestbyname("sm3");
	if(!md) {

		return -1;
    }
    mdctx = EVP_MD_CTX_new(); //allocates, initializes and returns a digest context.
    EVP_DigestInit_ex(mdctx, md, e);  //sets up digest context ctx to use a digest type from ENGINE impl.
	size_t nread=0;
    EVP_DigestUpdate(mdctx, data, size); //hashes cnt bytes of data at d into the digest context ctx. 


    EVP_DigestFinal_ex(mdctx,(unsigned char*) hash, (unsigned int *)&len);//retrieves the digest value from ctx and places it in md. 

    EVP_MD_CTX_free(mdctx);

	LOG_DATA(hash, len);
	return 0;
}
/* Engine 的id */
static const char *engine_hw_id = "sm3engine";
/* Engine 的名字 */
static const char *engine_hw_name = "sm3engineTest";


#define NID_SM3 112233

/*	SM3摘要的结果长度为32字节（256比特）	*/
#define SM3_DIGEST_LENGTH 32 

/*	SM3输入消息的块长度为64字节（512比特），这个值在计算HMAC时会用到	*/
#define SM3_CBLOCK			(64)

#define EVP_PKEY_NULL_method	NULL,NULL,{0,0,0,0}

/*  MAC环境默认为小端    */
typedef unsigned int sm3_word_t;
typedef struct sm3_ctx_t {
	int m_size;							/*	总长度(字节)			*/
	
	unsigned char remain[128];			/*	剩余的数据(块长度的2倍)	*/
	unsigned int r_len;					/*	剩余数据长度(字节)		*/

	sm3_word_t iv[8];					/*	中间过程的IV值			*/
	
} SM3_CTX;



int init(EVP_MD_CTX *ctx){
	INFO("start...\n");
	INFO("end...\n");
	return 1;
}

int update(EVP_MD_CTX *ctx, const void *data, size_t count){
	INFO("start...\n");
	INFO("end...\n");
	return 1;
}
int final(EVP_MD_CTX *ctx, unsigned char *md){
	INFO("start...\n");
	INFO("end...\n");
	return 1;
}
int md_ctrl (EVP_MD_CTX *ctx, int cmd, int p1, void *p2){
	INFO("start...\n");
	INFO("end...\n");
	return 1;
}
const EVP_MD *EVP_sm3_md(void)
{
	static const EVP_MD ecdsa_sm3_md = 
	{
		NID_SM3,   //信息摘要算法的NID标识
		NID_SM3,  //是信息摘要-签名算法体制的相应NID标识
		SM3_DIGEST_LENGTH,  //是信息摘要算法生成的信息摘要的长度，如SHA算法是SHA_DIGEST_LENGTH，该值是20
		0,       //flags
		init,  //指向一个特定信息摘要算法的初始化函数，如对于SHA算法，指针指向SHA_Init
		update,  //指向一个真正计算摘要值的函数，例如SHA算法就是指向SHA_Update
		final, //指向一个信息摘要值计算之后要调用的函数，该函数完成最后的一块数据的处理工作。例如SHA算法就是指向SHA_Final.
		NULL,   //copy 指向一个可以在两个EVP_MD_CTX结构之间拷贝参数值的函数
		NULL,   //cleanup
		SM3_CBLOCK,   //一个用来进行信息摘要的输入块的的长度（单位是字节），如SHA算法就是SHA_CBLOCK
		sizeof(EVP_MD *)+sizeof(SHA_CTX),  //是CTX结构的长度，在SHA算法里面应该就是sizeof(EVP_MD*)+sizeof(SHA_CTX)
		md_ctrl
	};
	return(&ecdsa_sm3_md);
}

static int sm3_digest(ENGINE *e, const EVP_MD **digest,const int **nids, int nid)
{
	switch (nid) {
	case NID_sm3:
		*digest = EVP_sm3_md();
		break;

	default:
		*digest = NULL;
		break;
	}
	
	return (*digest != NULL);
}


static int bind_helper(ENGINE *e){
	if(!ENGINE_set_id(e,engine_hw_id) || 
	  !ENGINE_set_name(e,engine_hw_name) ||
	  !ENGINE_set_digests(e,sm3_digest) ||
		0){

		return 0;
	}
	return 1;
}


static ENGINE *engine_hwcipher(void)
{
	ENGINE *ret = ENGINE_new();
	if(!ret)
		return NULL;
	if(!bind_helper(ret))
	{
		ENGINE_free(ret);
		return NULL;
	}
	return ret;
}


void ENGINE_load_hwcipher()
{
	ENGINE *e_hw = engine_hwcipher();
	if (!e_hw) return;
	ENGINE_add(e_hw);
	ENGINE_free(e_hw);
	ERR_clear_error();
}


int main(){
	char *str="this is a test...";

	OpenSSL_add_all_algorithms();
	
/*********************************************/
	//加载engine
	ENGINE *e;

	ENGINE_load_hwcipher();

	e=ENGINE_by_id(engine_hw_id);
	if(!e){ printf("ENGINE_by_id failed\n"); return -1;}
	char *name = (char *)ENGINE_get_name(e);
	printf("engine name :%s \n",name);
/*********************************************/

	SM3Test(e,(unsigned char*)str, strlen(str));
	return 0;
}