#include "stdafx.h"
#include "disa.h"

namespace Disa
{
//---------------------------------------------------------------------
Context::Context(uint len): length(len)
{
	memoryUsage = new ubyte[len];
	memset(memoryUsage, DU_EMPTY, len);
}

//---------------------------------------------------------------------
Location* Context::FindLocation(uint offset) const
{
	Location loc(LT_INVALID, offset);
	TLocationSet::iterator i = locations.find(&loc);
	if(i != locations.end())
	{
		return *i;
	}
	return nullptr;
}

//---------------------------------------------------------------------
Location* Context::FindParentLocation(uint offset) const
{
	/*
	Location lo(LT_LABEL, offset);
	auto loc = locations.upper_bound(&lo);
	if(loc == locations.end())
		return nullptr;
	
	Location* l = *loc;
	return l;
	*/

	int best = INT_MAX;
	Location* bloc;

	for(auto loc = locations.begin(); loc != locations.end(); ++loc)
	{
		Location* l = (*loc);
		if(l->type == Disa::LT_SUB && l->offset < offset)
		{
			int dif = offset - l->offset;
			if(dif < best)
			{
				best = dif;
				bloc = l;
			}
		}
	}
	return bloc;
}

//---------------------------------------------------------------------
void Context::SetLocationName(uint offset, const char* name)
{
	Location* loc = FindLocation(offset);
	if(loc)
		loc->name = name;
}

//---------------------------------------------------------------------
DataLocation* Context::AddDataLocation(const char* name, uint offset, DataType dt, uint size)
{
	DataLocation* loc = static_cast<DataLocation*>(FindLocation(offset));
	if(loc)
	{
		if(loc->name.empty())
			loc->name = name;
		if(loc->type != LT_DATA)
			return loc;
		if(loc->dtype < dt)
		{
			loc->dtype = dt;
			loc->size = size;
		}
		return loc;
	}

	loc = new DataLocation(offset);
	loc->name = name;
	loc->dtype = dt;
	loc->size = size;
	locations.insert(loc);
	return loc;
}

//---------------------------------------------------------------------
Location* Context::AddProcedure(const char* name, uint offset)
{
	Location* loc = FindLocation(offset);
	if(loc)
	{
		loc->name = name;
		loc->type = LT_SUB;
	}
	else
	{
		loc = new Location(LT_SUB, offset);
		loc->name = name;
		locations.insert(loc);
	}
	return loc;
}

//---------------------------------------------------------------------
Location* Context::HandleTarget(LocationType ltype, uint offset, uint ref)
{
	if(offset == 0x4484)
		offset=offset;

	DataUsage du = Usage(offset);
	if(du != DU_EMPTY)
	{
		if(du != DU_CODE && (ltype == LT_LABEL || ltype == LT_SUB))
			return nullptr;
	}

	Location* loc = FindLocation(offset);

	RefType rt;
	if(ltype == LT_SUB)
	{
		rt = RT_CALL;
	}
	else if(ltype == LT_LABEL)
	{
		rt = RT_JUMP;
	}
	else if(ltype == LT_DATA)
	{
		rt = RT_READ;
	}

	if(loc == nullptr)
	{
		if(ltype == LT_SUB)
		{
			loc = new Location(ltype, offset);
			rt = RT_CALL;
		}
		else if(ltype == LT_LABEL)
		{
			loc = new Location(ltype, offset);
			rt = RT_JUMP;
		}
		else if(ltype == LT_DATA)
		{
			loc = new DataLocation(offset);
			rt = RT_READ;
		}

		locations.insert(loc);
	}
	if(ref)
		loc->references.push_back(Reference(rt, ref));
	return loc;
}

//---------------------------------------------------------------------
void Dump::AddString(const char* str)
{
	strcpy(buffer + lineOffset, str);
	lineOffset += strlen(str);
}

}
