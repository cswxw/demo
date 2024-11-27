#include "MyMemory.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <memory.h>
#include <time.h>

static bool use_heap_api = false;
static HANDLE heap_handle = NULL;

/*********************************** String start ******************************/
// Get the length of the string
UINT StrLen(char *str)
{
	// Validate arguments
	if (str == NULL)
	{
		return 0;
	}


	return (UINT)strlen(str);
}

// Memory copy
void Copy(void *dst, void *src, UINT size)
{
	// Validate arguments
	if (dst == NULL || src == NULL || size == 0 || dst == src)
	{
		return;
	}


	memcpy(dst, src, size);
}

// Convert a hexadecimal string to a 4 bit integer
UINT HexTo4Bit(char c)
{
	if ('0' <= c && c <= '9')
	{
		return c - '0';
	}
	else if ('a' <= c && c <= 'f')
	{
		return c - 'a' + 10;
	}
	else if ('A' <= c && c <= 'F')
	{
		return c - 'A' + 10;
	}
	else
	{
		return 0;
	}
}


// Uncapitalize a character
char ToLower(char c)
{
	if ('A' <= c && c <= 'Z')
	{
		c += 'z' - 'Z';
	}
	return c;
}

// Capitalize a character
char ToUpper(char c)
{
	if ('a' <= c && c <= 'z')
	{
		c += 'Z' - 'z';
	}
	return c;
}
// Compare the strings in case-insensitive mode
int StrCmpi(char *str1, char *str2)
{
	UINT i;
	// Validate arguments
	if (str1 == NULL && str2 == NULL)
	{
		return 0;
	}
	if (str1 == NULL)
	{
		return 1;
	}
	if (str2 == NULL)
	{
		return -1;
	}

	// String comparison
	i = 0;
	while (true)
	{
		char c1, c2;
		c1 = ToUpper(str1[i]);
		c2 = ToUpper(str2[i]);
		if (c1 > c2)
		{
			return 1;
		}
		else if (c1 < c2)
		{
			return -1;
		}
		if (str1[i] == 0 || str2[i] == 0)
		{
			return 0;
		}
		i++;
	}
}

// Compare the string
int StrCmp(char *str1, char *str2)
{
	// Validate arguments
	if (str1 == NULL && str2 == NULL)
	{
		return 0;
	}
	if (str1 == NULL)
	{
		return 1;
	}
	if (str2 == NULL)
	{
		return -1;
	}

	return strcmp(str1, str2);
}
// Copy a string
UINT StrCpy(char *dst, UINT size, char *src)
{
	UINT len;
	// Validate arguments
	if (dst == src)
	{
		return StrLen(src);
	}
	if (dst == NULL || src == NULL)
	{
		if (src == NULL && dst != NULL)
		{
			if (size >= 1)
			{
				dst[0] = '\0';
			}
		}
		return 0;
	}
	if (size == 1)
	{
		dst[0] = '\0';
		return 0;
	}
	if (size == 0)
	{
		// Ignore the length
		size = 0x7fffffff;
	}

	// Check the length
	len = StrLen(src);
	if (len <= (size - 1))
	{
		Copy(dst, src, len + 1);
	}
	else
	{
		len = size - 1;
		Copy(dst, src, len);
		dst[len] = '\0';
	}


	return len;
}

// Convert the HEX string to a 32 bit integer
UINT HexToInt(char *str)
{
	UINT len, i;
	UINT ret = 0;
	// Validate arguments
	if (str == NULL)
	{
		return 0;
	}

	if (str[0] == '0' && (str[1] == 'x' || str[1] == 'X'))
	{
		str += 2;
	}

	len = StrLen(str);
	for (i = 0;i < len;i++)
	{
		char c = str[i];

		if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F'))
		{
			ret = ret * 16 + (UINT)HexTo4Bit(c);
		}
		else
		{
			break;
		}
	}

	return ret;
}
// Converts a 4 bit value to hexadecimal string
char FourBitToHex(UINT value)
{
	value = value % 16;

	if (value <= 9)
	{
		return '0' + value;
	}
	else
	{
		return 'a' + (value - 10);
	}
}

// Convert a 64 bit integer to a HEX
void ToHex64(char *str, UINT64 value)
{
	char tmp[MAX_SIZE];
	UINT wp = 0;
	UINT len, i;
	// Validate arguments
	if (str == NULL)
	{
		return;
	}

	// Set to empty character
	StrCpy(tmp, 0, "");

	// Append from the last digit
	while (true)
	{
		UINT a = (UINT)(value % (UINT64)16);
		value = value / (UINT)16;
		tmp[wp++] = FourBitToHex(a);
		if (value == 0)
		{
			tmp[wp++] = 0;
			break;
		}
	}

	// Reverse order
	len = StrLen(tmp);
	for (i = 0;i < len;i++)
	{
		str[len - i - 1] = tmp[i];
	}
	str[len] = 0;
}

// String comparison function
int CompareStr(void *p1, void *p2)
{
	char *s1, *s2;
	if (p1 == NULL || p2 == NULL)
	{
		return 0;
	}
	s1 = *(char **)p1;
	s2 = *(char **)p2;

	return StrCmpi(s1, s2);
}


// Copy a string
char *CopyStr(char *str)
{
	UINT len;
	char *dst;
	// Validate arguments
	if (str == NULL)
	{
		return NULL;
	}

	len = StrLen(str);
	dst =(char *) Malloc(len + 1);
	StrCpy(dst, len + 1, str);
	return dst;
}
// Convert Unicode string to ANSI string
UINT UniToStr(char *str, UINT size, wchar_t *s)
{
	UINT ret;
	char *tmp;
	UINT new_size;
	// Validate arguments
	if (s == NULL || str == NULL)
	{
		return 0;
	}

	new_size = CalcUniToStr(s);
	if (new_size == 0)
	{
		if (size >= 1)
		{
			StrCpy(str, 0, "");
		}
		return 0;
	}
	tmp = (char*)Malloc(new_size);
	tmp[0] = 0;
	wcstombs(tmp, s, new_size);
	tmp[new_size - 1] = 0;
	ret = StrCpy(str, size, tmp);
	Free(tmp);

	return ret;
}
// Get the required number of bytes to convert Unicode string to the ANSI string
UINT CalcUniToStr(wchar_t *s)
{
	UINT ret;
	// Validate arguments
	if (s == NULL)
	{
		return 0;
	}

	ret = (UINT)wcstombs(NULL, s, UniStrLen(s));
	if (ret == (UINT)-1)
	{
		return 0;
	}

	return ret + 1;

}
// Format the string
void UniFormat(wchar_t *buf, UINT size, wchar_t *fmt, ...)
{
	va_list args;
	// Validate arguments
	if (buf == NULL || fmt == NULL)
	{
		return;
	}

	va_start(args, fmt);
	vswprintf(buf, size, fmt, args);
	va_end(args);
}

// Compare the strings in case-insensitive mode
int UniStrCmpi(wchar_t *str1, wchar_t *str2)
{
	UINT i;
	// Validate arguments
	if (str1 == NULL && str2 == NULL)
	{
		return 0;
	}
	if (str1 == NULL)
	{
		return 1;
	}
	if (str2 == NULL)
	{
		return -1;
	}

	// String comparison
	i = 0;
	while (true)
	{
		wchar_t c1, c2;
		c1 = UniToUpper(str1[i]);
		c2 = UniToUpper(str2[i]);
		if (c1 > c2)
		{
			return 1;
		}
		else if (c1 < c2)
		{
			return -1;
		}
		if (str1[i] == 0 || str2[i] == 0)
		{
			return 0;
		}
		i++;
	}
}

// Capitalize a character
wchar_t UniToUpper(wchar_t c)
{
	if (c >= L'a' && c <= L'z')
	{
		c -= L'a' - L'A';
	}

	return c;
}
// Unicode string copy
wchar_t *UniCopyStr(wchar_t *str)
{
	return CopyUniStr(str);
}
// Copy the Unicode string
wchar_t *CopyUniStr(wchar_t *str)
{
	UINT len;
	wchar_t *dst;
	// Validate arguments
	if (str == NULL)
	{
		return NULL;
	}

	len = UniStrLen(str);
	dst = (wchar_t *)Malloc((len + 1) * sizeof(wchar_t));
	UniStrCpy(dst, 0, str);

	return dst;
}

