#include <string.h>
#include <openssl/comp.h>
int main()
{
	COMP_CTX *ctx;
	int len,olen=100,ilen=50,i,total=0;
	unsigned char in[50],out[100];
	unsigned char expend[200];
#ifdef _WIN32
	ctx=COMP_CTX_new(COMP_rle());
#else
	/* for linux */
	ctx=COMP_CTX_new(COMP_zlib());
#endif
	for(i=0;i<50;i++)
		memset(&in[i],i,1);
	total=COMP_compress_block(ctx,out,olen,in,50);
	len=COMP_expand_block(ctx,expend,200,out,total);
	COMP_CTX_free(ctx);
	return 0;
}
