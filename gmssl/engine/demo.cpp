#include "main.h"
static int hw_get_random_bytes(unsigned char* buf, int num)
{
	int i;
	printf("call hw_get_random_bytes\n");
	for(i=0;i<num;i++)
	memset(buf++,i,1);
	return 1;
}
/* 生成RSA 密钥对 */
static int genrete_rsa_key(RSA *rsa, int bits, BIGNUM *e, BN_GENCB *cb)
{
	printf("genrete_rsa_key \n");
	return 1;
}
/* RSA 公钥加密 */
int rsa_pub_enc(int flen,const unsigned char *from,unsigned char *to,RSA *rsa,int padding)
{
	printf("call rsa_pub_enc \n");
	return 1;
}
/*RSA 公钥解密 */
int rsa_pub_dec(int flen,const unsigned char *from,unsigned char *to,RSA *rsa,int padding)
{
	printf("call rsa_pub_enc \n");
	return 1;
}
/* RSA 私钥加密 */
int rsa_priv_enc(int flen,const unsigned char *from,unsigned char *to,RSA *rsa,int padding)
{
	char *keyid;
	/* 获取私钥id */
	keyid=(char *)ENGINE_get_ex_data(rsa->engine,0);
	printf("call rsa_pub_dec \n");
	printf("use key id :%d \n",keyid);
	return 1;
}
/* RSA 私钥解密 */
int rsa_priv_dec(int flen,const unsigned char *from,unsigned char *to,RSA *rsa,int padding)
{
	printf("call rsa_priv_dec \n");
	return 1;
}
/* RSA 算法 */
RSA_METHOD hw_rsa =
{
	"hw cipher",
	rsa_pub_enc,
	rsa_pub_dec,
	rsa_priv_enc,
	rsa_priv_dec,
	NULL,
	NULL,
	NULL,
	NULL,  // int (*finish) (RSA *rsa);  called at free 
	RSA_FLAG_SIGN_VER,
	NULL,
	NULL,
    
	NULL,
	genrete_rsa_key
};
/* 随机数方法 */
static RAND_METHOD hw_rand =
{
	NULL,//void (*seed)(const void *buf, int num);
	hw_get_random_bytes,//int (*bytes)(unsigned char *buf, int num);
	NULL, //void (*cleanup)(void);
	NULL, //void (*add)(const void *buf, int num, double entropy);
	NULL, //int (*pseudorand)(unsigned char *buf, int num);
	NULL, //int (*status)(void);
};
/* Engine 的id */
static const char *engine_hw_id = "ID_hw";
/* Engine 的名字 */
static const char *engine_hw_name = "hwTest";
static int hw_init(ENGINE *e)
{
	printf("call hw_init\n");
	return 1;
}
static int hw_destroy(ENGINE *e)
{
	printf("call hw_destroy\n");
	return 1;
}
static int hw_finish(ENGINE *e)
{
	printf("call hw_finish\n");
	return 0;
}
static EVP_PKEY *hw_load_privkey(ENGINE* e, const char* key_id,UI_METHOD *ui_method, void *callback_data)
{
	/* 将密钥id 放在ENGINE 的扩展数据中 */
	int index;
	printf("call hw_load_privkey\n");
	index=0;
	ENGINE_set_ex_data(e, index, (char *)key_id);
	return NULL;
}
#define HW_SET_RSA_PRIVATE_KEY 1
/* 实现自己的控制函数 */
static int hw_ctrl(ENGINE *e, int cmd, long i, void *p, void (*f)(void))
{
	switch(cmd)
	{
	case HW_SET_RSA_PRIVATE_KEY:
		hw_load_privkey(e,(char *)p,NULL,NULL);
		break;
	default:
		printf("err.\n");
		return -1;
	}
	return 0;
}
static EVP_PKEY *hw_load_pubkey(ENGINE* e, const char* key_id,UI_METHOD *ui_method, void *callback_data)
{
	printf("call hw_load_pubkey\n");
	return NULL;
}
static const ENGINE_CMD_DEFN hw_cmd_defns[] = {
	{
		ENGINE_CMD_BASE,   /* The command number */
		"SO_PATH",        /* The command name itself */
		"Specifies the path to the 'hw' shared library",   /* A short description of the command */
		ENGINE_CMD_FLAG_STRING /* The input the command expects */
	},       
	{0, NULL, NULL, 0}
};
static int hw_init_key(EVP_CIPHER_CTX *ctx, const unsigned char *key,
const unsigned char *iv, int enc)
{
	return 1;
}
static int hw_cipher_enc(EVP_CIPHER_CTX *ctx, unsigned char *out,const unsigned char *in, unsigned int inl)
{
	memcpy(out,in,inl);
	return 1;
}
#include <openssl/objects.h>
/* 定义自己的des_ecb 硬件算法*/
static const EVP_CIPHER EVP_hw_c=
{
	NID_des_ecb,
	1,
	8,/* Default value for variable length ciphers */
	0,  //iv_len
	8,  ///* Various flags */
	hw_init_key,  /* init key */
	hw_cipher_enc, /* encrypt/decrypt data */
	NULL,  /* cleanup ctx */
	1,     /* how big ctx->cipher_data needs to be */
	NULL,
	NULL,
	NULL,
	NULL
};
const EVP_CIPHER *EVP_hw_cipher(void)
{
	return(&EVP_hw_c);
}
/* 选择对称计算函数 */
static int cipher_nids[] ={ NID_des_ecb, NID_des_ede3_cbc, 0 };
static int hw_ciphers(ENGINE *e, const EVP_CIPHER **cipher, const int **nids, int nid)
{
	if(cipher==NULL)
	{
		*nids = cipher_nids;
		return (sizeof(cipher_nids)-1)/sizeof(cipher_nids[0]);
	}
	switch (nid)
	{
	case NID_des_ecb:
		*cipher = EVP_hw_cipher();
		break;
	//其他对称函数
	}
	return 1;
}
static int init(EVP_MD_CTX *ctx)
{
	printf("call md init\n");
	return 1;
}
static int update(EVP_MD_CTX *ctx,const void *data,size_t count)
{
	printf("call md update\n");
	return 1;
}
static int final(EVP_MD_CTX *ctx,unsigned char *md)
{
	int i;
	printf("call md final\n");
	for(i=0;i<20;i++)
		memset(md++,i,1);
	return 1;
}
int mySign(int type, const unsigned char *m, unsigned int m_length,
unsigned char *sigret, unsigned int *siglen, void *key)
{
	RSA *k;
	int keyid;
	k=(RSA *)key;
	/* 获取硬件中的私钥ID，进行计算 */
	keyid=(int)ENGINE_get_ex_data(k->engine,0);
	printf("call mySign\n");
	printf("use key id is %d\n",keyid);
	return 1;
}
int myVerify(int type, const unsigned char *m, unsigned int m_length,const unsigned char *sigbuf, unsigned int siglen,void *key)
{
	printf("call myVerify\n");
	return 1;
}
static int digest_nids[] =
{ NID_sha1, NID_md5, 0 };
/* 实现的sha1 摘要算法 */

