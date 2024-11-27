#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <iconv.h>
#include <errno.h>
#include <locale.h>

#ifndef ICONV_CONST
# define ICONV_CONST const
#endif


/*!
 对字符串进行语言编码转换
 param from  原始编码，比如"GB2312",的按照iconv支持的写
 param to      转换的目的编码
 param save  转换后的数据保存到这个指针里，需要在外部分配内存
 param savelen 存储转换后数据的内存大小
 param src      原始需要转换的字符串
 param srclen    原始字符串长度
 */
int convert(const char *from, const char *to, char* save, int savelen, char *src, int srclen)
{
    iconv_t cd;
    char   *inbuf = src;
    char *outbuf = save;
    size_t outbufsize = savelen;
    int status = 0;
    size_t  savesize = 0;
    size_t inbufsize = srclen;
    const char* inptr = inbuf;
    size_t      insize = inbufsize;
    char* outptr = outbuf;
    size_t outsize = outbufsize;
    
    cd = iconv_open(to, from);
    iconv(cd,NULL,NULL,NULL,NULL);
    if (inbufsize == 0) {
        status = -1;
        goto done;
    }
    while (insize > 0) {
        size_t res = iconv(cd,( char**)&inptr,&insize,&outptr,&outsize);
        if (outptr != outbuf) {
            int saved_errno = errno;
            int outsize = outptr - outbuf;
            strncpy(save+savesize, outbuf, outsize);
            errno = saved_errno;
        }
        if (res == (size_t)(-1)) {
#if 0
            if (errno == EILSEQ) {
                int one = 1;
                iconvctl(cd,ICONV_SET_DISCARD_ILSEQ,&one);
                status = -3;
            } else if (errno == EINVAL) {
                if (inbufsize == 0) {
                    status = -4;
                    goto done;
                } else {
                    break;
                }
            } else if (errno == E2BIG) {
                status = -5;
                goto done;
            } else {
                status = -6;
                goto done;
            }
#endif
						goto done;
        }
    }
    status = strlen(save);
done:
    iconv_close(cd);
    return status;
}


bool gbk_to_utf8 (char *inbuf, size_t *inlen, char *outbuf, size_t *outlen)
{
  /* 目的编码, TRANSLIT：遇到无法转换的字符就找相近字符替换
   *           IGNORE ：遇到无法转换字符跳过*/
  char *encTo = "UTF-8//IGNORE";

/* 源编码 */
  char *encFrom = "GBK";

  /* 获得转换句柄
   *@param encTo 目标编码方式
   *@param encFrom 源编码方式
   *
   * */
  iconv_t cd = iconv_open (encTo, encFrom);
  if (cd == (iconv_t)-1)
  {
     perror ("iconv_open");
  }

  /* 需要转换的字符串 */
  //printf("inbuf=%s\n", inbuf);

  /* 打印需要转换的字符串的长度 */
  //printf("inlen=%d\n", *inlen);


  /* 由于iconv()函数会修改指针，所以要保存源指针 */
  char *tmpin = inbuf;
  char *tmpout = outbuf;
  size_t insize = *inlen;
  size_t outsize = *outlen;

  /* 进行转换
   *@param cd iconv_open()产生的句柄
   *@param srcstart 需要转换的字符串
   *@param inlen 存放还有多少字符没有转换
   *@param tempoutbuf 存放转换后的字符串
   *@param outlen 存放转换后,tempoutbuf剩余的空间
   *
   * */
  size_t ret = iconv (cd, &tmpin, inlen, &tmpout, outlen);
  if (ret == -1)
  {
     perror ("iconv");
  }

  /* 存放转换后的字符串 */
  //printf("outbuf=%s\n", outbuf);

  //存放转换后outbuf剩余的空间
  //printf("outlen=%d\n", *outlen);
/*
  int i = 0;

  for (i=0; i<(outsize- (*outlen)); i++)
  {
     //printf("%2c", outbuf[i]);
     //printf("%x\n", outbuf[i]);
  }
*/
  /* 关闭句柄 */
  iconv_close (cd);

  return 0;
}


bool utf8_to_gbk (char *inbuf, size_t *inlen, char *outbuf, size_t *outlen)
{

  /* 目的编码, TRANSLIT：遇到无法转换的字符就找相近字符替换
   *           IGNORE ：遇到无法转换字符跳过*/
  char *encTo = "GBK//IGNORE";
  /* 源编码 */
  char *encFrom = "UTF-8";

  /* 获得转换句柄
   *@param encTo 目标编码方式
   *@param encFrom 源编码方式
   *
   * */
  iconv_t cd = iconv_open (encTo, encFrom);
  if (cd == (iconv_t)-1)
  {
      perror ("iconv_open");
  }

  /* 需要转换的字符串 */
  //printf("inbuf=%s\n", inbuf);

  /* 打印需要转换的字符串的长度 */
 // printf("inlen=%d\n", *inlen);

  /* 由于iconv()函数会修改指针，所以要保存源指针 */
  char *tmpin = inbuf;
  char *tmpout = outbuf;
  size_t insize = *inlen;
  size_t outsize = *outlen;

  /* 进行转换
   *@param cd iconv_open()产生的句柄
   *@param srcstart 需要转换的字符串
   *@param inlen 存放还有多少字符没有转换
   *@param tempoutbuf 存放转换后的字符串
   *@param outlen 存放转换后,tempoutbuf剩余的空间
   *
   * */
  size_t ret = iconv (cd, &tmpin, inlen, &tmpout, outlen);
  if (ret == -1)
  {
     perror ("iconv");
  }

  /* 存放转换后的字符串 */
  //printf("outbuf=%s\n", outbuf);

  //存放转换后outbuf剩余的空间
  //printf("outlen=%d\n", *outlen);

  int i = 0;
/*
  for (i=0; i<(outsize- (*outlen)); i++)
  {
     //printf("%2c", outbuf[i]);
     printf("%x\n", outbuf[i]);
  }
*/
  /* 关闭句柄 */
  iconv_close (cd);

  return 0;
}

int main ()
{
  /* 需要转换的字符串 */
  char text[1024] = "abcd汉asdfsa哈哈哈哈行"; 
  //char *text = "汉";    
    
  char inbuf[1024] = {};
  strcpy (inbuf, text);
  size_t inlen = strlen (inbuf);

  /* 存放转换后的字符串 */
  char outbuf[1024] = {};
  size_t outlen = 1024;

  utf8_to_gbk (inbuf, &inlen, outbuf, &outlen);
  printf ("print outbuf: %s\n", outbuf);

  size_t outsize = strlen(outbuf);
  size_t insize = 1024;
  char instr[1024] = {};
  gbk_to_utf8 (outbuf, &outsize, instr, &insize);
  printf ("print buf: %s\n", instr);
  return 0;
}

