#pragma once
#include "types.h"
#include <vector>

class XMLDump;

struct AxisDescriptor
{
	uint desc; //sram address
	ushort size;
	int labels[32];
	void* address;
	DataType dataType;
	int GetLabelValue(uint index) const;

	AxisDescriptor(): desc(0), size(0), address(nullptr), dataType(DT_UNKNOWN) {}
	bool operator==(const AxisDescriptor& other)
	{
		return(desc == other.desc &&
				size == other.size &&
				((address && address == other.address) ||
				memcmp(labels, other.labels, size * sizeof(labels[0])) == 0));
	}
};

struct Descriptor
{
	uint desc;
	int min, max;
	ushort minSize, maxSize;
	int count;
	std::string name;
	DataType dataType;
	float scale;
	float offset;
	Descriptor(): desc(0), min(~0), max(0), minSize(~0), maxSize(0), count(0),
	dataType(DT_UNKNOWN), scale(1), offset(0) {}
};

enum MapType
{
	MT_UNKNOWN,
	MT_FUEL,
	MT_SPARK,
	MT_AIR,
	MT_MISC,
	MT_TEMP_KOMP,
	MT_TRANSITION,
	MT_RPM,
};

enum XDFDataType
{
	XT_ENGINE_SPEED = 6,
	XT_AIR_TEMP = 19,
	XT_COOLANT_TEMP = 13,
	XT_LOAD = 23,
	XT_THROTTLE_POS = 57,
	XT_BATTERY_VOLTAGE = 66,
	XT_TIME = 0,
	XT_SPARK_ADVANCE,
	XT_AIRFLOW,
};

struct MapDescriptor
{
	DataType dtype;
	int xdesc;
	int ydesc;
	int begin;
	int end;
	int dataOffset;

	MapType type;
	const char* name;
	float scale;
	float offset;
	MapDescriptor(): dtype(DT_UBYTE), xdesc(-1), ydesc(-1), begin(0), end(0), dataOffset(0),
		type(MT_UNKNOWN), name(nullptr), scale(1), offset(0) {}
};

struct MapDescriptorInfo
{
	MapType type;
	int dataOffset;
	const char* name;
	float scale;
	float offset;
};

class MapFinder
{
public:
	std::vector<Descriptor> descriptors;
	std::vector<AxisDescriptor> axes;
	std::vector<MapDescriptor> maps;
	ubyte* buffer;
	uint bufferSize;
	ubyte* used;

	MapFinder(const ubyte* buff, uint size)
	{
		buffer = new ubyte[size];
		memcpy(buffer, buff, size);
		used = new ubyte[(size + 7) >> 3];
		memset(used, 0, (size + 7) >> 3); 
		bufferSize = size;
	}

	void AddMapInfo(uint addr, const char* name, MapType type, float scale, float offset);
	void AddDescriptorInfo(uint desc, const char* name, DataType dataType, float scale, float offset);

	~MapFinder()
	{
		delete[] buffer;
		delete[] used;
	}

	inline void UseOffset(int offset)
	{
		used[offset >> 3] |= 1 << (offset & 7);
	}

	void UseRegion(int offset, int size)
	{
		for(int n = 0; n < size; n++)
			UseOffset(offset + n);
	}

	int AddMap(const MapDescriptor& desc)
	{
		UseRegion(desc.begin, desc.end - desc.begin);
		maps.push_back(desc);
		return maps.size() - 1;
	}

	inline bool UsedOffset(int offset) const
	{
		return (used[offset >> 3] & (1 << (offset & 7))) != 0;
	}

	bool UsedRegion(int offset, int size) const
	{
		for(int n = 0; n < size; n++)
		{
			if(UsedOffset(offset + n))
				return true;
		}
		return false;
	}

	void Add3DMap(uint addr, const char* name, DataType dt, MapType type, float scale, float offset, int xdesc, int ydesc)
	{
		MapDescriptor md;
		
		md.dtype = dt;
		md.dataOffset = addr;
		md.name = name;
		md.type = type;
		md.scale = scale;
		md.offset = offset;
		md.xdesc = xdesc;
		md.ydesc = ydesc;
		int byteSize = type == DT_UBYTE ? 1 : 2;
		md.begin = addr;
		md.end = addr + axes[xdesc].size * axes[ydesc].size * byteSize;
		AddMap(md);
	}

	int AddAxisDescriptor(const AxisDescriptor& desc);

	const Descriptor& FindDescriptorInfo(uint desc) const
	{
		for(uint n = 0; n < descriptors.size(); n++)
		{
			if(descriptors[n].desc == desc)
				return descriptors[n];
		}
		static const Descriptor f;
		return f;
	}

	void DumpAxis(XMLDump& dump, const char* name, const AxisDescriptor& desc);
	void DumpTable(XMLDump& dump, const MapDescriptor& map);
	void DumpSeparator(XMLDump& dump, const char* title);
	void DumpTables(XMLDump& dump, MapType mt);
	void DumpTables(XMLDump& dump);

};


