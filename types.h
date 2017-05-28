#pragma once
#include <xutility>

typedef char byte;
typedef unsigned char ubyte;
typedef unsigned short ushort;
typedef unsigned int uint;

enum DataType
{
	DT_UNKNOWN,
	DT_UBYTE,
	DT_BYTE,
	DT_USHORT,
	DT_SHORT,
	DT_UINT,
	DT_INT,
	DT_FLOAT
};

inline uint TypeSizeof(DataType dt)
{
	if(dt == DT_UNKNOWN) return 0;
	if(dt == DT_UBYTE || dt == DT_BYTE) return 1;
	if(dt == DT_USHORT || dt == DT_SHORT) return 2;
	return 4;
}

/*template<class T> T max(const T& v1, const T& v2)
{
	return v1 > v2 ? v1 : v2;
}

template<class T> T min(const T& v1, const T& v2)
{
	return v1 < v2 ? v1 : v2;
}
*/
inline int abs(int a)
{
	return (a < 0) ? -a : a;
}


void Float2Str(char* buff, float v);

