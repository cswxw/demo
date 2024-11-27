#include <openssl/evp.h>
#include <openssl/cmac.h>
#include <openssl/gmsdf.h>
#include <openssl/gmsaf.h>
#include <openssl/engine.h>
#include <openssl/err.h>
#include <string.h>
#include <openssl/evp.h>
struct evp_Encode_Ctx_st {
    /* number saved in a partial encode/decode */
    int num;
    /*
     * The length is either the output line length (in input bytes) or the
     * shortest input line length that is ok.  Once decoding begins, the
     * length is adjusted up each time a longer line is decoded
     */
    int length;
    /* data to encode */
    unsigned char enc_data[80];
    /* number read on current line */
    int line_num;
    int expect_nl;
};
int main()
{
EVP_ENCODE_CTX ectx,dctx;
unsigned char in[500],out[800],d[500];
int inl,outl,i,total,ret,total2;
EVP_EncodeInit(&ectx);
for(i=0;i<500;i++)
memset(&in[i],i,1);
inl=500;
total=0;
EVP_EncodeUpdate(&ectx,out,&outl,in,inl);
total+=outl;
EVP_EncodeFinal(&ectx,out+total,&outl);
total+=outl;
printf("%s\n",out);
EVP_DecodeInit(&dctx);
outl=500;
total2=0;
ret=EVP_DecodeUpdate(&dctx,d,&outl,out,total);
if(ret<0)
{
printf("EVP_DecodeUpdate err!\n");
return -1;
}
total2+=outl;
ret=EVP_DecodeFinal(&dctx,d,&outl);
total2+=outl;
return 0;
}
