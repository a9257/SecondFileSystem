#include "Utility.h"
#include <string.h>

int Utility::Min(int a, int b)
{
	if (a < b)
		return a;
	return b;
}

void Utility::IOMove(unsigned char *from, unsigned char *to, int count)
{
	while (count--)
	{
		*to++ = *from++;
	}
	return;
}

void Utility::DWordCopy(int *src, int *dst, int count)
{
	while (count--)
	{
		*dst++ = *src++;
	}
	return;
}

int Utility::StringLength(char *pString)
{
	int length = 0;
	char *pChar = pString;

	while (*pChar++)
	{
		length++;
	}

	/* 返回字符串长度 */
	return length;
}

void Utility::StringCopy(char *src, char *dst)
{
	while ((*dst++ = *src++) != 0)
		;
}
