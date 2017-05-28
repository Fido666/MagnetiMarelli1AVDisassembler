#pragma once
#include "types.h"

namespace Utils
{
	bool isDataRep(const ubyte* b, int minRep, ubyte data);
	int checksum16(const ubyte* data, int length);
	bool isASCIINum(ubyte chr);
	bool isASCIIChar(ubyte chr);
	bool checkPattern(const char* patt, const ubyte* buff);
	int findPattern(const char* patt, const ubyte* b, int length);
	int getDiff(const ubyte* buff, const ubyte* sample, int size);
	bool isDescending(const ubyte* buff, int buffSize);
	bool isAscending(const ubyte* buff, int buffSize);
	int findNearest(const ubyte* buff, int buffSize, const ubyte* sample, int sampleSize, int offset);
	int findMap(const ubyte* buff, int buffSize, int mapSize, int low, int high, int offset);

	int lowercpy(char* dest, const char* src);
}