static const EVP_MD hw_newmd=
{
	NID_sha1,
	NID_sha1WithRSAEncryption,
	SHA_DIGEST_LENGTH,
	0,
	init,
	update,
	final,
	NULL,  //copy
	NULL,  //cleanup
	20,		 //block_size  //TODO
	sizeof(EVP_MD *)+sizeof(SHA_CTX),		//ctx_size
	NULL,		//int (*md_ctrl) (EVP_MD_CTX *ctx, int cmd, int p1, void *p2);
				//mySign,     
				//myVerify, 
				//sizeof(EVP_MD *)+sizeof(SHA_CTX),
				//6
};
/*

static  EVP_MD hw_newmd=
{
	NID_sha1,
	NID_sha1WithRSAEncryption,
	SHA_DIGEST_LENGTH,
	0,
	init,
	update,
	final,
	NULL,
	NULL,
	(int)mySign, 
	(int)myVerify, 
	//sizeof(EVP_MD *)+sizeof(SHA_CTX),
	NULL
};
*/


static EVP_MD * EVP_hw_md()
{
	return (EVP_MD *)(&hw_newmd);
}
/* 选择摘要算法的函数 */
static int hw_md(ENGINE *e, const EVP_MD **digest,const int **nids, int nid)
{
	if(digest==NULL)
	{
		*nids = digest_nids;
		return (sizeof(digest_nids)-1)/sizeof(digest_nids[0]);
	}
	switch (nid)
	{
	case NID_sha1:
		*digest = EVP_hw_md();
		break;
	//其他摘要函数
	}
	return 1;
}

