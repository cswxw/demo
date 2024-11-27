/********************************************************************
 * author: waponx
 * email : 18826419224@163.com
 * time  : 10.11.2015
 * ******************************************************************/

#ifndef _MAIL_BASE64ENCODE_H
#define _MAIL_BASE64ENCODE_H

#include <string>

std::string Base64Encode(const std::string& src) {
	using std::string;
    int i, j, srcLen = src.length();
    string dst(srcLen / 3 * 4 + 4, 0);
    for(i = 0, j= 0; i <=srcLen - 3; i+=3, j+=4) {
	        dst[j] = (src[i] & 0xFC) >> 2;
	        dst[j+1] = ((src[i] & 0x03) << 4) + ((src[i+1] & 0xF0) >> 4);
	        dst[j+2] = ((src[i+1] & 0x0F) << 2) + ((src[i+2] & 0xC0) >> 6);
	        dst[j+3] = src[i+2] & 0x3F;
	    }
    if( srcLen % 3 == 1 ) {
	        dst[j] = (src[i] & 0xFC) >> 2;
	        dst[j+1] = ((src[i] & 0x03) << 4);
	        dst[j+2] = 64;
	        dst[j+3] = 64;
	        j += 4;
	    }
    else if( srcLen % 3 == 2 ) {
	        dst[j] = (src[i] & 0xFC) >> 2;
	        dst[j+1] = ((src[i] & 0x03) << 4) + ((src[i+1] & 0xF0) >> 4);
	        dst[j+2] = ((src[i+1] & 0x0F) << 2);
	        dst[j+3] = 64;
	        j+=4;
	    }

    static unsigned char *base64 = 
		(unsigned char*)("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=");
    for(i = 0; i < j; ++i) {    //map 6 bit value to base64 ASCII character
	        dst[i] = base64[(int)dst[i]];
	    }
    
    return dst;
}

#endif	// _MAIL_BASE64ENCODE_H
