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
/* Engine ��id */
static const char *engine_hw_id = "sm3engine";
/* Engine ������ */
static const char *engine_hw_name = "sm3engineTest";


#define NID_SM3 112233

/*	SM3ժҪ�Ľ������Ϊ32�ֽڣ�256���أ�	*/
#define SM3_DIGEST_LENGTH 32 

/*	SM3������Ϣ�Ŀ鳤��Ϊ64�ֽڣ�512���أ������ֵ�ڼ���HMACʱ���õ�	*/
#define SM3_CBLOCK			(64)

#define EVP_PKEY_NULL_method	NULL,NULL,{0,0,0,0}

/*  MAC����Ĭ��ΪС��    */
typedef unsigned int sm3_word_t;
typedef struct sm3_ctx_t {
	int m_size;							/*	�ܳ���(�ֽ�)			*/
	
	unsigned char remain[128];			/*	ʣ�������(�鳤�ȵ�2��)	*/
	unsigned int r_len;					/*	ʣ�����ݳ���(�ֽ�)		*/

	sm3_word_t iv[8];					/*	�м���̵�IVֵ			*/
	
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
		NID_SM3,   //��ϢժҪ�㷨��NID��ʶ
		NID_SM3,  //����ϢժҪ-ǩ���㷨���Ƶ���ӦNID��ʶ
		SM3_DIGEST_LENGTH,  //����ϢժҪ�㷨���ɵ���ϢժҪ�ĳ��ȣ���SHA�㷨��SHA_DIGEST_LENGTH����ֵ��20
		0,       //flags
		init,  //ָ��һ���ض���ϢժҪ�㷨�ĳ�ʼ�������������SHA�㷨��ָ��ָ��SHA_Init
		update,  //ָ��һ����������ժҪֵ�ĺ���������SHA�㷨����ָ��SHA_Update
		final, //ָ��һ����ϢժҪֵ����֮��Ҫ���õĺ������ú����������һ�����ݵĴ�����������SHA�㷨����ָ��SHA_Final.
		NULL,   //copy ָ��һ������������EVP_MD_CTX�ṹ֮�俽������ֵ�ĺ���
		NULL,   //cleanup
		SM3_CBLOCK,   //һ������������ϢժҪ�������ĵĳ��ȣ���λ���ֽڣ�����SHA�㷨����SHA_CBLOCK
		sizeof(EVP_MD *)+sizeof(SHA_CTX),  //��CTX�ṹ�ĳ��ȣ���SHA�㷨����Ӧ�þ���sizeof(EVP_MD*)+sizeof(SHA_CTX)
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
	//����engine
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