static int bind_helper(ENGINE *e)
{
	int ret;
	ret=ENGINE_set_id(e, engine_hw_id);//设置Engine 的id
	if(ret!=1)
	{
		printf("ENGINE_set_id failed\n");
		return 0;
	}
	ret=ENGINE_set_name(e, engine_hw_name);//设置Engine 名字
	if(ret!=1)
	{
		printf("ENGINE_set_name failed\n");
		return 0;
	}
	ret=ENGINE_set_RSA(e, &hw_rsa);
	if(ret!=1)
	{
		printf("ENGINE_set_RSA failed\n");
		return 0;
	}
	ret=ENGINE_set_RAND(e, &hw_rand);
	if(ret!=1)
	{
		printf("ENGINE_set_RAND failed\n");
		return 0;
	}
	ret=ENGINE_set_destroy_function(e, hw_destroy);
	if(ret!=1)
	{
		printf("ENGINE_set_destroy_function failed\n");
		return 0;
	}
	ret=ENGINE_set_init_function(e, hw_init);
	if(ret!=1)
	{
		printf("ENGINE_set_init_function failed\n");
		return 0;
	}
	ret=ENGINE_set_finish_function(e, hw_finish);
	if(ret!=1)
	{
		printf("ENGINE_set_finish_function failed\n");
		return 0;
	}
	ret=ENGINE_set_ctrl_function(e, hw_ctrl);
	if(ret!=1)
	{
		printf("ENGINE_set_ctrl_function failed\n");
		return 0;
	}
	ret=ENGINE_set_load_privkey_function(e, hw_load_privkey);
	if(ret!=1)
	{
		printf("ENGINE_set_load_privkey_function failed\n");
		return 0;
	}
	ret=ENGINE_set_load_pubkey_function(e, hw_load_pubkey);
	if(ret!=1)
	{
		printf("ENGINE_set_load_pubkey_function failed\n");
		return 0;
	}
	ret=ENGINE_set_cmd_defns(e, hw_cmd_defns);
	if(ret!=1)
	{
		printf("ENGINE_set_cmd_defns failed\n");
		return 0;
	}
	ret=ENGINE_set_ciphers(e,hw_ciphers);
	if(ret!=1)
	{
		printf("ENGINE_set_ciphers failed\n");
		return 0;
	}
	ret=ENGINE_set_digests(e,hw_md);
	if(ret!=1)
	{
		printf("ENGINE_set_digests failed\n");
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
#define HW_set_private_keyID(a) func(e,a,0,(void *)1,NULL)
#include <openssl/engine.h>
#include <openssl/evp.h>

int main(){
	ENGINE *e;
	//RSA_METHOD *meth;
	int ret,num=20,i;
	char buf[20],*name;
	const EVP_CIPHER *cipher;
	const EVP_MD *md;
	EVP_MD_CTX mctx,md_ctx;
	EVP_CIPHER_CTX ciph_ctx={0},dciph_ctx={0};
	unsigned char key[8],iv[8];
	unsigned char in[50],out[100],dd[60];
	int inl,outl,total,dtotal;
	RSA *rkey;
	//RSA_METHOD *rsa_m;
	//EVP_PKEY *ek;
	EVP_PKEY *pkey;
	ENGINE_CTRL_FUNC_PTR func;
	OpenSSL_add_all_algorithms();
	ENGINE_load_hwcipher();

	e=ENGINE_by_id("ID_hw");
	name = (char *)ENGINE_get_name(e);
	printf("engine name :%s \n",name);
	/* 随机数生成 */
	ret=RAND_set_rand_engine(e);
	if(ret!=1)
	{
		printf("RAND_set_rand_engine err\n");
		return -1;
	}
	ret=ENGINE_set_default_RAND(e);
	if(ret!=1)
	{
		printf("ENGINE_set_default_RAND err\n");
		return -1;
	}
	ret=RAND_bytes((unsigned char *)buf,num);
	/* 对称加密 */
	for(i=0;i<8;i++)
		memset(&key[i],i,1);
	memset(&ciph_ctx,0,sizeof(ciph_ctx));
	EVP_CIPHER_CTX_init(&ciph_ctx);
	/* 采用Engine 对称算法 */
	cipher=EVP_des_ecb();
	ret=EVP_EncryptInit_ex(&ciph_ctx,cipher,e,key,iv);
	if(ret!=1)
	{
		printf("EVP_EncryptInit_ex err\n");
		return -1;
	}
	strcpy((char *)in,"zcpsssssssssssss");
	inl=strlen((const char *)in);
	total=0;
	ret=EVP_EncryptUpdate(&ciph_ctx,out,&outl,in,inl);
	if(ret!=1)
	{
		printf("EVP_EncryptUpdate err\n");
		return -1;
	}
	total+=outl;
	ret=EVP_EncryptFinal(&ciph_ctx,out+total,&outl);
	if(ret!=1)
	{
		printf("EVP_EncryptFinal err\n");
		return -1;
	}
	total+=outl;
	/* 解密 */
	dtotal=0;
	memset(&dciph_ctx,0,sizeof(dciph_ctx));
	EVP_CIPHER_CTX_init(&dciph_ctx);
	ret=EVP_DecryptInit_ex(&dciph_ctx,cipher,e,key,iv);
	if(ret!=1)
	{
		printf("EVP_DecryptInit_ex err\n");
		return -1;
	}
	ret=EVP_DecryptUpdate(&dciph_ctx,dd,&outl,out,total);
	if(ret!=1)
	{
		printf("EVP_DecryptUpdate err\n");
		return -1;
	}
	dtotal+=outl;
	ret=EVP_DecryptFinal(&dciph_ctx,dd+dtotal,&outl);
	if(ret!=1)
	{
		printf("EVP_DecryptFinal err\n");
		return -1;
	}
	dtotal+=outl;
	/* Engine 摘要 */
	memset(&mctx,0,sizeof(mctx));
	EVP_MD_CTX_init(&mctx);
	md=EVP_sha1();
	ret=EVP_DigestInit_ex(&mctx,md,e);
	if(ret!=1)
	{
		printf("EVP_DigestInit_ex err.\n");
		return -1;
	}
	ret=EVP_DigestUpdate(&mctx,in,inl);
	if(ret!=1)
	{
		printf("EVP_DigestInit_ex err.\n");
		return -1;
	}
	ret=EVP_DigestFinal(&mctx,out,(unsigned int *)&outl);
	if(ret!=1)
	{
		printf("EVP_DigestInit_ex err.\n");
		return -1;
	}
	func=ENGINE_get_ctrl_function(e);
	/* 设置计算私钥ID */
	func(e,1,0,(void *)0,NULL);//HW_set_private_keyID(1);
	rkey=RSA_new_method(e);
	pkey=EVP_PKEY_new();
	EVP_PKEY_set1_RSA(pkey,rkey);
	memset(&md_ctx,0,sizeof(md_ctx));
	EVP_MD_CTX_init(&md_ctx);
	ret=EVP_SignInit_ex(&md_ctx,EVP_sha1(),e);
	if(ret!=1)
	{
		printf("EVP_SignInit_ex err\n");
		return -1;
	}
	ret=EVP_SignUpdate(&md_ctx,in,inl);
	if(ret!=1)
	{
		printf("EVP_SignUpdate err\n");
		return -1;
	}
	ret=EVP_SignFinal(&md_ctx,out,(unsigned int *)&outl,pkey); // error?
	if(ret!=1)
	{
		printf("EVP_SignFinal err\n");
		return -1;
	}
	/* 私钥加密 */
	RSA_private_encrypt(inl,in,out,rkey,1);
	/* 公钥解密 */
	/* 公钥加密 */
	/* 私钥解密 */
	printf("all test ok.\n");
	ENGINE_free(e);
	ENGINE_finish(e);
	return 0;
}
