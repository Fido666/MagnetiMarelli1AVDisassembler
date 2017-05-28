#pragma once
#include <vector>
#include <string>
#include <set>
#include "types.h"

namespace Disa
{
struct OpCodeDef
{
};

enum MemoryType
{
	MT_INVALID,
	MT_RAM,
	MT_ROM,
	MT_IO,
};

enum MemoryProtect
{
	MP_NONE,
	MP_R = 1,
	MP_W = 2,
	MP_X = 4,
};

struct MemoryDef
{
	MemoryType type;
	uint begin, end;
	uint protect;
};

enum RefType
{
	RT_CALL,
	RT_JUMP,
	RT_READ,
	RT_WRITE
};

struct Reference
{
	RefType type;
	uint offset;
	Reference() {}
	Reference(RefType t, uint off): type(t), offset(off) {}
};

enum LocationType
{
	LT_INVALID,
	LT_LABEL,
	LT_SUB,
	LT_DATA,
};

struct Location
{
	uint offset;
	LocationType type;
	std::string name;
	std::vector<Reference> references;
	virtual ~Location() {}
	Location(): type(LT_INVALID), offset(-1) {}
	Location(LocationType lt, uint off): offset(off), type(lt)  {}
};

typedef Disa::Location* LocationPtr;
struct LocationCompare
{
//	bool operator()(Location*const& a, Location*const& b) const
	bool operator()(const LocationPtr& a, const LocationPtr& b) const
	{
		return a->offset < b->offset;
	}
};

typedef std::set<Disa::Location*, LocationCompare> TLocationSet;

struct DataLocation: public Location
{
	DataType dtype;
	uint size;
	DataLocation() {}
	DataLocation(uint off): Location(LT_DATA, off) {}
};

enum DataUsage
{
	DU_EMPTY,
	DU_INVALID,
	DU_DATA,
	DU_CODE,
};

//---------------------------------------------------------------------
class Context
{
protected:
	ubyte*	memoryUsage;
	uint length;
	TLocationSet locations;
	std::vector<MemoryDef> memory;

public:
	Context(uint len);

	void Use(uint offset, DataUsage du, uint len = 1)
	{
		for(uint n = 0; n < len; n++)
			memoryUsage[offset + n] = du;
	}

	DataUsage Usage(uint offset) const
	{
		return (DataUsage)memoryUsage[offset];
	}

	Disa::MemoryType MemType(uint offset)
	{
		for(uint n = 0; n < memory.size(); n++)
		{
			if(offset >= memory[n].begin && offset < memory[n].end)
				return memory[n].type;
		}
		return MT_INVALID;
	}

	uint MemProtect(uint offset)
	{
		for(uint n = 0; n < memory.size(); n++)
		{
			if(offset >= memory[n].begin && offset < memory[n].end)
				return memory[n].protect;
		}
		return 0;
	}

	void AddMemory(uint from, uint to, Disa::MemoryType mt, uint memProtect)
	{
		MemoryDef md;
		md.begin = from;
		md.end = to;
		md.type = mt;
		md.protect = memProtect;
		memory.push_back(md);
	}
	void SetLocationName(uint offset, const char* name);

	Location* FindLocation(uint offset) const;
	Location* FindParentLocation(uint offset) const;

	DataLocation* AddDataLocation(const char* name, uint offset, DataType dt, uint size);
	DataLocation* AddDataLocation(const char* name, uint offset, DataType dt, uint width, uint height);
	Location* AddProcedure(const char* name, uint offset);
	Location* HandleTarget(LocationType ltype, uint offset, uint ref);
};

inline ushort ReadUShortBE(const ubyte* buff, uint offset)
{
	return (buff[offset] << 8) | buff[offset + 1];
}

	class Dump
	{
	protected:
		uint codeOffset;
		uint lineOffset;
		char buffer[256];

	public:
		Dump(): codeOffset(0), lineOffset(0)
		{
		}

		void SetOffset(uint offset)
		{
			codeOffset = 0;
		}

		void Begin(uint offset)
		{
			sprintf(buffer, "%04x ", offset);
			lineOffset = strlen(buffer);
		}
		
		void End()
		{
			strcat(buffer, "\n");
			printf(buffer);
		}

		void AddString(const char* str);
	};

}
