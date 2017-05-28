#include "stdafx.h"
#include "utils.h"
#include <locale>

namespace Utils
{
//---------------------------------------------------------------------
int lowercpy(char* dest, const char* src)
{
	std::locale loc;
	int len = 0;
	while(true)
	{
		*dest++ = std::tolower(src[len], loc);
		if(!src[len])
			break;
		len++;
	}
	return len;
}


//---------------------------------------------------------------------
bool isDataRep(const ubyte* b, int minRep, ubyte data)
{
	for(int n = 0; n < minRep; n++)
	{
		if(b[n] != data)
			return false;
	}
	return true;
}

//---------------------------------------------------------------------
int checksum16(const ubyte* data, int length)
{
	unsigned int chksum = 0;
	for(int n = 0; n < length; n++)
	{
		chksum += data[n];
	}
	return chksum & 0xffff;
}

//---------------------------------------------------------------------
bool isASCIINum(ubyte chr)
{
	return (chr >= '0' && chr <= '9');
}

bool isASCIIChar(ubyte chr)
{
	return (chr >= 'a' && chr <= 'z') || (chr >= 'A' && chr <= 'Z');
}

//---------------------------------------------------------------------
bool checkPattern(const char* patt, const ubyte* buff)
{
	while(*patt)
	{
		switch(*patt)
		{
		case '?':
			break;
		case '#':
			if(!isASCIINum(*buff))
				return false;
			break;
		case '$':
			if(!isASCIIChar(*buff))
				return false;
			break;
		default:
			if((ubyte)*patt != *buff)
				return false;
		}
		patt++;
		buff++;
	}
	return true;
}

//---------------------------------------------------------------------
int findPattern(const char* patt, const ubyte* b, int length)
{
	int pattLength = strlen(patt);

	for(int n = 0; n < length - pattLength; n++)
	{
		if(checkPattern(patt, b + n))
			return n;
	}
	return -1;
}
//---------------------------------------------------------------------
int getDiff(const ubyte* buff, const ubyte* sample, int size)
{
	int diff = 0;
	for(int n = 0; n < size; n++)
	{
		diff += abs(buff[n] - sample[n]);
	}
	return diff;
}

//---------------------------------------------------------------------
bool isDescending(const ubyte* buff, int buffSize)
{
	if(buff[0] == buff[buffSize - 1])
		return false;

	for(int n = 0; n < buffSize - 1; n++)
	{
		if(buff[n] < buff[n + 1])
			return false;
	}
	return true;
}

//---------------------------------------------------------------------
bool isAscending(const ubyte* buff, int buffSize)
{
	for(int n = 0; n < buffSize - 1; n++)
	{
		if(buff[n] > buff[n + 1])
			return false;
	}
	return true;
}

//---------------------------------------------------------------------
int findNearest(const ubyte* buff, int buffSize, const ubyte* sample, int sampleSize, int offset)
{
	int bestOffset = -1;
	int bestDiff = 0x7fffffff;

	for(int n = offset; n < buffSize - sampleSize; n++)
	{
		if(buff[n + 5] != 0)
			continue;
		if(!isDescending(buff + n, 6))
			continue;

		int diff = getDiff(buff + n, sample, sampleSize);
		if(diff < bestDiff)
		{
			bestDiff = diff;
			bestOffset = n;
		}
	}
	return bestOffset;
}

//---------------------------------------------------------------------
int findMap(const ubyte* buff, int buffSize, int mapSize, int low, int high, int offset)
{
	for(int n = offset; n < buffSize - mapSize; n++)
	{
		bool isOk = true;
		for(int s = 0; s < mapSize; s++)
		{
			if(buff[n + s] >= low && buff[n + s] <= high)
				continue;
			isOk = false;
			break;
		}
		if(isOk)
			return n;
	}
	return -1;
}


}