// Copy a Unicode string to ANSI string
char *CopyUniToStr(wchar_t *unistr)
{
	char *str;
	UINT str_size;
	// Validate arguments
	if (unistr == NULL)
	{
		return NULL;
	}

	str_size = CalcUniToStr(unistr);
	if (str_size == 0)
	{
		return CopyStr("");
	}
	str = (char*)Malloc(str_size);
	UniToStr(str, str_size, unistr);

	return str;
}
// Check whether str starts with the key
bool UniStartWith(wchar_t *str, wchar_t *key)
{
	UINT str_len;
	UINT key_len;
	wchar_t *tmp;
	bool ret;
	// Validate arguments
	if (str == NULL || key == NULL)
	{
		return false;
	}

	// Comparison
	str_len = UniStrLen(str);
	key_len = UniStrLen(key);
	if (str_len < key_len)
	{
		return false;
	}
	if (str_len == 0 || key_len == 0)
	{
		return false;
	}
	tmp = CopyUniStr(str);
	tmp[key_len] = 0;

	if (UniStrCmpi(tmp, key) == 0)
	{
		ret = true;
	}
	else
	{
		ret = false;
	}

	Free(tmp);

	return ret;
}
// Check whether str ends with the key
bool UniEndWith(wchar_t *str, wchar_t *key)
{
	UINT str_len;
	UINT key_len;
	// Validate arguments
	if (str == NULL || key == NULL)
	{
		return false;
	}

	// Comparison
	str_len = UniStrLen(str);
	key_len = UniStrLen(key);
	if (str_len < key_len)
	{
		return false;
	}

	if (UniStrCmpi(str + (str_len - key_len), key) == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}
// String concatenation
UINT UniStrCat(wchar_t *dst, UINT size, wchar_t *src)
{
	UINT len1, len2, len_test;
	// Validate arguments
	if (dst == NULL || src == NULL)
	{
		return 0;
	}
	if (size != 0 && size < sizeof(wchar_t))
	{
		return 0;
	}
	if (size == sizeof(wchar_t))
	{
		wcscpy(dst, L"");
		return 0;
	}
	if (size == 0)
	{
		// Ignore the length
		size = 0x3fffffff;
	}

	len1 = UniStrLen(dst);
	len2 = UniStrLen(src);
	len_test = len1 + len2 + 1;
	if (len_test > (size / sizeof(wchar_t)))
	{
		if (len2 <= (len_test - (size / sizeof(wchar_t))))
		{
			return 0;
		}
		len2 -= len_test - (size / sizeof(wchar_t));
	}
	Copy(&dst[len1], src, len2 * sizeof(wchar_t));
	dst[len1 + len2] = 0;

	return len1 + len2;
}
// Remove white space of the both side of the string
void UniTrim(wchar_t *str)
{
	// Validate arguments
	if (str == NULL)
	{
		return;
	}

	UniTrimLeft(str);
	UniTrimRight(str);
}
// Remove white space on the right side of the string
void UniTrimRight(wchar_t *str)
{
	wchar_t *buf, *tmp;
	UINT len, i, wp, wp2;
	bool flag;
	// Validate arguments
	if (str == NULL)
	{
		return;
	}
	len = UniStrLen(str);
	if (len == 0)
	{
		return;
	}
	if (str[len - 1] != L' ' && str[len - 1] != L'\t')
	{
		return;
	}

	buf = (wchar_t *)Malloc((len + 1) * sizeof(wchar_t));
	tmp = (wchar_t *)Malloc((len + 1) * sizeof(wchar_t));
	flag = false;
	wp = wp2 = 0;
	for (i = 0;i < len;i++)
	{
		if (str[i] != L' ' && str[i] != L'\t')
		{
			Copy(&buf[wp], tmp, wp2 * sizeof(wchar_t));
			wp += wp2;
			wp2 = 0;
			buf[wp++] = str[i];
		}
		else
		{
			tmp[wp2++] = str[i];
		}
	}
	buf[wp] = 0;
	UniStrCpy(str, 0, buf);
	Free(buf);
	Free(tmp);
}

// Remove white space from the left side of the string
void UniTrimLeft(wchar_t *str)
{
	wchar_t *buf;
	UINT len, i, wp;
	bool flag;
	// Validate arguments
	if (str == NULL)
	{
		return;
	}
	len = UniStrLen(str);
	if (len == 0)
	{
		return;
	}
	if (str[0] != L' ' && str[0] != L'\t')
	{
		return;
	}

	buf = (wchar_t *)Malloc((len + 1) * sizeof(wchar_t));
	flag = false;
	wp = 0;
	for (i = 0;i < len;i++)
	{
		if (str[i] != L' ' && str[i] != L'\t')
		{
			flag = true;
		}
		if (flag)
		{
			buf[wp++] = str[i];
		}
	}
	buf[wp] = 0;
	UniStrCpy(str, 0, buf);
	Free(buf);
}
// Parse the token
UNI_TOKEN_LIST *UniParseToken(wchar_t *src, wchar_t *separator)
{

	UNI_TOKEN_LIST *ret;
	wchar_t *tmp;
	wchar_t *str1, *str2;
	UINT len, num;



	// Validate arguments
	if (src == NULL)
	{
		ret = (UNI_TOKEN_LIST*)ZeroMalloc(sizeof(UNI_TOKEN_LIST));
		ret->Token = (wchar_t**)ZeroMalloc(0);
		return ret;
	}
	if (separator == NULL)
	{
		separator = L" .\t\r\n";
	}
	len = UniStrLen(src);
	str1 = (wchar_t *)Malloc((len + 1) * sizeof(wchar_t));
	str2 = (wchar_t *)Malloc((len + 1) * sizeof(wchar_t));
	UniStrCpy(str1, 0, src);
	UniStrCpy(str2, 0, src);

	//Lock(token_lock);
	{
		tmp = wcstok(str1, separator

			);
		num = 0;
		while (tmp != NULL)
		{
			num++;
			tmp = wcstok(NULL, separator

				);
		}
		ret = (UNI_TOKEN_LIST*)Malloc(sizeof(UNI_TOKEN_LIST));
		ret->NumTokens = num;
		ret->Token = (wchar_t **)Malloc(sizeof(wchar_t *) * num);
		num = 0;
		tmp = wcstok(str2, separator

			);
		while (tmp != NULL)
		{
			ret->Token[num] = (wchar_t *)Malloc((UniStrLen(tmp) + 1) * sizeof(wchar_t));
			UniStrCpy(ret->Token[num], 0, tmp);
			num++;
			tmp = wcstok(NULL, separator
				);
		}
	}
	//Unlock(token_lock);

	Free(str1);
	Free(str2);
	return ret;

}

// Release of the token list
void UniFreeToken(UNI_TOKEN_LIST *tokens)
{
	UINT i;
	if (tokens == NULL)
	{
		return;
	}
	for (i = 0;i < tokens->NumTokens;i++)
	{
		Free(tokens->Token[i]);
	}
	Free(tokens->Token);
	Free(tokens);
}
// Convert a Unicode token list to a token list
TOKEN_LIST *UniTokenListToTokenList(UNI_TOKEN_LIST *src)
{
	TOKEN_LIST *ret;
	UINT i;
	// Validate arguments
	if (src == NULL)
	{
		return NULL;
	}

	ret = (TOKEN_LIST *)ZeroMalloc(sizeof(TOKEN_LIST));
	ret->NumTokens = src->NumTokens;
	ret->Token = (char**)ZeroMalloc(sizeof(char *) * ret->NumTokens);

	for (i = 0;i < ret->NumTokens;i++)
	{
		ret->Token[i] = CopyUniToStr(src->Token[i]);
	}

	return ret;
}
// Remove white spaces of the both side of the string
void Trim(char *str)
{
	// Validate arguments
	if (str == NULL)
	{
		return;
	}

	// Trim on the left side
	TrimLeft(str);

	// Trim on the right side
	TrimRight(str);
}

// Remove white spaces on the right side of the string
void TrimRight(char *str)
{
	char *buf, *tmp;
	UINT len, i, wp, wp2;
	BOOL flag;
	// Validate arguments
	if (str == NULL)
	{
		return;
	}
	len = StrLen(str);
	if (len == 0)
	{
		return;
	}
	if (str[len - 1] != ' ' && str[len - 1] != '\t')
	{
		return;
	}

	buf = (char*)Malloc(len + 1);
	tmp = (char*)Malloc(len + 1);
	flag = FALSE;
	wp = 0;
	wp2 = 0;
	for (i = 0;i < len;i++)
	{
		if (str[i] != ' ' && str[i] != '\t')
		{
			Copy(buf + wp, tmp, wp2);
			wp += wp2;
			wp2 = 0;
			buf[wp++] = str[i];
		}
		else
		{
			tmp[wp2++] = str[i];
		}
	}
	buf[wp] = 0;
	StrCpy(str, 0, buf);
	Free(buf);
	Free(tmp);
}

// Remove white spaces from the left side of the string
void TrimLeft(char *str)
{
	char *buf;
	UINT len, i, wp;
	BOOL flag;
	// Validate arguments
	if (str == NULL)
	{
		return;
	}
	len = StrLen(str);
	if (len == 0)
	{
		return;
	}
	if (str[0] != ' ' && str[0] != '\t')
	{
		return;
	}

	buf = (char*)Malloc(len + 1);
	flag = FALSE;
	wp = 0;
	for (i = 0;i < len;i++)
	{
		if (str[i] != ' ' && str[i] != '\t')
		{
			flag = TRUE;
		}
		if (flag)
		{
			buf[wp++] = str[i];
		}
	}
	buf[wp] = 0;
	StrCpy(str, 0, buf);
	Free(buf);
}
// Release of the token list
void FreeToken(TOKEN_LIST *tokens)
{
	UINT i;
	if (tokens == NULL)
	{
		return;
	}
	for (i = 0;i < tokens->NumTokens;i++)
	{
		if (tokens->Token[i] != 0)
		{
			Free(tokens->Token[i]);
		}
	}
	Free(tokens->Token);
	Free(tokens);
}
// Get the buffer size needed to store the string
UINT UniStrSize(wchar_t *str)
{
	// Validate arguments
	if (str == NULL)
	{
		return 0;
	}

	return (UniStrLen(str) + 1) * sizeof(wchar_t);
}
// Get the length of the string
UINT UniStrLen(wchar_t *str)
{
	UINT i;
	// Validate arguments
	if (str == NULL)
	{
		return 0;
	}

	i = 0;
	while (true)
	{
		if (str[i] == 0)
		{
			break;
		}
		i++;
	}

	return i;
}
// Get the buffer size when converted UTF-8 to Unicode
UINT CalcUtf8ToUni(BYTE *u, UINT u_size)
{
	// Validate arguments
	if (u == NULL)
	{
		return 0;
	}
	if (u_size == 0)
	{
		u_size = StrLen((char *)u);
	}

	return (Utf8Len(u, u_size) + 1) * sizeof(wchar_t);
}

// Get the number of characters in UTF-8 string
UINT Utf8Len(BYTE *u, UINT size)
{
	UINT i, num;
	// Validate arguments
	if (u == NULL)
	{
		return 0;
	}
	if (size == 0)
	{
		size = StrLen((char *)u);
	}

	i = num = 0;
	while (true)
	{
		UINT type;

		type = GetUtf8Type(u, size, i);
		if (type == 0)
		{
			break;
		}
		i += type;
		num++;
	}

	return num;
}
// Convert an Unicode string to UTF-8 string
UINT UniToUtf8(BYTE *u, UINT size, wchar_t *s)
{
	UINT i, len, type, wp;
	// Validate arguments
	if (u == NULL || s == NULL)
	{
		return 0;
	}
	if (size == 0)
	{
		size = 0x3fffffff;
	}

	len = UniStrLen(s);
	wp = 0;
	for (i = 0;i < len;i++)
	{
		BYTE c1, c2;
		wchar_t c = s[i];

		if (IsBigEndian())
		{
			if (sizeof(wchar_t) == 2)
			{
				c1 = ((BYTE *)&c)[0];
				c2 = ((BYTE *)&c)[1];
			}
			else
			{
				c1 = ((BYTE *)&c)[2];
				c2 = ((BYTE *)&c)[3];
			}
		}
		else
		{
			c1 = ((BYTE *)&c)[1];
			c2 = ((BYTE *)&c)[0];
		}

		type = GetUniType(s[i]);
		switch (type)
		{
		case 1:
			if (wp < size)
			{
				u[wp++] = c2;
			}
			break;
		case 2:
			if (wp < size)
			{
				u[wp++] = 0xc0 | (((((c1 & 0x07) << 2) & 0x1c)) | (((c2 & 0xc0) >> 6) & 0x03));
			}
			if (wp < size)
			{
				u[wp++] = 0x80 | (c2 & 0x3f);
			}
			break;
		case 3:
			if (wp < size)
			{
				u[wp++] = 0xe0 | (((c1 & 0xf0) >> 4) & 0x0f);
			}
			if (wp < size)
			{
				u[wp++] = 0x80 | (((c1 & 0x0f) << 2) & 0x3c) | (((c2 & 0xc0) >> 6) & 0x03);
			}
			if (wp < size)
			{
				u[wp++] = 0x80 | (c2 & 0x3f);
			}
			break;
		}
	}
	if (wp < size)
	{
		u[wp] = 0;
	}
	return wp;
}

// Get the number of bytes of a first character of the offset address of the UTF-8 string that starts with s
UINT GetUtf8Type(BYTE *s, UINT size, UINT offset)
{
	// Validate arguments
	if (s == NULL)
	{
		return 0;
	}
	if ((offset + 1) > size)
	{
		return 0;
	}
	if ((s[offset] & 0x80) == 0)
	{
		// 1 byte
		return 1;
	}
	if ((s[offset] & 0x20) == 0)
	{
		// 2 bytes
		if ((offset + 2) > size)
		{
			return 0;
		}
		return 2;
	}
	// 3 bytes
	if ((offset + 3) > size)
	{
		return 0;
	}
	return 3;
}

// Type of the converted character 'c' to UTF-8 (in bytes)
UINT GetUniType(wchar_t c)
{
	BYTE c1, c2;

	if (IsBigEndian())
	{
		if (sizeof(wchar_t) == 2)
		{
			c1 = ((BYTE *)&c)[0];
			c2 = ((BYTE *)&c)[1];
		}
		else
		{
			c1 = ((BYTE *)&c)[2];
			c2 = ((BYTE *)&c)[3];
		}
	}
	else
	{
		c1 = ((BYTE *)&c)[1];
		c2 = ((BYTE *)&c)[0];
	}

	if (c1 == 0)
	{
		if (c2 <= 0x7f)
		{
			// 1 byte
			return 1;
		}
		else
		{
			// 2 bytes
			return 2;
		}
	}
	if ((c1 & 0xf8) == 0)
	{
		// 2 bytes
		return 2;
	}
	// 3 bytes
	return 3;
}
// Convert the UTF-8 strings to a Unicode string
UINT Utf8ToUni(wchar_t *s, UINT size, BYTE *u, UINT u_size)
{
	UINT i, wp, num;
	// Validate arguments
	if (s == NULL || u == NULL)
	{
		return 0;
	}
	if (size == 0)
	{
		size = 0x3fffffff;
	}
	if (u_size == 0)
	{
		u_size = StrLen((char *)u);
	}

	i = 0;
	wp = 0;
	num = 0;
	while (true)
	{
		UINT type;
		wchar_t c;
		BYTE c1, c2;

		type = GetUtf8Type(u, u_size, i);
		if (type == 0)
		{
			break;
		}
		switch (type)
		{
		case 1:
			c1 = 0;
			c2 = u[i];
			break;
		case 2:
			c1 = (((u[i] & 0x1c) >> 2) & 0x07);
			c2 = (((u[i] & 0x03) << 6) & 0xc0) | (u[i + 1] & 0x3f);
			break;
		case 3:
			c1 = ((((u[i] & 0x0f) << 4) & 0xf0)) | (((u[i + 1] & 0x3c) >> 2) & 0x0f);
			c2 = (((u[i + 1] & 0x03) << 6) & 0xc0) | (u[i + 2] & 0x3f);
			break;
		}
		i += type;

		c = 0;

		if (IsBigEndian())
		{
			if (sizeof(wchar_t) == 2)
			{
				((BYTE *)&c)[0] = c1;
				((BYTE *)&c)[1] = c2;
			}
			else
			{
				((BYTE *)&c)[2] = c1;
				((BYTE *)&c)[3] = c2;
			}
		}
		else
		{
			((BYTE *)&c)[0] = c2;
			((BYTE *)&c)[1] = c1;
		}

		if (wp < ((size / sizeof(wchar_t)) - 1))
		{
			s[wp++] = c;
			num++;
		}
		else
		{
			break;
		}
	}

	if (wp < (size / sizeof(wchar_t)))
	{
		s[wp++] = 0;
	}

	return num;
}

// Calculating the length of the string when converting Unicode string to UTF-8 string
UINT CalcUniToUtf8(wchar_t *s)
{
	UINT i, len, size;
	// Validate arguments
	if (s == NULL)
	{
		return 0;
	}

	size = 0;
	len = UniStrLen(s);
	for (i = 0;i < len;i++)
	{
		size += GetUniType(s[i]);
	}

	return size;
}


// Examine whether the string is empty
bool IsEmptyStr(char *str)
{
	char *s;
	// Validate arguments
	if (str == NULL)
	{
		return true;
	}

	s = CopyStr(str);
	Trim(s);

	if (StrLen(s) == 0)
	{
		Free(s);
		return true;
	}
	else
	{
		Free(s);
		return false;
	}
}
/*********************************** String end ******************************/

/*********************************** Other start ******************************/
void AbortExitEx(char *msg)
{
	FILE *f;
	// Validate arguments
	if (msg == NULL)
	{
		msg = "Unknown Error";
	}

	f = fopen("abort_error_log.txt", "w");
	if (f != NULL)
	{
		fwrite(msg, 1, strlen(msg), f);
		fclose(f);
	}

	fputs("Fatal Error: ", stdout);
	fputs(msg, stdout);
	fputs("\r\n", stdout);

	_exit(1);

}

void *ZeroMallocFast(UINT size)
{
	void *p = MallocFast(size);
	Zero(p, size);
	return p;
}


// Malloc
void *Malloc(UINT size)
{
	return calloc(size,sizeof(char));
}

// Release the memory
void Free(void *p){
	// Validate arguments
	if (p == NULL)
	{
		return;
	}
	free(p);
}

// Sleep thread
void SleepThread(UINT time)
{

	Sleep(time);
}

// Clone the memory area
void *Clone(void *addr, UINT size)
{
	void *ret;
	// Validate arguments
	if (addr == NULL)
	{
		return NULL;
	}

	ret = Malloc(size);
	Copy(ret, addr, size);

	return ret;
}

// Comparison of items in the list of integers
int CompareInt(void *p1, void *p2)
{
	UINT *v1, *v2;
	if (p1 == NULL || p2 == NULL)
	{
		return 0;
	}

	v1 = *((UINT **)p1);
	v2 = *((UINT **)p2);
	if (v1 == NULL || v2 == NULL)
	{
		return 0;
	}

	return COMPARE_RET(*v1, *v2);
}

int CompareInt64(void *p1, void *p2)
{
	UINT64 *v1, *v2;
	if (p1 == NULL || p2 == NULL)
	{
		return 0;
	}

	v1 = *((UINT64 **)p1);
	v2 = *((UINT64 **)p2);
	if (v1 == NULL || v2 == NULL)
	{
		return 0;
	}

	return COMPARE_RET(*v1, *v2);
}


// Comparison of memory
UINT Cmp(void *p1, void *p2, UINT size)
{
	UCHAR *c1 = (UCHAR *)p1;
	UCHAR *c2 = (UCHAR *)p2;
	UINT i;

	for (i = 0;i < size;i++)
	{
		if (c1[i] != c2[i])
		{
			if (c1[i] > c2[i])
			{
				return 1;
			}
			else
			{
				return -1;
			}
		}
	}

	return 0;
}

// Endian conversion 16bit
USHORT Endian16(USHORT src)
{
	int x = 1;
	if (*((char *)&x))
	{
		return Swap16(src);
	}
	else
	{
		return src;
	}
}

// Endian conversion 32bit//一般来说，x86 系列 CPU 都是 little-endian 的字节序，PowerPC 通常是 big-endian
UINT Endian32(UINT src)
{
	int x = 1;
	if (*((char *)&x))
	{
		return Swap32(src);//printf("Little Endian"); 
	}
	else
	{
		return src;//printf("Big Endian");
	}
}

// Endian conversion 64bit
UINT64 Endian64(UINT64 src)
{
	int x = 1;
	if (*((char *)&x))
	{
		return Swap64(src);
	}
	else
	{
		return src;
	}
}
// 16-bit swap
USHORT Swap16(USHORT value)
{
	USHORT r;
	((BYTE *)&r)[0] = ((BYTE *)&value)[1];
	((BYTE *)&r)[1] = ((BYTE *)&value)[0];
	return r;
}

// 32bit swap
UINT Swap32(UINT value)
{
	UINT r;
	((BYTE *)&r)[0] = ((BYTE *)&value)[3];
	((BYTE *)&r)[1] = ((BYTE *)&value)[2];
	((BYTE *)&r)[2] = ((BYTE *)&value)[1];
	((BYTE *)&r)[3] = ((BYTE *)&value)[0];
	return r;
}
// 64-bit swap
UINT64 Swap64(UINT64 value)
{
	UINT64 r;
	((BYTE *)&r)[0] = ((BYTE *)&value)[7];
	((BYTE *)&r)[1] = ((BYTE *)&value)[6];
	((BYTE *)&r)[2] = ((BYTE *)&value)[5];
	((BYTE *)&r)[3] = ((BYTE *)&value)[4];
	((BYTE *)&r)[4] = ((BYTE *)&value)[3];
	((BYTE *)&r)[5] = ((BYTE *)&value)[2];
	((BYTE *)&r)[6] = ((BYTE *)&value)[1];
	((BYTE *)&r)[7] = ((BYTE *)&value)[0];
	return r;
}

// Zero-clear of memory
void Zero(void *addr, UINT size)
{
	// Validate arguments
	if (addr == NULL || size == 0)
	{
		return;
	}

	memset(addr, 0, size);
}
// ZeroMalloc
void *ZeroMalloc(UINT size)
{
	void *p = Malloc(size);
	Zero(p, size);
	return p;
}


void ConvertPathW(wchar_t *path)
{
#define PATH_BACKSLASH

	UINT i, len;
#ifdef	PATH_BACKSLASH
	wchar_t new_char = L'\\';
#else
	wchar_t new_char = L'/';
#endif  //PATH_BACKSLASH

	len = UniStrLen(path);
	for (i = 0;i < len;i++)
	{
		if (path[i] == L'\\' || path[i] == L'/')
		{
			path[i] = new_char;
		}
	}
}

UINT64 SystemToUINT64(SYSTEMTIME *st)
{
	UINT64 sec64;
	time_t time;
	// Validate arguments
	if (st == NULL)
	{
		return 0;
	}

	time = SystemToTime(st);
	sec64 = (UINT64)time * (UINT64)1000;
	sec64 += st->wMilliseconds;

	return sec64 - 32400000ULL;
}

time_t SystemToTime(SYSTEMTIME *st)
{

    struct tm gm = {st->wSecond, st->wMinute, st->wHour, st->wDay, st->wMonth-1, st->wYear-1900, st->wDayOfWeek, 0, 0};
    return mktime(&gm);

}
// Convert UINT64 to the SYSTEMTIME
void UINT64ToSystem(SYSTEMTIME *st, UINT64 sec64)
{
	UINT64 tmp64;
	UINT sec, millisec;
	time_t time;
	// Validate arguments
	if (st == NULL)
	{
		return;
	}

	sec64 = SafeTime64(sec64 + 32400000ULL);
	tmp64 = sec64 / (UINT64)1000;
	millisec = (UINT)(sec64 - tmp64 * (UINT64)1000);
	sec = (UINT)tmp64;
	time = (time_t)sec;
	TimeToSystem(st, time);
	st->wMilliseconds = (WORD)millisec;
}
// Convert to a time to be used safely in the current POSIX implementation
UINT64 SafeTime64(UINT64 sec64)
{
	return MAKESURE(sec64, 0, 2115947647000ULL);
}
// Convert the time_t to SYSTEMTIME
void TimeToSystem(SYSTEMTIME *st, time_t t)
{
    tm temptm = *localtime(&t);

    SYSTEMTIME st1 = {1900 + temptm.tm_year, 

           1 + temptm.tm_mon, 

           temptm.tm_wday, 

           temptm.tm_mday, 

           temptm.tm_hour, 

           temptm.tm_min, 

           temptm.tm_sec, 

           0};
	*st = st1;
}

// Comparison of DIRLIST list entry
int CompareDirListByName(void *p1, void *p2)
{
	DIRENT *d1, *d2;
	if (p1 == NULL || p2 == NULL)
	{
		return 0;
	}
	d1 = *(DIRENT **)p1;
	d2 = *(DIRENT **)p2;
	if (d1 == NULL || d2 == NULL)
	{
		return 0;
	}
	return UniStrCmpi(d1->FileNameW, d2->FileNameW);
}

bool GetEnvW(wchar_t *name, wchar_t *data, UINT size)
{
	return GetEnvW_ForWin32(name, data, size,true);

}


bool GetEnvW_ForWin32(wchar_t *name, wchar_t *data, UINT size,bool isUnicode)
{
	wchar_t *ret;
	// Validate arguments
	if (name == NULL || data == NULL)
	{
		return false;
	}

	if (isUnicode == false)
	{
		bool ret;
		char *name_a = CopyUniToStr(name);
		char data_a[MAX_SIZE];

		ret = GetEnv(name_a, data_a, sizeof(data_a));

		if (ret)
		{
			StrToUni(data, size, data_a);
		}

		Free(name_a);

		return ret;
	}

	UniStrCpy(data, size, L"");

	ret = _wgetenv(name);
	if (ret == NULL)
	{
		return false;
	}

	UniStrCpy(data, size, ret);

	return true;
}
// Get the environment variable string
bool GetEnv(char *name, char *data, UINT size)
{
	char *ret;
	// Validate arguments
	if (name == NULL || data == NULL)
	{
		return false;
	}

	StrCpy(data, size, "");

	ret = getenv(name);
	if (ret == NULL)
	{
		return false;
	}

	StrCpy(data, size, ret);

	return true;
}

// Increment of 32bit integer
void Win32Inc32(LONG *value)
{
	InterlockedIncrement(value);
}

// Decrement of 32bit integer
void Win32Dec32(LONG *value)
{
	InterlockedDecrement(value);
}

// Function of sort for PACK
int ComparePackName(void *p1, void *p2)
{
	ELEMENT *o1, *o2;
	if (p1 == NULL || p2 == NULL)
	{
		return 0;
	}
	o1 = *(ELEMENT **)p1;
	o2 = *(ELEMENT **)p2;
	if (o1 == NULL || o2 == NULL)
	{
		return 0;
	}

	return StrCmpi(o1->name, o2->name);
}

// Check the endian
bool CheckEndian()
{
	unsigned short test;
	UCHAR *buf;

	test = 0x1234;
	buf = (UCHAR *)&test;
	if (buf[0] == 0x12)
	{  //大端
		return false;
	}
	else
	{  //小端
		return true;
	}
}
bool IsBigEndian(){
	return CheckEndian()? false : true;
}
bool IsLittleEndian(){
	return CheckEndian();
}
// Comparison of the window list items
int CmpWindowList(void *p1, void *p2)
{
	HWND *h1, *h2;
	if (p1 == NULL || p2 == NULL)
	{
		return 0;
	}
	h1 = *(HWND **)p1;
	h2 = *(HWND **)p2;
	if (h1 == NULL || h2 == NULL)
	{
		return 0;
	}

	return Cmp(h1, h2, sizeof(HWND));
}

// Get the system timer
UINT Win32GetTick()
{
	return (UINT)timeGetTime();//This function retrieves the system time, in milliseconds. The system time is the time elapsed since the system started.
}
/*********************************** Other end ******************************/



/*********************************** List start ******************************/

LIST *NewListEx(COMPARE *cmp, bool fast)
{
	LIST *o;

	o = (LIST *)Malloc(sizeof(LIST));
	
	o->num_item = 0;
	o->num_reserved = INIT_NUM_RESERVED;
	o->Param1 = 0;

	o->p = (void **)Malloc(sizeof(void *) * o->num_reserved);

	o->cmp = cmp;
	o->sorted = true;

	return o;
}




// Creating a list
LIST *NewList(COMPARE *cmp)
{
	return NewListEx(cmp, false);
}

// Creating a list
LIST *NewListFast(COMPARE *cmp)
{
	return NewListEx(cmp, false);
}


// Release the list
void ReleaseList(LIST *o)
{
	// Validate arguments
	if (o == NULL)
	{
		return;
	}

	Free(o->p);

	Free(o);
}

// ReAlloc
void *ReAlloc(void *addr, UINT size){
	return realloc(addr,size);
}


// Add an element to the list
void Add(LIST *o, void *p)
{
	UINT i;
	// Validate arguments
	if (o == NULL || p == NULL)
	{
		return;
	}

	i = o->num_item;
	o->num_item++;

	if (o->num_item > o->num_reserved)
	{
		o->num_reserved = o->num_reserved * 2;
		o->p = (void**)ReAlloc(o->p, sizeof(void *) * o->num_reserved);
	}

	o->p[i] = p;
	o->sorted = false;

}

// Sort the list
void Sort(LIST *o)
{
	// Validate arguments
	if (o == NULL || o->cmp == NULL)
	{
		return;
	}

	qsort(o->p, o->num_item, sizeof(void *), (int(*)(const void *, const void *))o->cmp);
	o->sorted = true;
}
void SortEx(LIST *o, COMPARE *cmp)
{
	// Validate arguments
	if (o == NULL)
	{
		return;
	}

	qsort(o->p, o->num_item, sizeof(void *), (int(*)(const void *, const void *))cmp);
	o->sorted = false;

}

// Insert an item to the list
void Insert(LIST *o, void *p)
{
	int low, high, middle;
	UINT pos;
	int i;
	// Validate arguments
	if (o == NULL || p == NULL)
	{
		return;
	}

	if (o->cmp == NULL)
	{
		// adding simply if there is no sort function
		Add(o, p);
		return;
	}

	// Sort immediately if it is not sorted
	if (o->sorted == false)
	{
		Sort(o);
	}

	low = 0;
	high = LIST_NUM(o) - 1;

	pos = INFINITE;

	while (low <= high)
	{
		int ret;

		middle = (low + high) / 2;
		ret = o->cmp(&(o->p[middle]), &p);

		if (ret == 0)
		{
			pos = middle;
			break;
		}
		else if (ret > 0)
		{
			high = middle - 1;
		}
		else
		{
			low = middle + 1;
		}
	}

	if (pos == INFINITE)
	{
		pos = low;
	}

	o->num_item++;
	if (o->num_item > o->num_reserved)
	{
		o->num_reserved *= 2;
		o->p = (void**)ReAlloc(o->p, sizeof(void *) * o->num_reserved);
	}

	if (LIST_NUM(o) >= 2)
	{
		for (i = (LIST_NUM(o) - 2);i >= (int)pos;i--)
		{
			o->p[i + 1] = o->p[i];
		}
	}

	o->p[pos] = p;


}

// Search in the list
void * Search(LIST *o, void *target)
{
	void **ret;
	// Validate arguments
	if (o == NULL || target == NULL)
	{
		return NULL;
	}
	if (o->cmp == NULL)
	{
		return NULL;
	}

	// Check the sort
	if (o->sorted == false)
	{
		// Sort because it is not sorted
		Sort(o);
	}

	ret = (void **)bsearch(&target, o->p, o->num_item, sizeof(void *),
		(int(*)(const void *, const void *))o->cmp);


	if (ret != NULL)
	{
		return *ret;
	}
	else
	{
		return NULL;
	}
}

// Delete the element from the list
bool Delete(LIST *o, void *p)
{
	UINT i, n;
	// Validate arguments
	if (o == NULL || p == NULL)
	{
		return false;
	}

	for (i = 0;i < o->num_item;i++)
	{
		if (o->p[i] == p)
		{
			break;
		}
	}
	if (i == o->num_item)
	{
		return false;
	}

	n = i;
	for (i = n;i < (o->num_item - 1);i++)
	{
		o->p[i] = o->p[i + 1];
	}
	o->num_item--;
	if ((o->num_item * 2) <= o->num_reserved)
	{
		if (o->num_reserved > (INIT_NUM_RESERVED * 2))
		{
			o->num_reserved = o->num_reserved / 2;
			o->p =(void**) ReAlloc(o->p, sizeof(void *) * o->num_reserved);
		}
	}



	return true;
}

// Delete all elements from the list
void DeleteAll(LIST *o)
{
	// Validate arguments
	if (o == NULL)
	{
		return;
	}

	o->num_item = 0;
	o->num_reserved = INIT_NUM_RESERVED;
	o->p =(void**) ReAlloc(o->p, sizeof(void *) * INIT_NUM_RESERVED);
}



// Insert an item to the list (Do not insert if it already exists)
void InsertDistinct(LIST *o, void *p)
{
	// Validate arguments
	if (o == NULL || p == NULL)
	{
		return;
	}

	if (IsInList(o, p))
	{
		return;
	}

	Insert(o, p);
}


// Add an element to the list (Don't add if it already exists)
void AddDistinct(LIST *o, void *p)
{
	// Validate arguments
	if (o == NULL || p == NULL)
	{
		return;
	}

	if (IsInList(o, p))
	{
		return;
	}

	Add(o, p);
}


// Setting the sort flag
void SetSortFlag(LIST *o, bool sorted)
{
	// Validate arguments
	if (o == NULL)
	{
		return;
	}

	o->sorted = sorted;
}


// Check whether the specified number is already in the list
bool IsIntInList(LIST *o, UINT i)
{
	UINT j;
	// Validate arguments
	if (o == NULL)
	{
		return false;
	}

	for (j = 0;j < LIST_NUM(o);j++)
	{
		UINT *p = (UINT*)LIST_DATA(o, j);

		if (*p == i)
		{
			return true;
		}
	}

	return false;
}


/* int start */
// Add an integer to the list
void AddInt(LIST *o, UINT i)
{
	// Validate arguments
	if (o == NULL)
	{
		return;
	}

	Add(o, Clone(&i, sizeof(UINT)));
}

void InsertInt(LIST *o, UINT i)
{
	// Validate arguments
	if (o == NULL)
	{
		return;
	}

	Insert(o, Clone(&i, sizeof(UINT)));
}

// Add an integer to the list (no duplicates)
void AddIntDistinct(LIST *o, UINT i)
{
	// Validate arguments
	if (o == NULL)
	{
		return;
	}

	if (IsIntInList(o, i) == false)
	{
		AddInt(o, i);
	}
}
void InsertIntDistinct(LIST *o, UINT i)
{
	// Validate arguments
	if (o == NULL)
	{
		return;
	}

	if (IsIntInList(o, i) == false)
	{
		InsertInt(o, i);
	}
}

// Delete an integer from list
void DelInt(LIST *o, UINT i)
{
	LIST *o2 = NULL;
	UINT j;
	// Validate arguments
	if (o == NULL)
	{
		return;
	}

	for (j = 0;j < LIST_NUM(o);j++)
	{
		UINT *p = (UINT *)LIST_DATA(o, j);

		if (*p == i)
		{
			if (o2 == NULL)
			{
				o2 = NewListFast(NULL);
			}
			Add(o2, p);
		}
	}

	for (j = 0;j < LIST_NUM(o2);j++)
	{
		UINT *p = (UINT *)LIST_DATA(o2, j);

		Delete(o, p);

		Free(p);
	}

	if (o2 != NULL)
	{
		ReleaseList(o2);
	}
}

// Remove all int from the interger list
void DelAllInt(LIST *o)
{
	UINT i;
	// Validate arguments
	if (o == NULL)
	{
		return;
	}

	for (i = 0;i < LIST_NUM(o);i++)
	{
		UINT *p =(UINT *) LIST_DATA(o, i);

		Free(p);
	}

	DeleteAll(o);
}

// Create a new list of integers
LIST *NewIntList(bool sorted)
{
	LIST *o = NewList(sorted ? CompareInt : NULL);

	return o;
}


// Release the integer list
void ReleaseIntList(LIST *o)
{
	UINT i;
	// Validate arguments
	if (o == NULL)
	{
		return;
	}

	for (i = 0;i < LIST_NUM(o);i++)
	{
		UINT *p = (UINT *)LIST_DATA(o, i);

		Free(p);
	}

	ReleaseList(o);
}

// Examine whether the item exists in the list
bool IsInList(LIST *o, void *p)
{
	UINT i;
	// Validate arguments
	if (o == NULL || p == NULL)
	{
		return false;
	}

	for (i = 0;i < LIST_NUM(o);i++)
	{
		void *q = LIST_DATA(o, i);
		if (p == q)
		{
			return true;
		}
	}

	return false;
}

/* int end */





/* int64 end */
void InsertInt64(LIST *o, UINT64 i)
{
	// Validate arguments
	if (o == NULL)
	{
		return;
	}

	Insert(o, Clone(&i, sizeof(UINT64)));
}
void InsertInt64Distinct(LIST *o, UINT64 i)
{
	// Validate arguments
	if (o == NULL)
	{
		return;
	}

	if (IsInt64InList(o, i) == false)
	{
		InsertInt64(o, i);
	}
}
void AddInt64(LIST *o, UINT64 i)
{
	// Validate arguments
	if (o == NULL)
	{
		return;
	}

	Add(o, Clone(&i, sizeof(UINT64)));
}
void AddInt64Distinct(LIST *o, UINT64 i)
{
	// Validate arguments
	if (o == NULL)
	{
		return;
	}

	if (IsInt64InList(o, i) == false)
	{
		AddInt64(o, i);
	}
}
void DelInt64(LIST *o, UINT64 i)
{
	LIST *o2 = NULL;
	UINT j;
	// Validate arguments
	if (o == NULL)
	{
		return;
	}

	for (j = 0;j < LIST_NUM(o);j++)
	{
		UINT64 *p =(UINT64 *) LIST_DATA(o, j);

		if (*p == i)
		{
			if (o2 == NULL)
			{
				o2 = NewListFast(NULL);
			}
			Add(o2, p);
		}
	}

	for (j = 0;j < LIST_NUM(o2);j++)
	{
		UINT64 *p = (UINT64 *)LIST_DATA(o2, j);

		Delete(o, p);

		Free(p);
	}

	if (o2 != NULL)
	{
		ReleaseList(o2);
	}
}
LIST *NewInt64List(bool sorted)
{
	LIST *o = NewList(sorted ? CompareInt64 : NULL);

	return o;
}


void ReleaseInt64List(LIST *o)
{
	UINT i;
	// Validate arguments
	if (o == NULL)
	{
		return;
	}

	for (i = 0;i < LIST_NUM(o);i++)
	{
		UINT64 *p =(UINT64 *) LIST_DATA(o, i);

		Free(p);
	}

	ReleaseList(o);
}


bool IsInt64InList(LIST *o, UINT64 i)
{
	UINT j;
	// Validate arguments
	if (o == NULL)
	{
		return false;
	}

	for (j = 0;j < LIST_NUM(o);j++)
	{
		UINT64 *p = (UINT64*)LIST_DATA(o, j);

		if (*p == i)
		{
			return true;
		}
	}

	return false;
}

/* int64 end */


/* str start */
// Insert the string to the list
bool InsertStr(LIST *o, char *str)
{
	// Validate arguments
	if (o == NULL || str == NULL)
	{
		return false;
	}

	if (Search(o, str) == NULL)
	{
		Insert(o, str);

		return true;
	}

	return false;
}


// Free the string list
void FreeStrList(LIST *o)
{
	UINT i;
	// Validate arguments
	if (o == NULL)
	{
		return;
	}

	for (i = 0;i < LIST_NUM(o);i++)
	{
		char *s = (char*)LIST_DATA(o, i);
		Free(s);
	}

	ReleaseList(o);
}

// Get the memory size needed to store the string
UINT StrSize(char *str)
{
	// Validate arguments
	if (str == NULL)
	{
		return 0;
	}

	return StrLen(str) + 1;
}


// Format the string, and return the result
char *CopyFormat(char *fmt, ...)
{
	char *buf;
	UINT size;
	va_list args;
	// Validate arguments
	if (fmt == NULL)
	{
		return NULL;
	}

	size = MAX(StrSize(fmt) * 10, MAX_SIZE * 10);
	buf = (char *)Malloc(size);

	va_start(args, fmt);
	vsprintf(buf,fmt,args);
	va_end(args);

	return buf;
}



// Copy an ANSI string to a Unicode string
wchar_t *CopyStrToUni(char *str)
{
	wchar_t *uni;
	UINT uni_size;
	// Validate arguments
	if (str == NULL)
	{
		return NULL;
	}

	uni_size = CalcStrToUni(str);
	if (uni_size == 0)
	{
		return CopyUniStr(L"");
	}
	uni = (wchar_t *)Malloc(uni_size);
	StrToUni(uni, uni_size, str);

	return uni;
}

// Get the required buffer size for converting an ANSI string to an Unicode string
UINT CalcStrToUni(char *str)
{
	UINT ret;
	// Validate arguments
	if (str == NULL)
	{
		return 0;
	}

	ret = (UINT)mbstowcs(NULL, str, StrLen(str));
	if (ret == (UINT)-1)
	{
		return 0;
	}

	return (ret + 1) * sizeof(wchar_t);

}



// String copy
UINT UniStrCpy(wchar_t *dst, UINT size, wchar_t *src)
{
	UINT len;
	// Validate arguments
	if (dst == NULL || src == NULL)
	{
		if (src == NULL && dst != NULL)
		{
			if (size >= sizeof(wchar_t))
			{
				dst[0] = L'\0';
			}
		}
		return 0;
	}
	if (dst == src)
	{
		return UniStrLen(src);
	}
	if (size != 0 && size < sizeof(wchar_t))
	{
		return 0;
	}
	if (size == sizeof(wchar_t))
	{
		wcscpy(dst, L"");
		return 0;
	}
	if (size == 0)
	{
		// Ignore the length
		size = 0x3fffffff;
	}

	// Check the length
	len = UniStrLen(src);
	if (len <= (size / sizeof(wchar_t) - 1))
	{
		Copy(dst, src, (len + 1) * sizeof(wchar_t));
	}
	else
	{
		len = size / sizeof(wchar_t) - 1;
		Copy(dst, src, len * sizeof(wchar_t));
		dst[len] = 0;
	}

	return len;
}


// Converted an ANSI string to a Unicode string
UINT StrToUni(wchar_t *s, UINT size, char *str)
{
	UINT ret;
	wchar_t *tmp;
	UINT new_size;
	// Validate arguments
	if (s == NULL || str == NULL)
	{
		return 0;
	}

	new_size = CalcStrToUni(str);
	if (new_size == 0)
	{
		if (size >= 2)
		{
			UniStrCpy(s, 0, L"");
		}
		return 0;
	}
	tmp =(wchar_t *) Malloc(new_size);
	tmp[0] = 0;
	mbstowcs(tmp, str, StrLen(str));
	tmp[(new_size - 1) / sizeof(wchar_t)] = 0;
	ret = UniStrCpy(s, size, tmp);
	Free(tmp);

	return ret;

}
/* str end */



// array start
// Copy the list to an array
void CopyToArray(LIST *o, void *p)
{
	// Validate arguments
	if (o == NULL || p == NULL)
	{
		return;
	}


	Copy(p, o->p, sizeof(void *) * o->num_item);
}

void *ToArrayEx(LIST *o, bool fast)
{
	void *p;
	// Validate arguments
	if (o == NULL)
	{
		return NULL;
	}

	// Memory allocation
	if (fast == false)
	{
		p = Malloc(sizeof(void *) * LIST_NUM(o));
	}
	else
	{
		p = MallocFast(sizeof(void *) * LIST_NUM(o));
	}
	// Copy
	CopyToArray(o, p);

	return p;
}
// Arrange the list to an array
void *ToArray(LIST *o)
{
	return ToArrayEx(o, false);
}
// array end

// Creating a new window list
LIST *NewWindowList()
{
	return NewListFast(CmpWindowList);
}
// Release of the window list
void FreeWindowList(LIST *o)
{
	UINT i;
	// Validate arguments
	if (o == NULL)
	{
		return;
	}

	for (i = 0;i < LIST_NUM(o);i++)
	{
		HWND *e = (HWND *)LIST_DATA(o, i);

		Free(e);
	}

	ReleaseList(o);
}
// Add a window to the window list
void AddWindow(LIST *o, HWND hWnd)
{
	HWND t, *e;
	// Validate arguments
	if (o == NULL || hWnd == NULL)
	{
		return;
	}

	t = hWnd;

	if (Search(o, &t) != NULL)
	{
		return;
	}

	e = (HWND*)ZeroMalloc(sizeof(HWND));
	*e = hWnd;

	Insert(o, e);
}

/*********************************** List end ******************************/

/*********************************** Base64 start ***********************************/ 
// Base64 encode
UINT Encode64(char *dst, char *src)
{
	// Validate arguments
	if (dst == NULL || src == NULL)
	{
		return 0;
	}

	return B64_Encode(dst, src, StrLen(src));
}

// Base64 decoding
UINT Decode64(char *dst, char *src)
{
	// Validate arguments
	if (dst == NULL || src == NULL)
	{
		return 0;
	}

	return B64_Decode(dst, src, StrLen(src));
}

// Base64 encode
int B64_Encode(char *set, char *source, int len)
{
	BYTE *src;
	int i,j;
	src = (BYTE *)source;
	j = 0;
	i = 0;
	if (!len)
	{
		return 0;
	}
	while (TRUE)
	{
		if (i >= len)
		{
			return j;
		}
		if (set)
		{
			set[j] = B64_CodeToChar((src[i]) >> 2);
		}
		if (i + 1 >= len)
		{
			if (set)
			{
				set[j + 1] = B64_CodeToChar((src[i] & 0x03) << 4);
				set[j + 2] = '=';
				set[j + 3] = '=';
			}
			return j + 4;
		}
		if (set)
		{
			set[j + 1] = B64_CodeToChar(((src[i] & 0x03) << 4) + ((src[i + 1] >> 4)));
		}
		if (i + 2 >= len)
		{
			if (set)
			{
				set[j + 2] = B64_CodeToChar((src[i + 1] & 0x0f) << 2);
				set[j + 3] = '=';
			}
			return j + 4;
		}
		if (set)
		{
			set[j + 2] = B64_CodeToChar(((src[i + 1] & 0x0f) << 2) + ((src[i + 2] >> 6)));
			set[j + 3] = B64_CodeToChar(src[i + 2] & 0x3f);
		}
		i += 3;
		j += 4;
	}
}

// Base64 decode
int B64_Decode(char *set, char *source, int len)
{
	int i,j;
	char a1,a2,a3,a4;
	char *src;
	int f1,f2,f3,f4;
	src = source;
	i = 0;
	j = 0;
	while (TRUE)
	{
		f1 = f2 = f3 = f4 = 0;
		if (i >= len)
		{
			break;
		}
		f1 = 1;
		a1 = B64_CharToCode(src[i]);
		if (a1 == -1)
		{
			f1 = 0;
		}
		if (i >= len + 1)
		{
			a2 = 0;
		}
		else
		{
			a2 = B64_CharToCode(src[i + 1]);
			f2 = 1;
			if (a2 == -1)
			{
				f2 = 0;
			}
		}
		if (i >= len + 2)
		{
			a3 = 0;
		}
		else
		{
			a3 = B64_CharToCode(src[i + 2]);
			f3 = 1;
			if (a3 == -1)
			{
				f3 = 0;
			}
		}
		if (i >= len + 3)
		{
			a4 = 0;
		}
		else
		{
			a4 = B64_CharToCode(src[i + 3]);
			f4 = 1;
			if (a4 == -1)
			{
				f4 = 0;
			}
		}
		if (f1 && f2)
		{
			if (set)
			{
				set[j] = (a1 << 2) + (a2 >> 4);
			}
			j++;
		}
		if (f2 && f3)
		{
			if (set)
			{
				set[j] = (a2 << 4) + (a3 >> 2);
			}
			j++;
		}
		if (f3 && f4)
		{
			if (set)
			{
				set[j] = (a3 << 6) + a4;
			}
			j++;
		}
		i += 4;
	}
	return j;
}

// Base64 : Convert a code to a character
char B64_CodeToChar(BYTE c)
{
	BYTE r;
	r = '=';
	if (c <= 0x19)
	{
		r = c + 'A';
	}
	if (c >= 0x1a && c <= 0x33)
	{
		r = c - 0x1a + 'a';
	}
	if (c >= 0x34 && c <= 0x3d)
	{
		r = c - 0x34 + '0';
	}
	if (c == 0x3e)
	{
		r = '+';
	}
	if (c == 0x3f)
	{
		r = '/';
	}
	return r;
}

// Base64 : Convert a character to a code
char B64_CharToCode(char c)
{
	if (c >= 'A' && c <= 'Z')
	{
		return c - 'A';
	}
	if (c >= 'a' && c <= 'z')
	{
		return c - 'a' + 0x1a;
	}
	if (c >= '0' && c <= '9')
	{
		return c - '0' + 0x34;
	}
	if (c == '+')
	{
		return 0x3e;
	}
	if (c == '/')
	{
		return 0x3f;
	}
	if (c == '=')
	{
		return -1;
	}
	return 0;
}

/*********************************** Base64 end ***********************************/ 



/*********************************** Buf start ******************************/

// Creating a buffer
BUF *NewBuf()
{
	BUF *b;

	// Memory allocation
	b = (BUF *)Malloc(sizeof(BUF));
	b->Buf = Malloc(INIT_BUF_SIZE);
	b->Size = 0;
	b->Current = 0;
	b->SizeReserved = INIT_BUF_SIZE;
	return b;
}

// Free the buffer
void FreeBuf(BUF *b)
{
	// Validate arguments
	if (b == NULL)
	{
		return;
	}

	// Memory release
	Free(b->Buf);
	Free(b);


}

// Clearing the buffer
void ClearBuf(BUF *b)
{
	// Validate arguments
	if (b == NULL)
	{
		return;
	}

	b->Size = 0;
	b->Current = 0;
}


// Compare BUFs whether two are identical
bool CompareBuf(BUF *b1, BUF *b2)
{
	// Validate arguments
	if (b1 == NULL && b2 == NULL)
	{
		return true;
	}
	if (b1 == NULL || b2 == NULL)
	{
		return false;
	}

	if (b1->Size != b2->Size)
	{
		return false;
	}

	if (Cmp(b1->Buf, b2->Buf, b1->Size) != 0)
	{
		return false;
	}

	return true;
}

// Write to the buffer
void WriteBuf(BUF *b, void *buf, UINT size)
{
	UINT new_size;
	// Validate arguments
	if (b == NULL || buf == NULL || size == 0)
	{
		return;
	}

	new_size = b->Current + size;
	if (new_size > b->Size)
	{
		// Adjust the size
		AdjustBufSize(b, new_size);
	}
	if (b->Buf != NULL)
	{
		Copy((UCHAR *)b->Buf + b->Current, buf, size);
	}
	b->Current += size;
	b->Size = new_size;


}

// Read from the buffer
UINT ReadBuf(BUF *b, void *buf, UINT size)
{
	UINT size_read;
	// Validate arguments
	if (b == NULL || size == 0)
	{
		return 0;
	}

	if (b->Buf == NULL)
	{
		Zero(buf, size);
		return 0;
	}
	size_read = size;
	if ((b->Current + size) >= b->Size)
	{
		size_read = b->Size - b->Current;
		if (buf != NULL)
		{
			Zero((UCHAR *)buf + size_read, size - size_read);
		}
	}

	if (buf != NULL)
	{
		Copy(buf, (UCHAR *)b->Buf + b->Current, size_read);
	}

	b->Current += size_read;

	return size_read;
}

// Adjusting the buffer size
void AdjustBufSize(BUF *b, UINT new_size)
{
	// Validate arguments
	if (b == NULL)
	{
		return;
	}

	if (b->SizeReserved >= new_size)
	{
		return;
	}

	while (b->SizeReserved < new_size)
	{
		b->SizeReserved = b->SizeReserved * 2;
	}
	b->Buf = ReAlloc(b->Buf, b->SizeReserved);

}

// Seek of the buffer
void SeekBuf(BUF *b, UINT offset, int mode)
{
	UINT new_pos;
	// Validate arguments
	if (b == NULL)
	{
		return;
	}

	if (mode == 0)
	{
		// Absolute position
		new_pos = offset;
	}
	else
	{
		if (mode > 0)
		{
			// Move Right
			new_pos = b->Current + offset;
		}
		else
		{
			// Move Left
			if (b->Current >= offset)
			{
				new_pos = b->Current - offset;
			}
			else
			{
				new_pos = 0;
			}
		}
	}
	b->Current = MAKESURE(new_pos, 0, b->Size);

}

// Seek to the beginning of the buffer
void SeekBufToBegin(BUF *b)
{
	// Validate arguments
	if (b == NULL)
	{
		return;
	}

	SeekBuf(b, 0, 0);
}

// Seek to end of the buffer
void SeekBufToEnd(BUF *b)
{
	// Validate arguments
	if (b == NULL)
	{
		return;
	}

	SeekBuf(b, b->Size, 0);
}

// Append a string to the buffer
void AddBufStr(BUF *b, char *str)
{
	// Validate arguments
	if (b == NULL || str == NULL)
	{
		return;
	}

	WriteBuf(b, str, StrLen(str));
}
// Write a line to the buffer
void WriteBufLine(BUF *b, char *str)
{
	char *crlf = "\r\n";
	// Validate arguments
	if (b == NULL || str == NULL)
	{
		return;
	}

	WriteBuf(b, str, StrLen(str));
	WriteBuf(b, crlf, StrLen(crlf));
}

// Write a string to a buffer
bool WriteBufStr(BUF *b, char *str)
{
	UINT len;
	// Validate arguments
	if (b == NULL || str == NULL)
	{
		return false;
	}

	// String length
	len = StrLen(str);
	if (WriteBufInt(b, len + 1) == false)
	{
		return false;
	}

	// String body
	WriteBuf(b, str, len);

	return true;
}

// Read a string from the buffer
bool ReadBufStr(BUF *b, char *str, UINT size)
{
	UINT len;
	UINT read_size;
	// Validate arguments
	if (b == NULL || str == NULL || size == 0)
	{
		return false;
	}

	// Read the length of the string
	len = ReadBufInt(b);
	if (len == 0)
	{
		return false;
	}
	len--;
	if (len <= (size - 1))
	{
		size = len + 1;
	}

	read_size = MIN(len, (size - 1));

	// Read the string body
	if (ReadBuf(b, str, read_size) != read_size)
	{
		return false;
	}
	if (read_size < len)
	{
		ReadBuf(b, NULL, len - read_size);
	}
	str[read_size] = 0;

	return true;
}

// Write an integer in the the buffer
bool WriteBufInt(BUF *b, UINT value)
{
	// Validate arguments
	if (b == NULL)
	{
		return false;
	}

	value = Endian32(value);

	WriteBuf(b, &value, sizeof(UINT));
	return true;
}
// Read an integer from the buffer
UINT ReadBufInt(BUF *b)
{
	UINT value;
	// Validate arguments
	if (b == NULL)
	{
		return 0;
	}

	if (ReadBuf(b, &value, sizeof(UINT)) != sizeof(UINT))
	{
		return 0;
	}
	return Endian32(value);
}

// Read a UCHAR from the buffer
UCHAR ReadBufChar(BUF *b)
{
	UCHAR uc;
	// Validate arguments
	if (b == NULL)
	{
		return 0;
	}

	if (ReadBuf(b, &uc, 1) != 1)
	{
		return 0;
	}

	return uc;
}

// Write a UCHAR to the buffer
bool WriteBufChar(BUF *b, UCHAR uc)
{
	// Validate arguments
	if (b == NULL)
	{
		return false;
	}

	WriteBuf(b, &uc, 1);

	return true;
}
// Read a 64bit integer from the buffer
UINT64 ReadBufInt64(BUF *b)
{
	UINT64 value;
	// Validate arguments
	if (b == NULL)
	{
		return 0;
	}

	if (ReadBuf(b, &value, sizeof(UINT64)) != sizeof(UINT64))
	{
		return 0;
	}
	return Endian64(value);
}
// Write a 64 bit integer to the buffer
bool WriteBufInt64(BUF *b, UINT64 value)
{
	// Validate arguments
	if (b == NULL)
	{
		return false;
	}

	value = Endian64(value);

	WriteBuf(b, &value, sizeof(UINT64));
	return true;
}

/*********************************** Buf end ******************************/


/*********************************** File start ******************************/
#if 0
// Create a file
HANDLE FileCreate(char *name)
{
	HANDLE h;
	// Validate arguments
	if (name == NULL)
	{
		return NULL;
	}

	h = CreateFile(name, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (h == NULL || h == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}

	return h;
}
// Open the file
HANDLE FileOpen(char *name, bool write_mode)
{
	HANDLE h;
	DWORD lock_mode;
	// Validate arguments
	if (name == NULL)
	{
		return NULL;
	}

	if (write_mode)
	{
		lock_mode = FILE_SHARE_READ;
	}
	else
	{
		lock_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	}

	h = CreateFile(name,
		(write_mode ? GENERIC_READ | GENERIC_WRITE : GENERIC_READ),
		lock_mode,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (h == INVALID_HANDLE_VALUE)
	{
		return NULL;
	}

	return h;
}
// Close the file
void FileClose(HANDLE h)
{
	// Validate arguments
	if (h == NULL)
	{
		return;
	}

	FlushFileBuffers(h);

	CloseHandle(h);
}
// Read from the file
bool FileRead(HANDLE h, void *buf, UINT size)
{
	UINT read_size;
	// Validate arguments
	if (h == NULL || buf == NULL || size == 0)
	{
		return false;
	}

	if (ReadFile(h, buf, size, (LPDWORD)&read_size, NULL) == false)
	{
		return false;
	}

	if (read_size != size)
	{
		return false;
	}

	return true;
}
// Write to the file
bool FileWrite(HANDLE h, void *buf, UINT size)
{
	DWORD write_size;
	// Validate arguments
	if (h == NULL || buf == NULL || size == 0)
	{
		return false;
	}

	if (WriteFile(h, buf, size, &write_size, NULL) == false)
	{
		return false;
	}

	if (write_size != size)
	{
		return false;
	}

	return true;
}
// Get the file size
UINT64 FileSize(HANDLE h)
{
	UINT64 ret;
	DWORD tmp;
	// Validate arguments
	if (h == NULL)
	{
		return 0;
	}

	tmp = 0;
	ret = GetFileSize(h, &tmp);
	if (ret == (DWORD)-1)
	{
		return 0;
	}

	if (tmp != 0)
	{
		ret += (UINT64)tmp * 4294967296ULL;
	}

	return ret;
}
#else
//unicode
IO *FileCreate(char *name)
{
	wchar_t *name_w = CopyStrToUni(name);
	IO *ret = FileCreateW(name_w);

	Free(name_w);

	return ret;
}
IO *FileCreateW(wchar_t *name)
{
	// Validate arguments
	if (name == NULL)
	{
		return NULL;
	}

	return FileCreateInnerW(name);
}
IO *FileCreateInnerW(wchar_t *name)
{
	IO *o;
	void *p;
	wchar_t name2[MAX_SIZE];
	// Validate arguments
	if (name == NULL)
	{
		return NULL;
	}

	UniStrCpy(name2, sizeof(name2), name);
	ConvertPathW(name2);

	p = OSFileCreateW(name2);
	if (p == NULL)
	{
		return NULL;
	}

	o =(IO*) ZeroMalloc(sizeof(IO));
	o->pData = p;
	UniStrCpy(o->NameW, sizeof(o->NameW), name2);
	UniToStr(o->Name, sizeof(o->Name), o->NameW);
	o->WriteMode = true;

	return o;
}

void *OSFileCreateW(wchar_t *name)
{
	return Win32FileCreateW(name);
}

// Write to a file
bool FileWrite(IO *o, void *buf, UINT size)
{
	// Validate arguments
	if (o == NULL || buf == NULL)
	{
		return false;
	}
	if (o->WriteMode == false)
	{
		return false;
	}

	if (size == 0)
	{
		return true;
	}

	return OSFileWrite(o->pData, buf, size);
}

// Write to a file
bool OSFileWrite(void *pData, void *buf, UINT size)
{
	return Win32FileWrite(pData, buf, size);
}

#endif  //_UNICODE

// Create a file
void *Win32FileCreateW(wchar_t *name)
{
	WIN32IO *p;
	HANDLE h;
	// Validate arguments
	if (name == NULL)
	{
		return NULL;
	}

	// Create a file
	h = CreateFileW(name, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (h == INVALID_HANDLE_VALUE)
	{
		h = CreateFileW(name, GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN,
			NULL);
		if (h == INVALID_HANDLE_VALUE)
		{
			return NULL;
		}
	}

	// Memory allocation
	p = (WIN32IO*)Win32MemoryAlloc(sizeof(WIN32IO));
	// Store Handle
	p->hFile = h;

	p->WriteMode = true;

	return (void *)p;
}
void *Win32FileCreate(char *name)
{
	WIN32IO *p;
	HANDLE h;
	// Validate arguments
	if (name == NULL)
	{
		return NULL;
	}

	// Create a file
	h = CreateFileA(name, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (h == INVALID_HANDLE_VALUE)
	{
		h = CreateFileA(name, GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_HIDDEN,
			NULL);
		if (h == INVALID_HANDLE_VALUE)
		{
			return NULL;
		}
	}

	// Memory allocation
	p = (WIN32IO*)Win32MemoryAlloc(sizeof(WIN32IO));
	// Store Handle
	p->hFile = h;

	p->WriteMode = true;

	return (void *)p;
}
// Memory allocation
void *Win32MemoryAlloc(UINT size)
{
	if (use_heap_api)
	{
		return HeapAlloc(heap_handle, 0, size);
	}
	else
	{
		return malloc(size);
	}
}

// Memory reallocation
void *Win32MemoryReAlloc(void *addr, UINT size)
{
	if (use_heap_api)
	{
		return HeapReAlloc(heap_handle, 0, addr, size);
	}
	else
	{
		return realloc(addr, size);
	}
}
// Memory allocation
void Win32MemoryFree(void *addr)
{
	if (use_heap_api)
	{
		HeapFree(heap_handle, 0, addr);
	}
	else
	{
		free(addr);
	}
}
// Write to the file
bool Win32FileWrite(void *pData, void *buf, UINT size)
{
	WIN32IO *p;
	DWORD write_size;
	// Validate arguments
	if (pData == NULL || buf == NULL || size == 0)
	{
		return false;
	}

	p = (WIN32IO *)pData;
	if (WriteFile(p->hFile, buf, size, &write_size, NULL) == FALSE)
	{
		return false;
	}

	if (write_size != size)
	{
		return false;
	}

	return true;
}
// Read from a file
bool Win32FileRead(void *pData, void *buf, UINT size)
{
	WIN32IO *p;
	DWORD read_size;
	// Validate arguments
	if (pData == NULL || buf == NULL || size == 0)
	{
		return false;
	}

	p = (WIN32IO *)pData;
	if (ReadFile(p->hFile, buf, size, &read_size, NULL) == FALSE)
	{
		return false;
	}

	if (read_size != size)
	{
		return false;
	}
	
	return true;;
}
// Close the file
void Win32FileClose(void *pData, bool no_flush)
{
	WIN32IO *p;
	// Validate arguments
	if (pData == NULL)
	{
		return;
	}

	p = (WIN32IO *)pData;
	if (p->WriteMode && no_flush == false)
	{
		FlushFileBuffers(p->hFile);
	}
	CloseHandle(p->hFile);
	p->hFile = NULL;

	// Memory release
	Win32MemoryFree(p);
}
// Get the date of the file
bool Win32FileGetDate(void *pData, UINT64 *created_time, UINT64 *updated_time, UINT64 *accessed_date)
{
	WIN32IO *p;
	BY_HANDLE_FILE_INFORMATION info;
	SYSTEMTIME st_create, st_update, st_access;
	// Validate arguments
	if (pData == NULL)
	{
		return false;
	}

	p = (WIN32IO *)pData;

	Zero(&info, sizeof(info));

	if (GetFileInformationByHandle(p->hFile, &info) == false)
	{
		return false;
	}

	Zero(&st_create, sizeof(st_create));
	Zero(&st_update, sizeof(st_update));
	Zero(&st_access, sizeof(st_access));

	FileTimeToSystemTime(&info.ftCreationTime, &st_create);
	FileTimeToSystemTime(&info.ftLastWriteTime, &st_update);
	FileTimeToSystemTime(&info.ftLastAccessTime, &st_access);

	if (created_time != NULL)
	{
		*created_time = SystemToUINT64(&st_create);
	}

	if (updated_time != NULL)
	{
		*updated_time = SystemToUINT64(&st_update);
	}

	if (accessed_date != NULL)
	{
		*accessed_date = SystemToUINT64(&st_access);
	}

	return true;
}
// Set the date of the file
bool Win32FileSetDate(void *pData, UINT64 created_time, UINT64 updated_time)
{
	WIN32IO *p;
	SYSTEMTIME st_created_time, st_updated_time;
	FILETIME ft_created_time, ft_updated_time;
	FILETIME *p_created_time = NULL, *p_updated_time = NULL;
	// Validate arguments
	if (pData == NULL || (created_time == 0 && updated_time == 0))
	{
		return false;
	}

	p = (WIN32IO *)pData;

	Zero(&st_created_time, sizeof(st_created_time));
	Zero(&st_updated_time, sizeof(st_updated_time));

	if (created_time != 0)
	{
		UINT64ToSystem(&st_created_time, created_time);

		SystemTimeToFileTime(&st_created_time, &ft_created_time);

		p_created_time = &ft_created_time;
	}

	if (updated_time != 0)
	{
		UINT64ToSystem(&st_updated_time, updated_time);

		SystemTimeToFileTime(&st_updated_time, &ft_updated_time);

		p_updated_time = &ft_updated_time;
	}
	//SetFileTime Nonzero indicates success.  Zero indicates failure. 
	return SetFileTime(p->hFile, p_created_time, NULL, p_updated_time) == 0 ? false:true;
}

// Flush to the file
void Win32FileFlush(void *pData)
{
	WIN32IO *p;
	// Validate arguments
	if (pData == NULL)
	{
		return;
	}

	p = (WIN32IO *)pData;
	if (p->WriteMode)
	{
		FlushFileBuffers(p->hFile);
	}
}
// Open the file
void *Win32FileOpenW(wchar_t *name, bool write_mode, bool read_lock)
{
	WIN32IO *p;
	HANDLE h;
	DWORD lock_mode;
	// Validate arguments
	if (name == NULL)
	{
		return NULL;
	}

	if (write_mode)
	{
		lock_mode = FILE_SHARE_READ;
	}
	else
	{
		if (read_lock == false)
		{
			lock_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;
		}
		else
		{
			lock_mode = FILE_SHARE_READ;
		}
	}

	// Open the file
	h = CreateFileW(name,
		(write_mode ? GENERIC_READ | GENERIC_WRITE : GENERIC_READ),
		lock_mode,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h == INVALID_HANDLE_VALUE)
	{
		UINT ret = GetLastError();
		// Failure
		return NULL;
	}

	// Memory allocation
	p = (WIN32IO*)Win32MemoryAlloc(sizeof(WIN32IO));
	// Store Handle
	p->hFile = h;

	p->WriteMode = write_mode;

	return (void *)p;
}
void *Win32FileOpen(char *name, bool write_mode, bool read_lock)
{
	WIN32IO *p;
	HANDLE h;
	DWORD lock_mode;
	// Validate arguments
	if (name == NULL)
	{
		return NULL;
	}

	if (write_mode)
	{
		lock_mode = FILE_SHARE_READ;
	}
	else
	{
		if (read_lock == false)
		{
			lock_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;
		}
		else
		{
			lock_mode = FILE_SHARE_READ;
		}
	}

	// Open the file
	h = CreateFileA(name,
		(write_mode ? GENERIC_READ | GENERIC_WRITE : GENERIC_READ),
		lock_mode,
		NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (h == INVALID_HANDLE_VALUE)
	{
		UINT ret = GetLastError();
		// Failure
		return NULL;
	}

	// Memory allocation
	p = (WIN32IO*)Win32MemoryAlloc(sizeof(WIN32IO));
	// Store Handle
	p->hFile = h;

	p->WriteMode = write_mode;

	return (void *)p;
}
// Delete the file
bool Win32FileDeleteW(wchar_t *name)
{
	// Validate arguments
	if (name == NULL)
	{
		return false;
	}

	if (DeleteFileW(name) == FALSE)
	{
		return false;
	}
	return true;
}
bool Win32FileDelete(char *name)
{
	// Validate arguments
	if (name == NULL)
	{
		return false;
	}

	if (DeleteFileA(name) == FALSE)
	{
		return false;
	}
	return true;
}
bool Win32FileSeek(void *pData, UINT mode, int offset)
{
	WIN32IO *p;
	DWORD ret;
	// Validate arguments
	if (pData == NULL)
	{
		return false;
	}
	if (mode != FILE_BEGIN && mode != FILE_END && mode != FILE_CURRENT)
	{
		return false;
	}

	p = (WIN32IO *)pData;
	ret = SetFilePointer(p->hFile, (LONG)offset, NULL, mode);
	if (ret == INVALID_SET_FILE_POINTER || ret == ERROR_NEGATIVE_SEEK)
	{
		return false;
	}
	return true;
}

// Delete the directory
bool Win32DeleteDirW(wchar_t *name)
{
	// Validate arguments
	if (name == NULL)
	{
		return false;
	}

	if (RemoveDirectoryW(name) == FALSE)
	{
		return false;
	}
	return true;
}
bool Win32DeleteDir(char *name)
{
	// Validate arguments
	if (name == NULL)
	{
		return false;
	}

	if (RemoveDirectoryA(name) == FALSE)
	{
		return false;
	}
	return true;
}
// Create a directory
bool Win32MakeDirW(wchar_t *name)
{
	// Validate arguments
	if (name == NULL)
	{
		return false;
	}


	if (CreateDirectoryW(name, NULL) == FALSE)
	{
		return false;
	}

	return true;
}
bool Win32MakeDir(char *name)
{
	// Validate arguments
	if (name == NULL)
	{
		return false;
	}

	if (CreateDirectoryA(name, NULL) == FALSE)
	{
		return false;
	}

	return true;
}
// Get the file size
UINT64 Win32FileSize(void *pData)
{
	WIN32IO *p;
	UINT64 ret;
	DWORD tmp;
	// Validate arguments
	if (pData == NULL)
	{
		return 0;
	}

	p = (WIN32IO *)pData;
	tmp = 0;
	ret = GetFileSize(p->hFile, &tmp);
	if (ret == (DWORD)-1)
	{
		return 0;
	}

	if (tmp != 0)
	{
		ret += (UINT64)tmp * 4294967296ULL;
	}

	return ret;
}

void Win32GetCurrentDir(char *dir, UINT size)
{
	// Validate arguments
	if (dir == NULL)
	{
		return;
	}

	GetCurrentDirectoryA(size, dir);
}

// Get the current directory
void Win32GetCurrentDirW(wchar_t *dir, UINT size)
{
	// Validate arguments
	if (dir == NULL)
	{
		return;
	}

	GetCurrentDirectoryW(size, dir);
}


// Enumeration of directory
DIRLIST *Win32EnumDirEx(char *dirname, COMPARE *compare)
{
	WIN32_FIND_DATAA data_a;
	WIN32_FIND_DATAW data_w;
	HANDLE h;
	wchar_t tmp[MAX_PATH];
	wchar_t tmp2[MAX_PATH];
	//wchar_t dirname2[MAX_PATH];
	LIST *o;
	DIRLIST *d;
	wchar_t *dirname_w = CopyStrToUni(dirname);

	UniStrCpy(tmp2, sizeof(tmp2), dirname_w);

	if (UniStrLen(tmp2) >= 1 && tmp[UniStrLen(tmp2) - 1] == L'\\')
	{
		tmp2[UniStrLen(tmp2) - 1] = 0;
	}

	UniFormat(tmp, sizeof(tmp), L"%s\\*.*", tmp2);
	//NormalizePathW(tmp, sizeof(tmp), tmp);
	//NormalizePathW(dirname2, sizeof(dirname2), tmp2);
	//Copy(dirname2,tmp2,UniStrLen(tmp2));

	o = NewListFast(compare);

	Zero(&data_a, sizeof(data_a));
	Zero(&data_w, sizeof(data_w));

	char *tmp_a = CopyUniToStr(tmp);

	h = FindFirstFileA(tmp_a, &data_a);

	Free(tmp_a);


	if (h != INVALID_HANDLE_VALUE)
	{
		bool b = true;

		do
		{
			Zero(&data_w, sizeof(data_w));
			StrToUni(data_w.cFileName, sizeof(data_w.cFileName), data_a.cFileName);
			data_w.dwFileAttributes = data_a.dwFileAttributes;
			data_w.ftCreationTime = data_a.ftCreationTime;
			data_w.ftLastWriteTime = data_a.ftLastWriteTime;
			data_w.nFileSizeHigh = data_a.nFileSizeHigh;
			data_w.nFileSizeLow = data_a.nFileSizeLow;


			if (UniStrCmpi(data_w.cFileName, L"..") != 0 &&
				UniStrCmpi(data_w.cFileName, L".") != 0)
			{
				DIRENT *f =(DIRENT*) ZeroMalloc(sizeof(DIRENT));

				f->FileNameW = UniCopyStr(data_w.cFileName);
				f->FileName = CopyUniToStr(f->FileNameW);
				f->Folder = (data_w.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? true : false;

				Add(o, f);
			}

			Zero(&data_w, sizeof(data_w));
			Zero(&data_a, sizeof(data_a));
			b = FindNextFileA(h, &data_a) == 0? false:true;


		}
		while (b);

		FindClose(h);
	}

	Sort(o);

	d = (DIRLIST*)ZeroMalloc(sizeof(DIRLIST));
	d->NumFiles = LIST_NUM(o);
	d->File = (DIRENT**)ToArray(o);

	ReleaseList(o);

	Free(dirname_w);

	return d;
}


DIRLIST *Win32EnumDirExW(wchar_t *dirname, COMPARE *compare)
{
	WIN32_FIND_DATAW data_w;
	HANDLE h;
	wchar_t tmp[MAX_PATH];
	wchar_t tmp2[MAX_PATH];
	//wchar_t dirname2[MAX_PATH];
	LIST *o;
	DIRLIST *d;

	UniStrCpy(tmp2, sizeof(tmp2), dirname);

	if (UniStrLen(tmp2) >= 1 && tmp[UniStrLen(tmp2) - 1] == L'\\')
	{
		tmp2[UniStrLen(tmp2) - 1] = 0;
	}

	UniFormat(tmp, sizeof(tmp), L"%s\\*.*", tmp2);
	//NormalizePathW(tmp, sizeof(tmp), tmp);
	//NormalizePathW(dirname2, sizeof(dirname2), tmp2);
	//Copy(dirname2,tmp2,UniStrLen(tmp2));

	o = NewListFast(compare);

	Zero(&data_w, sizeof(data_w));

	h = FindFirstFileW(tmp, &data_w);


	if (h != INVALID_HANDLE_VALUE)
	{
		bool b = true;

		do
		{
			if (UniStrCmpi(data_w.cFileName, L"..") != 0 &&
				UniStrCmpi(data_w.cFileName, L".") != 0)
			{
				DIRENT *f =(DIRENT*) ZeroMalloc(sizeof(DIRENT));

				f->FileNameW = UniCopyStr(data_w.cFileName);
				f->FileName = CopyUniToStr(f->FileNameW); //不能转换中文。。。
				f->Folder = (data_w.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? true : false;

				Add(o, f);
			}

			Zero(&data_w, sizeof(data_w));

			b = FindNextFileW(h, &data_w) == 0? false:true;


		}
		while (b);

		FindClose(h);
	}

	Sort(o);

	d = (DIRLIST*)ZeroMalloc(sizeof(DIRLIST));
	d->NumFiles = LIST_NUM(o);
	d->File = (DIRENT**)ToArray(o);

	ReleaseList(o);

	return d;
}

// Check whether the file exists
bool Win32IsFileExists(char *name)
{
	WIN32IO *o;
	o = (WIN32IO *)Win32FileOpen(name,false,false);
	if (o == NULL)
	{
		return false;
	}
	Win32FileClose(o,true);

	return true;
}
bool Win32IsFileExistsW(wchar_t *name)
{
	WIN32IO *o;
	// Validate arguments
	if (name == NULL)
	{
		return false;
	}
	o = (WIN32IO *)Win32FileOpenW(name,false,false);
	if (o == NULL)
	{
		return false;
	}
	Win32FileClose(o,true);
	return true;
}


// Release the enumeration of the directory 
void FreeDir(DIRLIST *d)
{
	UINT i;
	// Validate arguments
	if (d == NULL)
	{
		return;
	}

	for (i = 0;i < d->NumFiles;i++)
	{
		DIRENT *f = d->File[i];
		Free(f->FileName);
		Free(f->FileNameW);
		Free(f);
	}
	Free(d->File);
	Free(d);
}

// Raise the priority
void Win32SetHighPriority()
{
	SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
}

// Restore the priority
void Win32RestorePriority()
{
	SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);
}

void ConbinePathW(wchar_t *dst, UINT size, wchar_t *dirname, wchar_t *filename)
{
	bool is_full_path;
	wchar_t tmp[MAX_SIZE];
	wchar_t filename_ident[MAX_SIZE];
	// Validate arguments
	if (dst == NULL || dirname == NULL || filename == NULL)
	{
		return;
	}

	NormalizePathW(filename_ident, sizeof(filename_ident), filename);

	is_full_path = false;

	if (UniStartWith(filename_ident, L"\\") || UniStartWith(filename_ident, L"/"))
	{
		is_full_path = true;
	}

	filename = &filename_ident[0];

	if (UniStrLen(filename) >= 2)
	{
		if ((L'a' <= filename[0] && filename[0] <= L'z') || (L'A' <= filename[0] && filename[0] <= L'Z'))
		{
			if (filename[1] == L':')
			{
				is_full_path = true;
			}
		}
	}


	if (is_full_path == false)
	{
		UniStrCpy(tmp, sizeof(tmp), dirname);
		if (UniEndWith(tmp, L"/") == false && UniEndWith(tmp, L"\\") == false)
		{
			UniStrCat(tmp, sizeof(tmp), L"/");
		}
		UniStrCat(tmp, sizeof(tmp), filename);
	}
	else
	{
		UniStrCpy(tmp, sizeof(tmp), filename);
	}

	NormalizePathW(dst, size, tmp);
}
// Normalize the file path
void NormalizePathW(wchar_t *dst, UINT size, wchar_t *src)
{
	wchar_t tmp[MAX_SIZE];
	UNI_TOKEN_LIST *t;
	bool first_double_slash = false;
	bool first_single_slash = false;
	wchar_t win32_drive_char = 0;
	bool is_full_path = false;
	UINT i;
	SK *sk;
	// Validate arguments
	if (dst == NULL || src == 0)
	{
		return;
	}

	// Convert the path (Win32, UNIX conversion)
	UniStrCpy(tmp, sizeof(tmp), src);
	ConvertPathW(tmp);
	UniTrim(tmp);

	// If the path begins with "./ " or " ../", replace it to the current directory
	if (UniStartWith(tmp, L"./") || UniStartWith(tmp, L".\\") ||
		UniStartWith(tmp, L"../") || UniStartWith(tmp, L"..\\") ||
		UniStrCmpi(tmp, L".") == 0 || UniStrCmpi(tmp, L"..") == 0)
	{
		wchar_t cd[MAX_SIZE];
		Zero(cd, sizeof(cd));

		Win32GetCurrentDirW(cd, sizeof(cd));


		if (UniStartWith(tmp, L".."))
		{
			UniStrCat(cd, sizeof(cd), L"/../");
			UniStrCat(cd, sizeof(cd), tmp + 2);
		}
		else
		{
			UniStrCat(cd, sizeof(cd), L"/");
			UniStrCat(cd, sizeof(cd), tmp);
		}

		UniStrCpy(tmp, sizeof(tmp), cd);
	}

	// If the path starts with "~/", replace it with the home directory
	if (UniStartWith(tmp, L"~/") || UniStartWith(tmp, L"~\\"))
	{
		wchar_t tmp2[MAX_SIZE];
		GetHomeDirW(tmp2, sizeof(tmp2));
		UniStrCat(tmp2, sizeof(tmp2), L"/");
		UniStrCat(tmp2, sizeof(tmp2), tmp + 2);
		UniStrCpy(tmp, sizeof(tmp), tmp2);
	}

	if (UniStartWith(tmp, L"//") || UniStartWith(tmp, L"\\\\"))
	{
        // Begin with "//" or "\\"
		first_double_slash = true;
		is_full_path = true;
	}
	else if (UniStartWith(tmp, L"/") || UniStartWith(tmp, L"\\"))
	{
		// Begin with "\"
		first_single_slash = true;
		is_full_path = true;
	}


	if (UniStrLen(tmp) >= 2)
	{
		if (tmp[1] == L':')
		{
			// The drive string representation of the Win32
			wchar_t tmp2[MAX_SIZE];
			is_full_path = true;
			win32_drive_char = tmp[0];
			UniStrCpy(tmp2, sizeof(tmp2), tmp + 2);
			UniStrCpy(tmp, sizeof(tmp), tmp2);
		}
	}


	if (UniStrLen(tmp) == 1 && (tmp[0] == L'/' || tmp[0] == L'\\'))
	{
		tmp[0] = 0;
	}

	// Tokenize
	t = UniParseToken(tmp, L"/\\");

	sk = NewSk();

	for (i = 0;i < t->NumTokens;i++)
	{
		wchar_t *s = t->Token[i];

		if (UniStrCmpi(s, L".") == 0)
		{
			continue;
		}
		else if (UniStrCmpi(s, L"..") == 0)
		{
			if (sk->num_item >= 1 && (first_double_slash == false || sk->num_item >= 2))
			{
				Pop(sk);
			}
		}
		else
		{
			Push(sk, s);
		}
	}

	// Token concatenation
	UniStrCpy(tmp, sizeof(tmp), L"");

	if (first_double_slash)
	{
		UniStrCat(tmp, sizeof(tmp), L"//");
	}
	else if (first_single_slash)
	{
		UniStrCat(tmp, sizeof(tmp), L"/");
	}

	if (win32_drive_char != 0)
	{
		wchar_t d[2];
		d[0] = win32_drive_char;
		d[1] = 0;
		UniStrCat(tmp, sizeof(tmp), d);
		UniStrCat(tmp, sizeof(tmp), L":/");
	}

	for (i = 0;i < sk->num_item;i++)
	{
		UniStrCat(tmp, sizeof(tmp), (wchar_t *)sk->p[i]);
		if (i != (sk->num_item - 1))
		{
			UniStrCat(tmp, sizeof(tmp), L"/");
		}
	}

	ReleaseSk(sk);

	UniFreeToken(t);

	ConvertPathW(tmp);

	UniStrCpy(dst, size, tmp);
}

// Get the home directory
void GetHomeDirW(wchar_t *path, UINT size)
{
	// Validate arguments
	if (path == NULL)
	{
		return;
	}

	if (GetEnvW(L"HOME", path, size) == false)
	{
		wchar_t drive[MAX_SIZE];
		wchar_t hpath[MAX_SIZE];
		if (GetEnvW(L"HOMEDRIVE", drive, sizeof(drive)) &&
			GetEnvW(L"HOMEPATH", hpath, sizeof(hpath)))
		{
			UniFormat(path, sizeof(path), L"%s%s", drive, hpath);
		}
		else
		{
			Win32GetCurrentDirW(path, size);
		}
	}
}


// Enumeration of direction with all sub directories
TOKEN_LIST *EnumDirWithSubDirs(char *dirname, bool isAddDir)
{
	TOKEN_LIST *ret;
	UNI_TOKEN_LIST *ret2;
	wchar_t tmp[MAX_SIZE];
	// Validate arguments
	if (dirname == NULL)
	{
		dirname = "./";
	}

	StrToUni(tmp, sizeof(tmp), dirname);

	ret2 = EnumDirWithSubDirsW(tmp, isAddDir);

	ret = UniTokenListToTokenList(ret2);

	UniFreeToken(ret2);

	return ret;
}
UNI_TOKEN_LIST *EnumDirWithSubDirsW(wchar_t *dirname,bool isAddDir)
{
	ENUM_DIR_WITH_SUB_DATA d;
	UNI_TOKEN_LIST *ret;
	UINT i;
	// Validate arguments
	if (dirname == NULL)
	{
		dirname = L"./";
	}

	Zero(&d, sizeof(d));

	d.FileList = NewListFast(NULL);

	EnumDirWithSubDirsMain(&d, dirname,isAddDir);

	ret = (UNI_TOKEN_LIST*)ZeroMalloc(sizeof(UNI_TOKEN_LIST));

	ret->NumTokens = LIST_NUM(d.FileList);
	ret->Token = (wchar_t **)ZeroMalloc(sizeof(wchar_t *) * ret->NumTokens);

	for (i = 0;i < ret->NumTokens;i++)
	{
		wchar_t *s = (wchar_t *)LIST_DATA(d.FileList, i);

		ret->Token[i] = UniCopyStr(s);
	}

	FreeStrList(d.FileList);

	return ret;
}

void EnumDirWithSubDirsMain(ENUM_DIR_WITH_SUB_DATA *d, wchar_t *dirname,bool isAddDir)
{
	DIRLIST *dir;
	UINT i;
	// Validate arguments
	if (d == NULL || dirname == NULL)
	{
		return;
	}

	dir = EnumDirExW(dirname, NULL);
	if (dir == NULL)
	{
		return;
	}

	// Files
	for (i = 0;i < dir->NumFiles;i++)
	{
		DIRENT *e = dir->File[i];

		if (e->Folder == false || isAddDir)
		{
			wchar_t tmp[MAX_SIZE];

			ConbinePathW(tmp, sizeof(tmp), dirname, e->FileNameW);

			Add(d->FileList, CopyUniStr(tmp));
		}
	}

	// Sub directories
	for (i = 0;i < dir->NumFiles;i++)
	{
		DIRENT *e = dir->File[i];

		if (e->Folder)
		{
			wchar_t tmp[MAX_SIZE];

			ConbinePathW(tmp, sizeof(tmp), dirname, e->FileNameW);

			EnumDirWithSubDirsMain(d, tmp,isAddDir);
		}
	}

	FreeDir(dir);
}

DIRLIST *EnumDirExW(wchar_t *dirname, COMPARE *compare)
{
	DIRLIST *d = NULL;
	// Validate arguments
	if (dirname == NULL)
	{
		dirname = L"./";
	}

	if (compare == NULL)
	{
		compare = CompareDirListByName;
	}


	d = Win32EnumDirExW(dirname, compare);


	return d;
}


bool MsIsDirectory(char *name)
{
	DWORD ret;
	//char tmp[MAX_PATH];
	// Validate arguments
	if (name == NULL)
	{
		return false;
	}

	//InnerFilePath(tmp, sizeof(tmp), name);

	ret = GetFileAttributesA(name);
	if (ret == 0xffffffff)
	{
		return false;
	}

	if (ret & FILE_ATTRIBUTE_DIRECTORY)
	{
		return true;
	}

	return false;
}
bool MsIsDirectoryW(wchar_t *name)
{
	return MsUniIsDirectory(name,true);
}
// Get whether the specified file name is a directory
bool MsUniIsDirectory(wchar_t *name,bool isUnicode)
{
	DWORD ret;
	// Validate arguments
	if (name == NULL)
	{
		return false;
	}

	if (isUnicode == false)
	{
		char *s = CopyUniToStr(name);
		ret = MsIsDirectory(s) == true ? 1:0;
		Free(s);

		return ret == 1;
	}

	ret = GetFileAttributesW(name);
	if (ret == 0xffffffff)
	{
		return false;
	}

	if (ret & FILE_ATTRIBUTE_DIRECTORY)
	{
		return true;
	}

	return false;
}

bool MsFileDelete(char *name)
{
	// Validate arguments
	if (name == NULL)
	{
		return false;
	}

	return DeleteFileA(name) == 0 ?false:true;
}

bool MsUniFileDelete(wchar_t *name)
{
	// Validate arguments
	if (name == NULL)
	{
		return false;
	}

	return DeleteFileW(name) == 0 ?false:true;
}
bool MsDirectoryDelete(char *name)
{
	// Validate arguments
	if (name == NULL)
	{
		return false;
	}

	return RemoveDirectoryA(name) == 0 ?false:true;
}
// Delete the directory
bool MsUniDirectoryDelete(wchar_t *name)
{
	// Validate arguments
	if (name == NULL)
	{
		return false;
	}

	return RemoveDirectoryW(name) == 0 ?false:true;
}
bool MsMakeDir(char *name)
{
	// Validate arguments
	if (name == NULL)
	{
		return false;
	}

	return CreateDirectoryA(name, NULL) == 0 ?false:true;
}
// Create a directory
bool MsUniMakeDir(wchar_t *name)
{
	// Validate arguments
	if (name == NULL)
	{
		return false;
	}

	return CreateDirectoryW(name, NULL) == 0 ?false:true;
}
void MsMakeDirEx(char *name)
{
	wchar_t *name_w = CopyStrToUni(name);

	MsUniMakeDirEx(name_w,false);

	Free(name_w);
}
void MsMakeDirExW(wchar_t *name_w)
{
	MsUniMakeDirEx(name_w,true);

	Free(name_w);
}
// Recursive directory creation
void MsUniMakeDirEx(wchar_t *name,bool isUnicode)
{
	UINT wp;
	wchar_t *tmp;
	UINT i, len;
	// Validate arguments
	if (name == NULL)
	{
		return;
	}

	tmp = (wchar_t*)ZeroMalloc(UniStrSize(name) * 2);
	wp = 0;
	len = UniStrLen(name);
	for (i = 0;i < len;i++)
	{
		wchar_t c = name[i];

		if (c == '\\')
		{
			if (UniStrCmpi(tmp, L"\\\\") != 0 && UniStrCmpi(tmp, L"\\") != 0)
			{
				if(isUnicode == true){
					MsUniMakeDir(tmp);
				}else {
					char *s = CopyUniToStr(tmp);
					bool ret = MsMakeDir(s);
					Free(s);
				}
			}
		}

		tmp[wp++] = c;
	}

	Free(tmp);

	MsUniMakeDir(name);
}


/*********************************** File end ******************************/


/************************************** stack start ***********************************/
// Creating a Stack
SK *NewSk()
{
	return NewSkEx(false);
}
SK *NewSkEx(bool no_compact)
{
	SK *s;

	s = (SK *)Malloc(sizeof(SK));
	//s->lock = NewLock();
	//s->ref = NewRef();
	s->num_item = 0;
	s->num_reserved = INIT_NUM_RESERVED;
	s->p = (void**)Malloc(sizeof(void *) * s->num_reserved);
	s->no_compact = no_compact;



	return s;
}
// Release of the stack
void ReleaseSk(SK *s)
{
	// Validate arguments
	if (s == NULL)
	{
		return;
	}

	//if (Release(s->ref) == 0)
	//{
		CleanupSk(s);
	//}
}
// Clean up the stack
void CleanupSk(SK *s)
{
	// Validate arguments
	if (s == NULL)
	{
		return;
	}

	// Memory release
	Free(s->p);
	//DeleteLock(s->lock);
	Free(s);


}

// Pop from the stack
void *Pop(SK *s)
{
	void *ret;
	// Validate arguments
	if (s == NULL)
	{
		return NULL;
	}
	if (s->num_item == 0)
	{
		return NULL;
	}
	ret = s->p[s->num_item - 1];
	s->num_item--;

	// Size reduction
	if (s->no_compact == false)
	{
		// Not to shrink when no_compact is true
		if ((s->num_item * 2) <= s->num_reserved)
		{
			if (s->num_reserved >= (INIT_NUM_RESERVED * 2))
			{
				s->num_reserved = s->num_reserved / 2;
				s->p = (void**)ReAlloc(s->p, sizeof(void *) * s->num_reserved);
			}
		}
	}

	return ret;
}
// Push to the stack
void Push(SK *s, void *p)
{
	UINT i;
	// Validate arguments
	if (s == NULL || p == NULL)
	{
		return;
	}

	i = s->num_item;
	s->num_item++;

	// Size expansion
	if (s->num_item > s->num_reserved)
	{
		s->num_reserved = s->num_reserved * 2;
		s->p = (void**)ReAlloc(s->p, sizeof(void *) * s->num_reserved);
	}
	s->p[i] = p;


}

/************************************** stack end ***********************************/


/************************************** lock start ***********************************/
// Creating a lock
LOCK *Win32NewLock()
{
	// Memory allocation
	LOCK *lock = (LOCK *)Win32MemoryAlloc(sizeof(LOCK));

	// Allocate a critical section
	CRITICAL_SECTION *critical_section = (CRITICAL_SECTION*)Win32MemoryAlloc(sizeof(CRITICAL_SECTION));

	if (lock == NULL || critical_section == NULL)
	{
		Win32MemoryFree(lock);
		Win32MemoryFree(critical_section);
		return NULL;
	}

	// Initialize the critical section
	InitializeCriticalSection(critical_section);

	lock->pData = (void *)critical_section;
	lock->Ready = true;

	return lock;
}

// Lock
bool Win32Lock(LOCK *lock)
{
	CRITICAL_SECTION *critical_section;
	if (lock->Ready == false)
	{
		// State is invalid
		return false;
	}

	// Enter the critical section
	critical_section = (CRITICAL_SECTION *)lock->pData;
	EnterCriticalSection(critical_section);

	return true;
}

// Unlock
void Win32Unlock(LOCK *lock)
{
	Win32UnlockEx(lock, false);
}
void Win32UnlockEx(LOCK *lock, bool inner)
{
	CRITICAL_SECTION *critical_section;
	if (lock->Ready == false && inner == false)
	{
		// State is invalid
		return;
	}

	// Leave the critical section
	critical_section = (CRITICAL_SECTION *)lock->pData;
	LeaveCriticalSection(critical_section);
}

// Delete the lock
void Win32DeleteLock(LOCK *lock)
{
	CRITICAL_SECTION *critical_section;
	// Reset the Ready flag safely
	Win32Lock(lock);
	lock->Ready = false;
	Win32UnlockEx(lock, true);

	// Delete the critical section
	critical_section = (CRITICAL_SECTION *)lock->pData;
	DeleteCriticalSection(critical_section);

	// Memory release
	Win32MemoryFree(critical_section);
	Win32MemoryFree(lock);
}

LOCK *NewLock()
{
	LOCK *lock = NewLockMain();
	return lock;
}
// Delete the lock object
void DeleteLock(LOCK *lock)
{
	// Validate arguments
	if (lock == NULL)
	{
		return;
	}

	Win32DeleteLock(lock);
}

// Create a lock object
LOCK *NewLockMain()
{
	LOCK *lock = NULL;
	UINT retry = 0;

	while (true)
	{
		if ((retry++) > OBJECT_ALLOC__MAX_RETRY)
		{
			AbortExitEx("error: OSNewLock() failed.\n\n");
		}
		lock = Win32NewLock(); //使用临界值
		if (lock != NULL)
		{
			break;
		}
		SleepThread(OBJECT_ALLOC_FAIL_SLEEP_TIME);
	}

	return lock;
}
/************************************** lock end ***********************************/



/************************************** event start ***********************************/
// Initialization of the event
void Win32InitEvent(EVENT *event)
{
	// Creating an auto-reset event
	HANDLE hEvent = CreateEvent(NULL, //  lpEventAttributes     一般为NULL
		FALSE,  //如果true,人工复位,   一旦该Event被设置为有信号,则它一直会等到ResetEvent()API被调用时才会恢复 为无信号.
				//如果设置为FALSE，当事件被一个等待线程释放以后，系统将会自动将事件状态复原为无信号状态。
		FALSE, //初始状态,true,有信号,false无信号   
		NULL);  //事件对象的名称。您在OpenEvent函数中可能使用。

	event->pData = hEvent;
}

// Set the event
void Win32SetEvent(EVENT *event)
{
	HANDLE hEvent = (HANDLE)event->pData;
	if (hEvent == NULL)
	{
		return;
	}

	SetEvent(hEvent);
}

// Reset the event
void Win32ResetEvent(EVENT *event)
{
	HANDLE hEvent = (HANDLE)event->pData;
	if (hEvent == NULL)
	{
		return;
	}

	ResetEvent(hEvent);
}

// Wait for the event
bool Win32WaitEvent(EVENT *event, UINT timeout)
{
	HANDLE hEvent = (HANDLE)event->pData;
	UINT ret;
	if (hEvent == NULL)
	{
		return false;
	}

	// Wait for an object
	ret = WaitForSingleObject(hEvent, timeout);
	if (ret == WAIT_TIMEOUT)
	{
		// Time-out
		return false;
	}
	else
	{
		// Signaled state
		return true;
	}
}

// Release of the event
void Win32FreeEvent(EVENT *event)
{
	HANDLE hEvent = (HANDLE)event->pData;
	if (hEvent == NULL)
	{
		return;
	}

	CloseHandle(hEvent);
}

/************************************** event end ***********************************/

/************************************** pack start ***********************************/
// Convert the PACK to the BUF
BUF *PackToBuf(PACK *p)
{
	BUF *b;
	// Validate arguments
	if (p == NULL)
	{
		return NULL;
	}

	b = NewBuf();
	WritePack(b, p);

	return b;
}
// Convert the BUF to a PACK
PACK *BufToPack(BUF *b)
{
	PACK *p;
	// Validate arguments
	if (b == NULL)
	{
		return NULL;
	}

	p = NewPack();
	if (ReadPack(b, p) == false)
	{
		FreePack(p);
		return NULL;
	}

	return p;
}
// Create a PACK object
PACK *NewPack()
{
	PACK *p;

	// Memory allocation
	p = (PACK *)Malloc(sizeof(PACK));

	// Creating a List
	p->elements = NewListFast(ComparePackName);

	return p;
}
// Read the PACK
bool ReadPack(BUF *b, PACK *p)
{
	UINT i, num;
	// Validate arguments
	if (b == NULL || p == NULL)
	{
		return false;
	}

	// The number of ELEMENTs
	num = ReadBufInt(b);
	if (num > MAX_ELEMENT_NUM)
	{
		// Number exceeds
		return false;
	}

	// Read the ELEMENT
	for (i = 0;i < num;i++)
	{
		ELEMENT *e;
		e = ReadElement(b);
		if (AddElement(p, e) == false)
		{
			// Adding error
			return false;
		}
	}

	return true;
}
// Write down the PACK
void WritePack(BUF *b, PACK *p)
{
	UINT i;
	// Validate arguments
	if (b == NULL || p == NULL)
	{
		return;
	}

	// The number of ELEMENTs
	WriteBufInt(b, LIST_NUM(p->elements));

	// Write the ELEMENT
	for (i = 0;i < LIST_NUM(p->elements);i++)
	{
		ELEMENT *e = (ELEMENT*)LIST_DATA(p->elements, i);
		WriteElement(b, e);
	}
}

// Release of the PACK object
void FreePack(PACK *p)
{
	UINT i;
	ELEMENT **elements;
	// Validate arguments
	if (p == NULL)
	{
		return;
	}

	elements = (ELEMENT **)ToArray(p->elements);
	for (i = 0;i < LIST_NUM(p->elements);i++)
	{
		FreeElement(elements[i]);
	}
	Free(elements);

	ReleaseList(p->elements);
	Free(p);
}
// Get a list of the element names in the PACK
TOKEN_LIST *GetPackElementNames(PACK *p)
{
	TOKEN_LIST *ret;
	UINT i;
	// Validate arguments
	if (p == NULL)
	{
		return NULL;
	}

	ret = (TOKEN_LIST*)ZeroMalloc(sizeof(TOKEN_LIST));

	ret->NumTokens = LIST_NUM(p->elements);
	ret->Token = (char **)ZeroMalloc(sizeof(char *) * ret->NumTokens);

	for (i = 0;i < ret->NumTokens;i++)
	{
		ELEMENT *e = (ELEMENT *)LIST_DATA(p->elements, i);

		ret->Token[i] = CopyStr(e->name);
	}

	return ret;
}

// Get a buffer from the PACK
BUF *PackGetBuf(PACK *p, char *name)
{
	return PackGetBufEx(p, name, 0);
}
BUF *PackGetBufEx(PACK *p, char *name, UINT index)
{
	UINT size;
	void *tmp;
	BUF *b;
	// Validate arguments
	if (p == NULL || name == NULL)
	{
		return NULL;
	}

	size = PackGetDataSizeEx(p, name, index);
	tmp = Malloc(size);
	if (PackGetDataEx(p, name, tmp, index) == false)
	{
		Free(tmp);
		return NULL;
	}

	b = NewBuf();
	WriteBuf(b, tmp, size);
	SeekBuf(b, 0, 0);

	Free(tmp);

	return b;
}

// Get the data from the PACK
bool PackGetData(PACK *p, char *name, void *data)
{
	return PackGetDataEx(p, name, data, 0);
}
bool PackGetDataEx(PACK *p, char *name, void *data, UINT index)
{
	ELEMENT *e;
	// Validate arguments
	if (p == NULL || name == NULL)
	{
		return false;
	}

	e = GetElement(p, name, VALUE_DATA);
	if (e == NULL)
	{
		return false;
	}
	Copy(data, GetDataValue(e, index), GetDataValueSize(e, index));
	return true;
}
bool PackGetData2(PACK *p, char *name, void *data, UINT size)
{
	return PackGetDataEx2(p, name, data, size, 0);
}
bool PackGetDataEx2(PACK *p, char *name, void *data, UINT size, UINT index)
{
	ELEMENT *e;
	// Validate arguments
	if (p == NULL || name == NULL)
	{
		return false;
	}

	e = GetElement(p, name, VALUE_DATA);
	if (e == NULL)
	{
		return false;
	}
	if (GetDataValueSize(e, index) != size)
	{
		return false;
	}
	Copy(data, GetDataValue(e, index), GetDataValueSize(e, index));
	return true;
}

// Get the data size from the PACK
UINT PackGetDataSize(PACK *p, char *name)
{
	return PackGetDataSizeEx(p, name, 0);
}
UINT PackGetDataSizeEx(PACK *p, char *name, UINT index)
{
	ELEMENT *e;
	// Validate arguments
	if (p == NULL || name == NULL)
	{
		return 0;
	}

	e = GetElement(p, name, VALUE_DATA);
	if (e == NULL)
	{
		return 0;
	}
	return GetDataValueSize(e, index);
}

// Get an integer from the PACK
UINT64 PackGetInt64(PACK *p, char *name)
{
	return PackGetInt64Ex(p, name, 0);
}
UINT64 PackGetInt64Ex(PACK *p, char *name, UINT index)
{
	ELEMENT *e;
	// Validate arguments
	if (p == NULL || name == NULL)
	{
		return 0;
	}

	e = GetElement(p, name, VALUE_INT64);
	if (e == NULL)
	{
		return 0;
	}
	return GetInt64Value(e, index);
}

// Get the index number from the PACK
UINT PackGetIndexCount(PACK *p, char *name)
{
	ELEMENT *e;
	// Validate arguments
	if (p == NULL || name == NULL)
	{
		return 0;
	}

	e = GetElement(p, name, INFINITE);
	if (e == NULL)
	{
		return 0;
	}

	return e->num_value;
}

// Get the number from the PACK
UINT PackGetNum(PACK *p, char *name)
{
	return MIN(PackGetInt(p, name), 65536);
}

// Get a bool type from the PACK
bool PackGetBool(PACK *p, char *name)
{
	return PackGetInt(p, name) == 0 ? false : true;
}
bool PackGetBoolEx(PACK *p, char *name, UINT index)
{
	return PackGetIntEx(p, name, index) == 0 ? false : true;
}

// Add a bool type into the PACK
void PackAddBool(PACK *p, char *name, bool b)
{
	PackAddInt(p, name, b ? 1 : 0);
}
void PackAddBoolEx(PACK *p, char *name, bool b, UINT index, UINT total)
{
	PackAddIntEx(p, name, b ? 1 : 0, index, total);
}

// Get an integer from the PACK
UINT PackGetInt(PACK *p, char *name)
{
	return PackGetIntEx(p, name, 0);
}
UINT PackGetIntEx(PACK *p, char *name, UINT index)
{
	ELEMENT *e;
	// Validate arguments
	if (p == NULL || name == NULL)
	{
		return 0;
	}

	e = GetElement(p, name, VALUE_INT);
	if (e == NULL)
	{
		return 0;
	}
	return GetIntValue(e, index);
}

// Get an Unicode string from the PACK
bool PackGetUniStr(PACK *p, char *name, wchar_t *unistr, UINT size)
{
	return PackGetUniStrEx(p, name, unistr, size, 0);
}
bool PackGetUniStrEx(PACK *p, char *name, wchar_t *unistr, UINT size, UINT index)
{
	ELEMENT *e;
	// Validate arguments
	if (p == NULL || name == NULL || unistr == NULL || size == 0)
	{
		return false;
	}

	unistr[0] = 0;

	e = GetElement(p, name, VALUE_UNISTR);
	if (e == NULL)
	{
		return false;
	}
	UniStrCpy(unistr, size, GetUniStrValue(e, index));
	return true;
}

// Compare strings in the PACK
bool PackCmpStr(PACK *p, char *name, char *str)
{
	char tmp[MAX_SIZE];

	if (PackGetStr(p, name, tmp, sizeof(tmp)) == false)
	{
		return false;
	}

	if (StrCmpi(tmp, str) == 0)
	{
		return true;
	}

	return false;
}

// Get a string from the PACK
bool PackGetStr(PACK *p, char *name, char *str, UINT size)
{
	return PackGetStrEx(p, name, str, size, 0);
}
bool PackGetStrEx(PACK *p, char *name, char *str, UINT size, UINT index)
{
	ELEMENT *e;
	// Validate arguments
	if (p == NULL || name == NULL || str == NULL || size == 0)
	{
		return false;
	}

	str[0] = 0;

	e = GetElement(p, name, VALUE_STR);
	if (e == NULL)
	{
		return false;
	}

	StrCpy(str, size, GetStrValue(e, index));
	return true;
}

// Add the buffer to the PACK (array)
void PackAddBufEx(PACK *p, char *name, BUF *b, UINT index, UINT total)
{
	// Validate arguments
	if (p == NULL || name == NULL || b == NULL || total == 0)
	{
		return;
	}

	PackAddDataEx(p, name, b->Buf, b->Size, index, total);
}

// Add the data to the PACK (array)
void PackAddDataEx(PACK *p, char *name, void *data, UINT size, UINT index, UINT total)
{
	VALUE *v;
	ELEMENT *e;
	// Validate arguments
	if (p == NULL || data == NULL || name == NULL || total == 0)
	{
		return;
	}

	v = NewDataValue(data, size);
	e = GetElement(p, name, VALUE_DATA);
	if (e != NULL)
	{
		if (e->num_value <= total)
		{
			e->values[index] = v;
		}
		else
		{
			FreeValue(v, VALUE_DATA);
		}
	}
	else
	{
		e = (ELEMENT*)ZeroMalloc(sizeof(ELEMENT));
		StrCpy(e->name, sizeof(e->name), name);
		e->num_value = total;
		e->type = VALUE_DATA;
		e->values = (VALUE**)ZeroMalloc(sizeof(VALUE *) * total);
		e->values[index] = v;
		AddElement(p, e);
	}
}

// Add the buffer to the PACK
void PackAddBuf(PACK *p, char *name, BUF *b)
{
	// Validate arguments
	if (p == NULL || name == NULL || b == NULL)
	{
		return;
	}

	PackAddData(p, name, b->Buf, b->Size);
}

// Add the data to the PACK
void PackAddData(PACK *p, char *name, void *data, UINT size)
{
	VALUE *v;
	// Validate arguments
	if (p == NULL || data == NULL || name == NULL)
	{
		return;
	}

	v = NewDataValue(data, size);
	AddElement(p, NewElement(name, VALUE_DATA, 1, &v));
}

// Add a 64 bit integer (array) to the PACK
void PackAddInt64Ex(PACK *p, char *name, UINT64 i, UINT index, UINT total)
{
	VALUE *v;
	ELEMENT *e;
	// Validate arguments
	if (p == NULL || name == NULL || total == 0)
	{
		return;
	}

	v = NewInt64Value(i);
	e = GetElement(p, name, VALUE_INT64);
	if (e != NULL)
	{
		if (e->num_value <= total)
		{
			e->values[index] = v;
		}
		else
		{
			FreeValue(v, VALUE_INT64);
		}
	}
	else
	{
		e = (ELEMENT*)ZeroMalloc(sizeof(ELEMENT));
		StrCpy(e->name, sizeof(e->name), name);
		e->num_value = total;
		e->type = VALUE_INT64;
		e->values = (VALUE**)ZeroMalloc(sizeof(VALUE *) * total);
		e->values[index] = v;
		AddElement(p, e);
	}
}

// Add an integer to the PACK (array)
void PackAddIntEx(PACK *p, char *name, UINT i, UINT index, UINT total)
{
	VALUE *v;
	ELEMENT *e;
	// Validate arguments
	if (p == NULL || name == NULL || total == 0)
	{
		return;
	}

	v = NewIntValue(i);
	e = GetElement(p, name, VALUE_INT);
	if (e != NULL)
	{
		if (e->num_value <= total)
		{
			e->values[index] = v;
		}
		else
		{
			FreeValue(v, VALUE_INT);
		}
	}
	else
	{
		e = (ELEMENT*)ZeroMalloc(sizeof(ELEMENT));
		StrCpy(e->name, sizeof(e->name), name);
		e->num_value = total;
		e->type = VALUE_INT;
		e->values = (VALUE**)ZeroMalloc(sizeof(VALUE *) * total);
		e->values[index] = v;
		AddElement(p, e);
	}
}

// Add a 64 bit integer to the PACK
void PackAddInt64(PACK *p, char *name, UINT64 i)
{
	VALUE *v;
	// Validate arguments
	if (p == NULL || name == NULL)
	{
		return;
	}

	v = NewInt64Value(i);
	AddElement(p, NewElement(name, VALUE_INT64, 1, &v));
}

// Add the number of items to the PACK
void PackAddNum(PACK *p, char *name, UINT num)
{
	PackAddInt(p, name, num);
}

// Add an integer to the PACK
void PackAddInt(PACK *p, char *name, UINT i)
{
	VALUE *v;
	// Validate arguments
	if (p == NULL || name == NULL)
	{
		return;
	}

	v = NewIntValue(i);
	AddElement(p, NewElement(name, VALUE_INT, 1, &v));
}

// Add a Unicode string (array) to the PACK
void PackAddUniStrEx(PACK *p, char *name, wchar_t *unistr, UINT index, UINT total)
{
	VALUE *v;
	ELEMENT *e;
	// Validate arguments
	if (p == NULL || name == NULL || unistr == NULL || total == 0)
	{
		return;
	}

	v = NewUniStrValue(unistr);
	e = GetElement(p, name, VALUE_UNISTR);
	if (e != NULL)
	{
		if (e->num_value <= total)
		{
			e->values[index] = v;
		}
		else
		{
			FreeValue(v, VALUE_UNISTR);
		}
	}
	else
	{
		e = (ELEMENT*)ZeroMalloc(sizeof(ELEMENT));
		StrCpy(e->name, sizeof(e->name), name);
		e->num_value = total;
		e->type = VALUE_UNISTR;
		e->values = (VALUE**)ZeroMalloc(sizeof(VALUE *) * total);
		e->values[index] = v;
		AddElement(p, e);
	}
}

// Add a Unicode string to the PACK
void PackAddUniStr(PACK *p, char *name, wchar_t *unistr)
{
	VALUE *v;
	// Validate arguments
	if (p == NULL || name == NULL || unistr == NULL)
	{
		return;
	}

	v = NewUniStrValue(unistr);
	AddElement(p, NewElement(name, VALUE_UNISTR, 1, &v));
}

// Add a string to the PACK (array)
void PackAddStrEx(PACK *p, char *name, char *str, UINT index, UINT total)
{
	VALUE *v;
	ELEMENT *e;
	// Validate arguments
	if (p == NULL || name == NULL || str == NULL || total == 0)
	{
		return;
	}

	v = NewStrValue(str);
	e = GetElement(p, name, VALUE_STR);
	if (e != NULL)
	{
		if (e->num_value <= total)
		{
			e->values[index] = v;
		}
		else
		{
			FreeValue(v, VALUE_STR);
		}
	}
	else
	{
		e = (ELEMENT*)ZeroMalloc(sizeof(ELEMENT));
		StrCpy(e->name, sizeof(e->name), name);
		e->num_value = total;
		e->type = VALUE_STR;
		e->values = (VALUE**)ZeroMalloc(sizeof(VALUE *) * total);
		e->values[index] = v;
		AddElement(p, e);
	}
}

// Add a string to the PACK
void PackAddStr(PACK *p, char *name, char *str)
{
	VALUE *v;
	// Validate arguments
	if (p == NULL || name == NULL || str == NULL)
	{
		return;
	}

	v = NewStrValue(str);
	AddElement(p, NewElement(name, VALUE_STR, 1, &v));
}




// Create a ELEMENT
ELEMENT *NewElement(char *name, UINT type, UINT num_value, VALUE **values)
{
	ELEMENT *e;
	UINT i;
	// Validate arguments
	if (name == NULL || num_value == 0 || values == NULL)
	{
		return NULL;
	}

	// Memory allocation
	e = (ELEMENT*)Malloc(sizeof(ELEMENT));
	StrCpy(e->name, sizeof(e->name), name);
	e->num_value = num_value;
	e->type = type;

	// Copy of the pointer list to the element
	e->values = (VALUE **)Malloc(sizeof(VALUE *) * num_value);
	for (i = 0;i < e->num_value;i++)
	{
		e->values[i] = values[i];
	}

	return e;
}
// Read the ELEMENT
ELEMENT *ReadElement(BUF *b)
{
	UINT i;
	char name[MAX_ELEMENT_NAME_LEN + 1];
	UINT type, num_value;
	VALUE **values;
	ELEMENT *e;
	// Validate arguments
	if (b == NULL)
	{
		return NULL;
	}

	// Name
	if (ReadBufStr(b, name, sizeof(name)) == false)
	{
		return NULL;
	}

	// Type of item
	type = ReadBufInt(b);

	// Number of items
	num_value = ReadBufInt(b);
	if (num_value > MAX_VALUE_NUM)
	{
		// Number exceeds
		return NULL;
	}

	// VALUE
	values = (VALUE **)Malloc(sizeof(VALUE *) * num_value);
	for (i = 0;i < num_value;i++)
	{
		values[i] = ReadValue(b, type);
	}

	// Create a ELEMENT
	e = NewElement(name, type, num_value, values);

	Free(values);

	return e;
}

// Write the ELEMENT
void WriteElement(BUF *b, ELEMENT *e)
{
	UINT i;
	// Validate arguments
	if (b == NULL || e == NULL)
	{
		return;
	}

	// Name
	WriteBufStr(b, e->name);
	// Type of item
	WriteBufInt(b, e->type);
	// Number of items
	WriteBufInt(b, e->num_value);
	// VALUE
	for (i = 0;i < e->num_value;i++)
	{
		VALUE *v = e->values[i];
		WriteValue(b, v, e->type);
	}
}
// Add an ELEMENT to the PACK
bool AddElement(PACK *p, ELEMENT *e)
{
	// Validate arguments
	if (p == NULL || e == NULL)
	{
		return false;
	}

	// Size Check
	if (LIST_NUM(p->elements) >= MAX_ELEMENT_NUM)
	{
		// Can not add any more
		FreeElement(e);
		return false;
	}

	// Check whether there is another item which have same name
	if (GetElement(p, e->name, INFINITE))
	{
		// Exists
		FreeElement(e);
		return false;
	}

	if (e->num_value == 0)
	{
		// VALUE without any items can not be added
		FreeElement(e);
		return false;
	}

	// Adding
	Add(p->elements, e);
	return true;
}
// Search and retrieve a ELEMENT from the PACK
ELEMENT *GetElement(PACK *p, char *name, UINT type)
{
	ELEMENT t;
	ELEMENT *e;
	// Validate arguments
	if (p == NULL || name == NULL)
	{
		return NULL;
	}

	// Search
	StrCpy(t.name, sizeof(t.name), name);
	e = (ELEMENT*)Search(p->elements, &t);

	if (e == NULL)
	{
		return NULL;
	}

	// Type checking
	if (type != INFINITE)
	{
		if (e->type != type)
		{
			return NULL;
		}
	}

	return e;
}
// Remove the ELEMENT from the PACK
void DelElement(PACK *p, char *name)
{
	ELEMENT *e;
	// Validate arguments
	if (p == NULL || name == NULL)
	{
		return;
	}

	e = GetElement(p, name, INFINITE);
	if (e != NULL)
	{
		Delete(p->elements, e);

		FreeElement(e);
	}
}


// Delete the ELEMENT
void FreeElement(ELEMENT *e)
{
	UINT i;
	// Validate arguments
	if (e == NULL)
	{
		return;
	}

	for (i = 0;i < e->num_value;i++)
	{
		FreeValue(e->values[i], e->type);
	}
	Free(e->values);

	Free(e);
}




// Delete the VALUE
void FreeValue(VALUE *v, UINT type)
{
	// Validate arguments
	if (v == NULL)
	{
		return;
	}

	switch (type)
	{
	case VALUE_INT:
	case VALUE_INT64:
		break;
	case VALUE_DATA:
		Free(v->Data);
		break;
	case VALUE_STR:
		Free(v->Str);
		break;
	case VALUE_UNISTR:
		Free(v->UniStr);
		break;
	}

	// Memory release
	Free(v);
}



// Create the VALUE of integer type
VALUE *NewIntValue(UINT i)
{
	VALUE *v;

	// Memory allocation
	v = (VALUE *)Malloc(sizeof(VALUE));
	v->IntValue = i;
	v->Size = sizeof(UINT);

	return v;
}
// Create the VALUE of 64 bit integer type
VALUE *NewInt64Value(UINT64 i)
{
	VALUE *v;

	v = (VALUE *)Malloc(sizeof(VALUE));
	v->Int64Value = i;
	v->Size = sizeof(UINT64);

	return v;
}
// Create the VALUE of the data type
VALUE *NewDataValue(void *data, UINT size)
{
	VALUE *v;
	// Validate arguments
	if (data == NULL)
	{
		return NULL;
	}

	// Memory allocation
	v =(VALUE *) Malloc(sizeof(VALUE));

	// Data copy
	v->Size = size;
	v->Data = Malloc(v->Size);
	Copy(v->Data, data, size);

	return v;
}
// Creation of the VALUE of ANSI string type
VALUE *NewStrValue(char *str)
{
	VALUE *v;
	// Validate arguments
	if (str == NULL)
	{
		return NULL;
	}

	// Memory allocation
	v = (VALUE *)Malloc(sizeof(VALUE));

	// String copy
	v->Size = StrLen(str) + 1;
	v->Str = (char*)Malloc(v->Size);
	StrCpy(v->Str, v->Size, str);

	Trim(v->Str);

	return v;
}
// Create a VALUE of Unicode String type
VALUE *NewUniStrValue(wchar_t *str)
{
	VALUE *v;
	// Validate arguments
	if (str == NULL)
	{
		return NULL;
	}

	// Memory allocation
	v = (VALUE *)Malloc(sizeof(VALUE));

	// String copy
	v->Size = UniStrSize(str);
	v->UniStr = (wchar_t *)Malloc(v->Size);
	UniStrCpy(v->UniStr, v->Size, str);

	UniTrim(v->UniStr);

	return v;
}
// Read the VALUE
VALUE *ReadValue(BUF *b, UINT type)
{
	UINT len;
	BYTE *u;
	void *data;
	char *str;
	wchar_t *unistr;
	UINT unistr_size;
	UINT size;
	UINT u_size;
	VALUE *v = NULL;
	// Validate arguments
	if (b == NULL)
	{
		return NULL;
	}

	// Data item
	switch (type)
	{
	case VALUE_INT:			// Integer
		v = NewIntValue(ReadBufInt(b));
		break;
	case VALUE_INT64:
		v = NewInt64Value(ReadBufInt64(b));
		break;
	case VALUE_DATA:		// Data
		size = ReadBufInt(b);
		if (size > MAX_VALUE_SIZE)
		{
			// Size over
			break;
		}
		data = Malloc(size);
		if (ReadBuf(b, data, size) != size)
		{
			// Read failure
			Free(data);
			break;
		}
		v = NewDataValue(data, size);
		Free(data);
		break;
	case VALUE_STR:			// ANSI string
		len = ReadBufInt(b);
		if ((len + 1) > MAX_VALUE_SIZE)
		{
			// Size over
			break;
		}
		str = (char*)Malloc(len + 1);
		// String body
		if (ReadBuf(b, str, len) != len)
		{
			// Read failure
			Free(str);
			break;
		}
		str[len] = 0;
		v = NewStrValue(str);
		Free(str);
		break;
	case VALUE_UNISTR:		// Unicode string
		u_size = ReadBufInt(b);
		if (u_size > MAX_VALUE_SIZE)
		{
			// Size over
			break;
		}
		// Reading an UTF-8 string
		u = (BYTE*)ZeroMalloc(u_size + 1);
		if (ReadBuf(b, u, u_size) != u_size)
		{
			// Read failure
			Free(u);
			break;
		}
		// Convert to a Unicode string
		unistr_size = CalcUtf8ToUni(u, u_size);
		if (unistr_size == 0)
		{
			Free(u);
			break;
		}
		unistr = (wchar_t *)Malloc(unistr_size);
		Utf8ToUni(unistr, unistr_size, u, u_size);
		Free(u);
		v = NewUniStrValue(unistr);
		Free(unistr);
		break;
	}

	return v;
}
// Write the VALUE
void WriteValue(BUF *b, VALUE *v, UINT type)
{
	UINT len;
	BYTE *u;
	UINT u_size;
	// Validate arguments
	if (b == NULL || v == NULL)
	{
		return;
	}

	// Data item
	switch (type)
	{
	case VALUE_INT:			// Integer
		WriteBufInt(b, v->IntValue);
		break;
	case VALUE_INT64:		// 64 bit integer
		WriteBufInt64(b, v->Int64Value);
		break;
	case VALUE_DATA:		// Data
		// Size
		WriteBufInt(b, v->Size);
		// Body
		WriteBuf(b, v->Data, v->Size);
		break;
	case VALUE_STR:			// ANSI string
		len = StrLen(v->Str);
		// Length
		WriteBufInt(b, len);
		// String body
		WriteBuf(b, v->Str, len);
		break;
	case VALUE_UNISTR:		// Unicode string
		// Convert to UTF-8
		u_size = CalcUniToUtf8(v->UniStr) + 1;
		u = (BYTE*)ZeroMalloc(u_size);
		UniToUtf8(u, u_size, v->UniStr);
		// Size
		WriteBufInt(b, u_size);
		// UTF-8 string body
		WriteBuf(b, u, u_size);
		Free(u);
		break;
	}
}

// Get data size
UINT GetDataValueSize(ELEMENT *e, UINT index)
{
	// Validate arguments
	if (e == NULL)
	{
		return 0;
	}
	if (e->values == NULL)
	{
		return 0;
	}
	if (index >= e->num_value)
	{
		return 0;
	}
	if (e->values[index] == NULL)
	{
		return 0;
	}

	return e->values[index]->Size;
}

// Get the data
void *GetDataValue(ELEMENT *e, UINT index)
{
	// Validate arguments
	if (e == NULL)
	{
		return NULL;
	}
	if (e->values == NULL)
	{
		return NULL;
	}
	if (index >= e->num_value)
	{
		return NULL;
	}
	if (e->values[index] == NULL)
	{
		return NULL;
	}

	return e->values[index]->Data;
}

// Get the Unicode string type
wchar_t *GetUniStrValue(ELEMENT *e, UINT index)
{
	// Validate arguments
	if (e == NULL)
	{
		return 0;
	}
	if (index >= e->num_value)
	{
		return 0;
	}
	if (e->values[index] == NULL)
	{
		return NULL;
	}

	return e->values[index]->UniStr;
}

// Get the ANSI string type
char *GetStrValue(ELEMENT *e, UINT index)
{
	// Validate arguments
	if (e == NULL)
	{
		return 0;
	}
	if (index >= e->num_value)
	{
		return 0;
	}
	if (e->values[index] == NULL)
	{
		return NULL;
	}

	return e->values[index]->Str;
}

// Get the 64 bit integer value
UINT64 GetInt64Value(ELEMENT *e, UINT index)
{
	// Validate arguments
	if (e == NULL)
	{
		return 0;
	}
	if (index >= e->num_value)
	{
		return 0;
	}
	if (e->values[index] == NULL)
	{
		return 0;
	}

	return e->values[index]->Int64Value;
}

// Get the integer value
UINT GetIntValue(ELEMENT *e, UINT index)
{
	// Validate arguments
	if (e == NULL)
	{
		return 0;
	}
	if (index >= e->num_value)
	{
		return 0;
	}
	if (e->values[index] == NULL)
	{
		return 0;
	}

	return e->values[index]->IntValue;
}

/************************************** pack end ***********************************/

/************************************** mfc start ***********************************/
// Enumerate the child windows of all that is in the specified window
LIST *EnumAllChildWindow(HWND hWnd)
{
	return EnumAllChildWindowEx(hWnd, false, false, false);
}

LIST *EnumAllChildWindowEx(HWND hWnd, bool no_recursion, bool include_ipcontrol, bool no_self)
{
	ENUM_CHILD_WINDOW_PARAM p;
	LIST *o = NewWindowList();

	Zero(&p, sizeof(p));
	p.include_ipcontrol = include_ipcontrol;
	p.no_recursion = no_recursion;
	p.o = o;

	if (no_self == false)
	{
		AddWindow(o, hWnd);
	}

	EnumChildWindows(hWnd, EnumChildWindowProc, (LPARAM)&p);

	return o;
}
// Child window enumeration procedure
BOOL CALLBACK EnumChildWindowProc(HWND hWnd, LPARAM lParam)
{
	ENUM_CHILD_WINDOW_PARAM *p = (ENUM_CHILD_WINDOW_PARAM *)lParam;
	LIST *o;
	HWND hParent;
	char c1[MAX_SIZE], c2[MAX_SIZE];
	// Validate arguments
	if (hWnd == NULL || p == NULL)
	{
		return TRUE;
	}

	o = p->o;

	Zero(c1, sizeof(c1));
	Zero(c2, sizeof(c2));

	hParent = GetParent(hWnd);

	GetClassNameA(hWnd, c1, sizeof(c1));

	if (hParent != NULL)
	{
		GetClassNameA(hParent, c2, sizeof(c2));
	}

	if (p->include_ipcontrol || (StrCmpi(c1, "SysIPAddress32") != 0 && (IsEmptyStr(c2) || StrCmpi(c2, "SysIPAddress32") != 0)))
	{
		AddWindow(o, hWnd);

		if (p->no_recursion == false)
		{
			//EnumChildWindows(hWnd, EnumChildWindowProc, (LPARAM)p); //not need
		}
	}

	return TRUE;
}

LIST *EnumAllWindow()
{
	return EnumAllWindowEx(false, false);
}
LIST *EnumAllWindowEx(bool no_recursion, bool include_ipcontrol)
{
	ENUM_CHILD_WINDOW_PARAM p;
	LIST *o = NewWindowList();

	Zero(&p, sizeof(p));
	p.o = o;
	p.no_recursion = no_recursion;
	p.include_ipcontrol = include_ipcontrol;

	EnumWindows(EnumChildWindowProc, (LPARAM)&p);

	return o;
}
LIST *EnumAllTopWindow()
{
	LIST *o = NewWindowList();

	EnumWindows(EnumTopWindowProc, (LPARAM)o);

	return o;
}
// Window enumeration procedure
BOOL CALLBACK EnumTopWindowProc(HWND hWnd, LPARAM lParam)
{
	LIST *o = (LIST *)lParam;
	HWND hParent;
	char c1[MAX_SIZE], c2[MAX_SIZE];
	// Validate arguments
	if (hWnd == NULL || o == NULL)
	{
		return TRUE;
	}

	Zero(c1, sizeof(c1));
	Zero(c2, sizeof(c2));

	hParent = GetParent(hWnd);

	GetClassNameA(hWnd, c1, sizeof(c1));

	if (hParent != NULL)
	{
		GetClassNameA(hParent, c2, sizeof(c2));
	}

	if (StrCmpi(c1, "SysIPAddress32") != 0 && (IsEmptyStr(c2) || StrCmpi(c2, "SysIPAddress32") != 0))
	{
		AddWindow(o, hWnd);
	}

	return TRUE;
}
// Search for the window
HWND SearchWindow(char *caption)
{
	SEARCH_WINDOW_PARAM p;
	wchar_t *caption_w;
	// Validate arguments
	if (caption == NULL)
	{
		return NULL;
	}
	caption_w = CopyStrToUni(caption);
	Zero(&p, sizeof(p));
	p.caption = caption_w;
	p.hWndFound = NULL;

	EnumWindows(SearchWindowEnumProcW, (LPARAM)&p);

	Free(caption_w);
	return p.hWndFound;
}


HWND SearchWindowW(wchar_t *caption)
{
	SEARCH_WINDOW_PARAM p;
	// Validate arguments
	if (caption == NULL)
	{
		return NULL;
	}

	Zero(&p, sizeof(p));
	p.caption = caption;
	p.hWndFound = NULL;

	EnumWindows(SearchWindowEnumProcW, (LPARAM)&p);

	return p.hWndFound;
}
// Window Searching procedure
BOOL CALLBACK SearchWindowEnumProcW(HWND hWnd, LPARAM lParam)
{
	if (hWnd != NULL && lParam != 0)
	{
		wchar_t *s = GetTextW(hWnd, 0);
		SEARCH_WINDOW_PARAM *p = (SEARCH_WINDOW_PARAM *)lParam;
		if (s != NULL)
		{
			if (UniStrCmpi(p->caption, s) == 0)
			{
				p->hWndFound = hWnd;
			}
			Free(s);
		}
	}
	return TRUE;
}
char *GetTextA(HWND hWnd, UINT id)
{
	char *ret;
	UINT size, len;
	// Validate arguments
	if (hWnd == NULL)
	{
		return NULL;
	}

	len = GetWindowTextLengthA(DlgItem(hWnd, id));
	if (len == 0)
	{
		return CopyStr("");
	}

	size = len + 1;
	ret = (char*)ZeroMalloc(size);

	GetWindowTextA(DlgItem(hWnd, id), ret, size);

	return ret;
}
// Get the text string
wchar_t *GetTextW(HWND hWnd, UINT id)
{
	wchar_t *ret;
	UINT size, len;
	// Validate arguments
	if (hWnd == NULL)
	{
		return NULL;
	}

	len = GetWindowTextLengthW(DlgItem(hWnd, id));
	if (len == 0)
	{
		return CopyUniStr(L"");
	}

	size = (len + 1) * 2;
	ret = (wchar_t*)ZeroMalloc(size);

	GetWindowTextW(DlgItem(hWnd, id), ret, size);

	return ret;
}
// Get the item in the dialog
HWND DlgItem(HWND hWnd, UINT id)
{
	// Validate arguments
	if (hWnd == NULL)
	{
		return NULL;
	}

	if (id == 0)
	{
		return hWnd;
	}
	else
	{
		return GetDlgItem(hWnd, id);
	}
}
// Move the window to the center
void Center(HWND hWnd)
{
	RECT screen;
	RECT win;
	UINT x, y;
	UINT win_x, win_y;
	// Validate arguments
	if (hWnd == NULL)
	{
		return;
	}

	if (SystemParametersInfo(SPI_GETWORKAREA, 0, &screen, 0) == false)
	{
		return;
	}

	GetWindowRect(hWnd, &win);
	win_x = win.right - win.left;
	win_y = win.bottom - win.top;

	if (win_x < (UINT)(screen.right - screen.left))
	{
		x = (screen.right - screen.left - win_x) / 2;
	}
	else
	{
		x = 0;
	}

	if (win_y < (UINT)(screen.bottom - screen.top))
	{
		y = (screen.bottom - screen.top - win_y) / 2;
	}
	else
	{
		y = 0;
	}

	SetWindowPos(hWnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
}


//list start
// Add items to the list view
void LvInsert(HWND hWnd, UINT id, UINT num_str , ...)
{
	UINT i;
	va_list va;
	UINT index = 0;
	HWND hList;
	if (hWnd == NULL)
	{
		return;
	}
	hList = ::GetDlgItem(hWnd, id);
	va_start(va, num_str);

	LVITEMA vitem;  
	Zero(&vitem,sizeof(vitem));
	vitem.mask = LVIF_TEXT;
	for (i = 0;i < num_str;i++)
	{
		char *s = va_arg(va, char *);
		
		if (i == 0)
		{

			vitem.pszText = s;  
			vitem.iItem = ListView_GetItemCount(hList);  
			vitem.iSubItem = 0;  
			//index = ListView_InsertItem(, );  
			index = (int)::SendMessageA((hList), LVM_INSERTITEMA, 0, (LPARAM)(const LV_ITEM *)(&vitem));
		}
		else
		{
			// 设置子项  
			vitem.pszText = s;  
			vitem.iItem = index;  
			vitem.iSubItem = i;
			::SendMessageA((hList), LVM_SETITEMA, 0, (LPARAM)(const LV_ITEM *)(&vitem));

		}
	}

	va_end(va);
}
// Get the number of items
UINT LvNum(HWND hWnd, UINT id)
{
	// Validate arguments
	if (hWnd == NULL)
	{
		return 0;
	}

	return ListView_GetItemCount(DlgItem(hWnd, id));
}

// Get the currently selected item
UINT LvGetSelected(HWND hWnd, UINT id)
{
	// Validate arguments
	if (hWnd == NULL)
	{
		return INFINITE;
	}

	return ListView_GetNextItem(DlgItem(hWnd, id), -1, LVNI_FOCUSED | LVNI_SELECTED);
}

// Remove an item
void LvDeleteItem(HWND hWnd, UINT id, UINT index)
{
	UINT i,num;
	// Validate arguments
	if (hWnd == NULL)
	{
		return;
	}

	ListView_DeleteItem(DlgItem(hWnd, id), index);
	num = LvNum(hWnd,id);
	if(num > 0){
		i = num-1 > index?index:num -1;
		LvSelect(hWnd, id, i);
	}

}

// Select an item
void LvSelect(HWND hWnd, UINT id, UINT index)
{
	// Validate arguments
	if (hWnd == NULL)
	{
		return;
	}

	if (index == INFINITE)
	{
		UINT i, num;
		// Deselect all
		num = LvNum(hWnd, id);
		for (i = 0;i < num;i++)
		{
			ListView_SetItemState(DlgItem(hWnd, id), i, 0, LVIS_SELECTED);
		}
	}
	else
	{
		// Select
		ListView_SetItemState(DlgItem(hWnd, id), index, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
		ListView_EnsureVisible(DlgItem(hWnd, id), index, true);
	}
}

// Get whether there is currently selected item
bool LvIsSelected(HWND hWnd, UINT id)
{
	// Validate arguments
	if (hWnd == NULL)
	{
		return false;
	}

	if (LvGetSelected(hWnd, id) == INFINITE)
	{
		return false;
	}

	return true;
}

// Rename the item
void LvRename(HWND hWnd, UINT id, UINT pos)
{
	// Validate arguments
	if (hWnd == NULL || pos == INFINITE)
	{
		return;
	}

	ListView_EditLabel(DlgItem(hWnd, id), pos);
}
void LvSetItemA(HWND hWnd, UINT id, UINT index, UINT pos, char *str)
{
	LVITEMA t;
	char *old_str;

	// Validate arguments
	if (hWnd == NULL || str == NULL)
	{
		return;
	}

	Zero(&t, sizeof(t));
	t.mask = LVIF_TEXT;
	t.pszText = str;
	t.iItem = index;
	t.iSubItem = pos;

	old_str = LvGetStrA(hWnd, id, index, pos);

	if (StrCmp(old_str, str) != 0)
	{
		::SendMessageA(DlgItem(hWnd, id), LVM_SETITEMA, 0, (LPARAM)&t);
	}

	Free(old_str);
}
char *LvGetStrA(HWND hWnd, UINT id, UINT index, UINT pos)
{
	char *tmp;
	UINT size;
	LVITEMA t;
	// Validate arguments
	if (hWnd == NULL)
	{
		return NULL;
	}

	size = 65536;
	tmp = (char*)Malloc(size);

	Zero(&t, sizeof(t));
	t.mask = LVIF_TEXT;
	t.iItem = index;
	t.iSubItem = pos;
	t.pszText = tmp;
	t.cchTextMax = size;

	if (SendMessageA(DlgItem(hWnd, id), LVM_GETITEMTEXTA, index, (LPARAM)&t) <= 0)
	{
		Free(tmp);
		return CopyStr("");
	}
	else
	{
		char *ret = CopyStr(tmp);
		Free(tmp);
		return ret;
	}
}

//list end
/************************************** mfc end ***********************************/


/************************************** socket start ***********************************/
#if 1

// TCP-SSL receive
UINT SecureRecv(SOCK *sock, void *data, UINT size)
{
	//ret = SSL_read(ssl, data, size);
	return 0;
}

// TCP disconnect
void Disconnect(SOCK *sock){
	//TODO
	closesocket(sock->socket);

}
// TCP receive
UINT Recv(SOCK *sock, void *data, UINT size, bool secure)
{
	SOCKET s;
	int ret;

	// Validate arguments
	if (sock == NULL || data == NULL || size == 0)
	{
		return 0;
	}


	if (secure)
	{
		//return SecureRecv(sock, data, size);
	}

	// Receive
	s = sock->socket;



	ret = recv(s, (char*)data, size, 0);



	if (ret > 0)
	{
		// Successful reception
		{
			sock->RecvSize += (UINT64)ret;
			sock->SendNum++;
		}
		return (UINT)ret;
	}

	// Transmission failure
	if (sock->AsyncMode)
	{
		// In asynchronous mode, examine the error
		if (ret == SOCKET_ERROR)
		{
#ifdef	WIN32
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				// In blocking
				return SOCK_LATER;
			}
			else
			{
				//Debug("Socket Error: %u\n", WSAGetLastError());
			}
#else	// WIN32
			if (errno == EAGAIN)
			{
				// In blocking
				return SOCK_LATER;
			}
#endif	// WIN32
		}
	}

	// Disconnected
	Disconnect(sock);
	return 0;
}

// Receive all by TCP
bool RecvAll(SOCK *sock, void *data, UINT size, bool secure)
{
	UINT recv_size, sz, ret;
	// Validate arguments
	if (sock == NULL || data == NULL)
	{
		return false;
	}
	if (size == 0)
	{
		return true;
	}

	recv_size = 0;

	while (true)
	{
		sz = size - recv_size;
		ret = Recv(sock, (UCHAR *)data + recv_size, sz, secure);
		if (ret == 0)
		{
			return false;
		}
		recv_size += ret;
		if (recv_size >= size)
		{
			return true;
		}
	}
}

// Receive a PACK
PACK *RecvPack(SOCK *s)
{
	PACK *p;
	BUF *b;
	void *data;
	UINT sz;
	// Validate arguments
	if (s == NULL || s->Type != SOCK_TCP)
	{
		return false;
	}

	if (RecvAll(s, &sz, sizeof(UINT), s->SecureMode) == false)
	{
		return false;
	}
	sz = Endian32(sz);
	if (sz > MAX_PACK_SIZE)
	{
		return false;
	}
	data = Malloc(sz);
	if (RecvAll(s, data, sz, s->SecureMode) == false)
	{
		Free(data);
		return false;
	}

	b = NewBuf();
	WriteBuf(b, data, sz);
	SeekBuf(b, 0, 0);
	p = BufToPack(b);
	FreeBuf(b);
	Free(data);

	return p;
}
// SHA-1 specific hash function
void HashSha1(void *dst, void *src, UINT size)
{
	// Validate arguments
	if (dst == NULL || (size != 0 && src == NULL))
	{
		return;
	}
	//TODO
	//SHA1(src, size, dst);
}
// Receive a PACK (with checking the hash)
PACK *RecvPackWithHash(SOCK *s)
{
	PACK *p;
	BUF *b;
	void *data;
	UINT sz;
	UCHAR hash1[SHA1_SIZE];
	UCHAR hash2[SHA1_SIZE];
	// Validate arguments
	if (s == NULL || s->Type != SOCK_TCP)
	{
		return false;
	}

	if (RecvAll(s, &sz, sizeof(UINT), s->SecureMode) == false)
	{
		return false;
	}
	sz = Endian32(sz);
	if (sz > MAX_PACK_SIZE)
	{
		return false;
	}
	data = Malloc(sz);
	if (RecvAll(s, data, sz, s->SecureMode) == false)
	{
		Free(data);
		return false;
	}

	HashSha1(hash1, data, sz);
	if (RecvAll(s, hash2, sizeof(hash2), s->SecureMode) == false)
	{
		Free(data);
		return false;
	}

	if (Cmp(hash1, hash2, SHA1_SIZE) != 0)
	{
		Free(data);
		return false;
	}

	b = NewBuf();
	WriteBuf(b, data, sz);
	SeekBuf(b, 0, 0);
	p = BufToPack(b);
	FreeBuf(b);
	Free(data);

	return p;
}

// Append to the TCP send buffer
void SendAdd(SOCK *sock, void *data, UINT size)
{
	// Validate arguments
	if (sock == NULL || data == NULL || size == 0 || sock->AsyncMode != false)
	{
		return;
	}

	WriteBuf(sock->SendBuf, data, size);
}
UINT SecureSend(SOCK *sock, void *data, UINT size){
	//TODO
	/*
	ret = SSL_write(ssl, data, size);
	if (ret < 0)
	{
		e = SSL_get_error(ssl, ret);
	}
	if (ret > 0)
	{
		// Successful transmission
		sock->SendSize += (UINT64)ret;
		sock->SendNum++;
		sock->WriteBlocked = false;
		return (UINT)ret;
	}
	if (ret == 0)
	{
		// Disconnect
		Debug("%s %u SecureSend() Disconnect\n", __FILE__, __LINE__);
		Disconnect(sock);
		return 0;
	}
	*/
	return 0;
}

// TCP transmission
UINT Send(SOCK *sock, void *data, UINT size, bool secure)
{
	SOCKET s;
	int ret;
	// Validate arguments
	if (sock == NULL || data == NULL || size == 0)
	{
		return 0;
	}

	size = MIN(size, MAX_SEND_BUF_MEM_SIZE);
	if (sock->Type != SOCK_TCP || sock->Connected == false || sock->ListenMode != false ||
		sock->socket == INVALID_SOCKET)
	{
		return 0;
	}
	if (secure != false && sock->SecureMode == false)
	{
		return 0;
	}

	if (secure)
	{
		return SecureSend(sock, data, size);
	}

	// Transmission
	s = sock->socket;
	ret = send(s, (char*)data, size, 0);
	if (ret > 0)
	{
		// Successful transmission
		//Lock(sock->lock);
		{
			sock->SendSize += (UINT64)ret;
			sock->SendNum++;
		}
		//Unlock(sock->lock);
		sock->WriteBlocked = false;
		return (UINT)ret;
	}

	// Transmission failure
	if (sock->AsyncMode)
	{
		// In asynchronous mode, examine the error
		if (ret == SOCKET_ERROR)
		{
#ifdef	WIN32
			if (WSAGetLastError() == WSAEWOULDBLOCK)
			{
				// In blocking
				sock->WriteBlocked = true;
				return SOCK_LATER;
			}
			else
			{
				//Debug("Socket Error: %u\n", WSAGetLastError());
			}
#else	// WIN32
			if (errno == EAGAIN)
			{
				// In blocking
				sock->WriteBlocked = true;
				return SOCK_LATER;
			}
#endif	// WIN32
		}
	}

	// Disconnected
	Disconnect(sock);
	return 0;
}

// Send all by TCP
bool SendAll(SOCK *sock, void *data, UINT size, bool secure)
{
	UCHAR *buf;
	UINT sent_size;
	UINT ret;
	// Validate arguments
	if (sock == NULL || data == NULL)
	{
		return false;
	}
	if (sock->AsyncMode)
	{
		return false;
	}
	if (size == 0)
	{
		return true;
	}

	buf = (UCHAR *)data;
	sent_size = 0;

	while (true)
	{
		ret = Send(sock, buf, size - sent_size, secure);
		if (ret == 0)
		{
			return false;
		}
		sent_size += ret;
		buf += ret;
		if (sent_size >= size)
		{
			return true;
		}
	}
}

// Send the TCP send buffer
bool SendNow(SOCK *sock, int secure)
{
	bool ret;
	// Validate arguments
	if (sock == NULL || sock->AsyncMode != false)
	{
		return false;
	}
	if (sock->SendBuf->Size == 0)
	{
		return true;
	}

	ret = SendAll(sock, sock->SendBuf->Buf, sock->SendBuf->Size, secure>0);
	ClearBuf(sock->SendBuf);

	return ret;
}



// Send a PACK
bool SendPack(SOCK *s, PACK *p)
{
	BUF *b;
	UINT sz;
	// Validate arguments
	if (s == NULL || p == NULL || s->Type != SOCK_TCP)
	{
		return false;
	}

	b = PackToBuf(p);
	sz = Endian32(b->Size);

	SendAdd(s, &sz, sizeof(UINT));
	SendAdd(s, b->Buf, b->Size);
	FreeBuf(b);

	return SendNow(s, s->SecureMode);
}

// Creating a new socket
SOCK *NewSock()
{
	SOCK *s = (SOCK *)ZeroMallocFast(sizeof(SOCK));

	//s->ref = NewRef();
	s->lock = NewLock();
	s->SendBuf = NewBuf();
	s->socket = INVALID_SOCKET;
	s->ssl_lock = NewLock();
	s->disconnect_lock = NewLock();

	

	return s;
}

// Clean-up of the socket
void CleanupSock(SOCK *s)
{
	// Validate arguments
	if (s == NULL)
	{
		return;
	}

//	{Debug("CleanupSock: Disconnect() Called: %s %u\n", __FILE__, __LINE__);Disconnect(s);}
	Disconnect(s);



	FreeBuf(s->SendBuf);
	if (s->socket != INVALID_SOCKET)
	{
		closesocket(s->socket);
	}
	Free(s->RemoteHostname);



	Free(s->WaitToUseCipher);
	DeleteLock(s->lock);
	DeleteLock(s->ssl_lock);
	DeleteLock(s->disconnect_lock);

	

	Free(s);
}

// Release of the socket
void ReleaseSock(SOCK *s)
{
	// Validate arguments
	if (s == NULL)
	{
		return;
	}


	{
		if (s->ListenMode == false && s->ServerMode)
		{
			//Print("");
		}
		CleanupSock(s);
	}
}

// Convert the IP to UINT
UINT IPToUINT(IP *ip)
{
	UCHAR *b;
	UINT i, value = 0;
	// Validate arguments
	if (ip == NULL)
	{
		return 0;
	}

	b = (UCHAR *)&value;
	for (i = 0;i < 4;i++)
	{
		b[i] = ip->addr[i];
	}

	return value;
}
// Set an IPv6 address
void SetIP6(IP *ip, UCHAR *value)
{
	// Validate arguments
	if (ip == NULL)
	{
		return;
	}

	Zero(ip, sizeof(IP));

	ip->addr[0] = 223;
	ip->addr[1] = 255;
	ip->addr[2] = 255;
	ip->addr[3] = 254;

	if (value != NULL)
	{
		UINT i;

		for (i = 0;i < 16;i++)
		{
			ip->ipv6_addr[i] = value[i];
		}
	}
}
// Check whether the specified address is a IPv6 address
bool IsIP6(IP *ip)
{
	// Validate arguments
	if (ip == NULL)
	{
		return false;
	}

	if (ip->addr[0] == 223 && ip->addr[1] == 255 && ip->addr[2] == 255 && ip->addr[3] == 254)
	{
		return true;
	}

	return false;
}
bool IsIP4(IP *ip)
{
	// Validate arguments
	if (ip == NULL)
	{
		return false;
	}

	return (IsIP6(ip) ? false : true);
}
// Examine whether the version of the two IP addresses are same
bool IsSameIPVer(IP *ip1, IP *ip2)
{
	// Validate arguments
	if (ip1 == NULL || ip2 == NULL)
	{
		return false;
	}

	if (IsIP4(ip1) && IsIP4(ip2))
	{
		return true;
	}

	if (IsIP6(ip1) && IsIP6(ip2))
	{
		return true;
	}

	return false;
}

// Create an IPv6 address of all zero
void ZeroIP6(IP *ip)
{
	// Validate arguments
	if (ip == NULL)
	{
		return;
	}

	SetIP6(ip, NULL);
}
// Create an IPv4 address of all zero
void ZeroIP4(IP *ip)
{
	// Validate arguments
	if (ip == NULL)
	{
		return;
	}

	Zero(ip, sizeof(IP));
}
// Convert UINT to IP
void UINTToIP(IP *ip, UINT value)
{
	UCHAR *b;
	UINT i;
	// Validate arguments
	if (ip == NULL)
	{
		return;
	}

	ZeroIP4(ip);

	b = (UCHAR *)&value;
	for (i = 0;i < 4;i++)
	{
		ip->addr[i] = b[i];
	}
}


#endif
/************************************** socket end ***********************************/