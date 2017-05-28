// decode.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "decode.h"
#include "xmldump.h"
#include <assert.h>

//---------------------------------------------------------------------
void MapFinder::DumpAxis(XMLDump& dump, const char* name, const AxisDescriptor& desc)
{
	const Descriptor& info = FindDescriptorInfo(desc.desc);

	dump.BeginElement("XDFAXIS");
		dump.AddAttribute("id", name);
		char temp[16];
		sprintf(temp, "0xf000%x", (int)desc.desc);
		dump.AddAttribute("uniqueid", temp);
		dump.BeginElement("EMBEDDEDDATA");
			dump.AddAttribute("mmedelementsizebits", "8");
			dump.AddAttribute("mmedmajorstridebits", "-32");
			dump.AddAttribute("mmedminorstridebits", "0");
		dump.EndElement();
		dump.AddElementValue("indexcount", (int)desc.size);
		dump.AddElementValue("datatype", info.dataType);
		dump.AddElementValue("unittype", "0");
		dump.BeginElement("DALINK");
			dump.AddAttribute("index", "0");
		dump.EndElement();
		for(int n = 0; n < desc.size; n++)
		{
			dump.BeginElement("LABEL");
				dump.AddAttribute("index", n);
				dump.AddAttribute("value", (float)desc.labels[n] * info.scale + info.offset);
			dump.EndElement();
		}
		dump.BeginElement("MATH");
			dump.AddAttribute("equation", "X");
			dump.BeginElement("VAR");
				dump.AddAttribute("id", "X");
			dump.EndElement();
		dump.EndElement();
	dump.EndElement();
}

//---------------------------------------------------------------------
void MapFinder::DumpTable(XMLDump& dump, const MapDescriptor& map)
{
	char temp[256];
	dump.BeginElement("XDFTABLE");
		const Descriptor& ydesc = FindDescriptorInfo(axes[map.ydesc].desc);

		sprintf(temp, "0x%x", map.dataOffset);

		dump.AddAttribute("uniqueid", temp);
		dump.AddAttribute("flags", "0x30");

		if(map.xdesc >= 0)
		{
			const Descriptor& xdesc = FindDescriptorInfo(axes[map.xdesc].desc);
			sprintf(temp, "0x%x (%s:%s)(%dx%d)", (int)map.dataOffset, ydesc.name, xdesc.name, (int)axes[map.ydesc].size, (int)axes[map.xdesc].size);
		}
		else
			sprintf(temp, "0x%x (%s)(%dx1)", (int)map.dataOffset, ydesc.name, (int)axes[map.ydesc].size);

		char temp2[128];
		if(map.name)
		{
			strcpy(temp2, map.name);
			strcat(temp2, " ");
			strcat(temp2, temp);
			dump.AddElementValue("title", temp2);
		}
		else
			dump.AddElementValue("title", temp);

		int width = 1;
		if(map.xdesc >= 0)
		{
			DumpAxis(dump, "x", axes[map.xdesc]); 
			width = axes[map.xdesc].size;
		}
		else
		{
			AxisDescriptor dd;
			dd.desc = 0;
			dd.size = 1;
			dd.labels[0] = 1;
			DumpAxis(dump, "x", dd); 
		}

		DumpAxis(dump, "y", axes[map.ydesc]);

		dump.BeginElement("XDFAXIS");
			int maxValue = 255;
			dump.AddAttribute("id", "z");
			dump.BeginElement("EMBEDDEDDATA");
				sprintf(temp, "0x%x", map.dataOffset);
				dump.AddAttribute("mmedaddress", temp);
				int flags = 0;
				//1 signed
				//2 LSB

				if(map.dtype == DT_UBYTE)
				{
					dump.AddAttribute("mmedelementsizebits", "8");
					maxValue = 255;
				}
				if(map.dtype == DT_BYTE)
				{
					dump.AddAttribute("mmedelementsizebits", "8");
					maxValue = 127;
				}
				if(map.dtype == DT_USHORT)
				{
					dump.AddAttribute("mmedelementsizebits", "16");
					maxValue = 65535;
					dump.AddAttribute("mmedtypeflags", "0x02");
				}
				if(map.dtype == DT_SHORT)
				{
					dump.AddAttribute("mmedelementsizebits", "16");
					maxValue = 32767;
					dump.AddAttribute("mmedtypeflags", "0x03");
				}

				dump.AddAttribute("mmedrowcount", (int)axes[map.ydesc].size);
				dump.AddAttribute("mmedcolcount", (int)width);
				dump.AddAttribute("mmedmajorstridebits", "0");
				dump.AddAttribute("mmedminorstridebits", "0");
			dump.EndElement();
			dump.AddElementValue("decimalpl", "2");
			dump.AddElementValue("min", (float)(map.offset));
			dump.AddElementValue("max", (float)(maxValue * map.scale + map.offset));
			dump.AddElementValue("outputtype", "1");
			dump.BeginElement("MATH");
				if(map.scale != 1 || map.offset != 0)
				{
					sprintf(temp, "X * %f + %f", map.scale, map.offset);
					dump.AddAttribute("equation", temp);
				}
				else
					dump.AddAttribute("equation", "X");
				dump.BeginElement("VAR");
					dump.AddAttribute("id", "X");
				dump.EndElement();
			dump.EndElement();
		dump.EndElement();
	dump.EndElement();
}

//---------------------------------------------------------------------
void MapFinder::DumpSeparator(XMLDump& dump, const char* title)
{
	dump.BeginElement("XDFTABLE");
		dump.AddAttribute("uniqueid", "0");
		dump.AddAttribute("flags", "0x30");

		dump.AddElementValue("title", title);

		{
			AxisDescriptor dd;
			dd.desc = 0;
			dd.size = 1;
			dd.labels[0] = 1;
			DumpAxis(dump, "x", dd); 
		}
		{
			AxisDescriptor dd;
			dd.desc = 0;
			dd.size = 1;
			dd.labels[0] = 1;
			DumpAxis(dump, "y", dd); 
		}

		dump.BeginElement("XDFAXIS");
			dump.AddAttribute("id", "z");
			dump.BeginElement("EMBEDDEDDATA");
				dump.AddAttribute("mmedaddress", "0");
				dump.AddAttribute("mmedelementsizebits", "8");
				dump.AddAttribute("mmedrowcount", (int)1);
				dump.AddAttribute("mmedcolcount", (int)1);
				dump.AddAttribute("mmedmajorstridebits", "0");
				dump.AddAttribute("mmedminorstridebits", "0");
			dump.EndElement();
			dump.AddElementValue("decimalpl", "2");
			dump.AddElementValue("min", "0.000000");
			dump.AddElementValue("max", "255.000000");
			dump.AddElementValue("outputtype", "1");
			dump.BeginElement("MATH");
				dump.AddAttribute("equation", "X");
				dump.BeginElement("VAR");
					dump.AddAttribute("id", "X");
				dump.EndElement();
			dump.EndElement();
		dump.EndElement();
	dump.EndElement();
}

//---------------------------------------------------------------------
void MapFinder::DumpTables(XMLDump& dump, MapType mt)
{
	for(uint n = 0; n < maps.size(); n++)
	{
		if(maps[n].type == mt)
			DumpTable(dump, maps[n]);
	}
}

//---------------------------------------------------------------------
int AxisDescriptor::GetLabelValue(uint index) const
{
	assert(index < size);
	if(address)
	{
		uint offset = TypeSizeof(dataType) * index;
		if(dataType == DT_UNKNOWN) return 0;
		if(dataType == DT_UBYTE)
			return *((ubyte*)address + offset);
		if(dataType == DT_BYTE)
			return *((byte*)address + offset);

		if(dataType == DT_USHORT)
		{
			ubyte* ptr = ((ubyte*)address + offset);
			return (ushort)(((ushort)ptr[0] << 8) | (ushort)ptr[1]);
		}
		if(dataType == DT_SHORT)
		{
			ubyte* ptr = ((ubyte*)address + offset);
			return (short)(((ushort)ptr[0] << 8) | (ushort)ptr[1]);
		}
		return 0;
	}
	else
	{
		return labels[index];
	}
}

//---------------------------------------------------------------------
int MapFinder::AddAxisDescriptor(const AxisDescriptor& desc)
{
	for(uint n = 0; n < axes.size(); n++)
	{
		if(axes[n] == desc)
			return n;
	}
	axes.push_back(desc);
	for(uint n = 0; n < descriptors.size(); n++)
	{
		if(descriptors[n].desc == desc.desc)
		{
			descriptors[n].count++;
			descriptors[n].min = std::min(descriptors[n].min, desc.GetLabelValue(0));
			descriptors[n].max = std::max(descriptors[n].max, desc.GetLabelValue(desc.size - 1));
			descriptors[n].minSize = std::min(descriptors[n].minSize, desc.size);
			descriptors[n].maxSize = std::max(descriptors[n].maxSize, desc.size);
			return axes.size() - 1;
		}
	}
	Descriptor newDesc;
	char tname[32];
	sprintf(tname, "%x", desc.desc);
	newDesc.name = tname;
	newDesc.count = 1;
	newDesc.desc = desc.desc;
	newDesc.min = desc.GetLabelValue(0);
	newDesc.max = desc.GetLabelValue(desc.size - 1);
	newDesc.minSize = desc.size;
	newDesc.maxSize = desc.size;
	descriptors.push_back(newDesc);
	return axes.size() - 1;
}

//---------------------------------------------------------------------
void MapFinder::AddMapInfo(uint addr, const char* name, MapType type, float scale, float offset)
{
	for(uint n = 0; n < maps.size(); n++)
	{
		if(maps[n].dataOffset == addr)
		{
			maps[n].name = name;
			maps[n].type = type;
			maps[n].scale = scale;
			maps[n].offset = offset;
			return;
		}
	}
}

//---------------------------------------------------------------------
void MapFinder::AddDescriptorInfo(uint desc, const char* name, DataType dataType, float scale, float offset)
{
	for(uint n = 0; n < descriptors.size(); n++)
	{
		if(descriptors[n].desc == desc)
		{
			descriptors[n].name = name;
			descriptors[n].dataType = dataType;
			descriptors[n].scale = scale;
			descriptors[n].offset = offset;
			return;
		}
	}
}

//---------------------------------------------------------------------
void MapFinder::DumpTables(XMLDump& dump)
{
	DumpSeparator(dump, "---------FUEL----------");
	DumpTables(dump, MT_FUEL);
	DumpSeparator(dump, "---------SPARK---------");
	DumpTables(dump, MT_SPARK);
	DumpSeparator(dump, "----------AIR----------");
	DumpTables(dump, MT_AIR);
	DumpSeparator(dump, "---------MISC----------");
	DumpTables(dump, MT_MISC);
	DumpSeparator(dump, "---------TEMP----------");
	DumpTables(dump, MT_TEMP_KOMP);
	DumpSeparator(dump, "---------TRANS----------");
	DumpTables(dump, MT_TRANSITION);
	DumpSeparator(dump, "---------RPM----------");
	DumpTables(dump, MT_RPM);
	DumpSeparator(dump, "--------UNKNOWN---------");
	DumpTables(dump, MT_UNKNOWN);
}

//---------------------------------------------------------------------
void DumpHeader(XMLDump& dump, uint startAddress, uint size, const char* desc)
{
	dump.BeginElement("XDFFORMAT");
	dump.AddAttribute("version", "1.50");
		dump.BeginElement("XDFHEADER");
			dump.AddElementValue("flags", "0x1");
			dump.AddElementValue("fileversion", "1.0");
			dump.AddElementValue("deftitle", "generated");
			dump.AddElementValue("description", "generated");
			dump.AddElementValue("author", "Fido generator");
			dump.AddElementValue("baseoffset", "0");
			dump.BeginElement("DEFAULTS");
				dump.AddAttribute("datasizeinbits", "8");
				dump.AddAttribute("sigdigits", "2");
				dump.AddAttribute("outputtype", "1");
				dump.AddAttribute("signed", "0");
				dump.AddAttribute("lsbfirst", "0");
				dump.AddAttribute("float", "0");
			dump.EndElement();
			dump.BeginElement("REGION");
			dump.AddAttribute("type", "0xFFFFFFFF");
				dump.AddAttribute("startaddress", "0x0");
				dump.AddAttribute("size", (int)size);
				dump.AddAttribute("regionflags", "0x0");
				dump.AddAttribute("name", "Binary File");
				dump.AddAttribute("desc", "This region describes the bin file edited by this XDF");
			dump.EndElement();
		dump.EndElement();
}

int cmpMap(const void* p1, const void* p2)
{
	const MapDescriptor* m1 = (const MapDescriptor*)p1;
	const MapDescriptor* m2 = (const MapDescriptor*)p2;

	return m1->dataOffset - m2->dataOffset;
}

bool CheckBin(const char* binName)
{
/*
	{
		AxisDescriptor xdesc;
		AxisDescriptor ydesc;
		xdesc.desc = 0;
		xdesc.size = 16;
		ydesc.desc = 0;
		ydesc.size = 16;
		for(int n = 0; n < 16; n++)
		{
			xdesc.labels[n] = n;
			ydesc.labels[n] = n;
		}
		int x = mapFinder.AddAxisDescriptor(xdesc);
		int y = mapFinder.AddAxisDescriptor(ydesc);
		mapFinder.Add3DMap(0x6d72, "MAF", DT_SHORT, MT_AIR, 1, 0, x, y);
	}


	qsort(&mapFinder.maps[0], mapFinder.maps.size(), sizeof(MapDescriptor), cmpMap);
	*/
	//d0 - rpm * 40
	//d4 - air temp * 0.75 - 48
	//d5 - load * 0.715
	//d7 - coolant * 0.75 - 48
	//d8 - volts * 0.068
	//advance * 0.75 - 22.5
/*
	for(int n = 0; n < mapFinder.descriptors.size(); n++)
	{
		const Descriptor& d = mapFinder.descriptors[n];
		if(d.count > 4 && d.desc != 0xd0 && d.desc != 0xd5)
		{
			float minValue = d.min * 0.75 - 48;
			float maxValue = d.max * 0.75 - 48;
			if(minValue < -20 && minValue > -50 &&
				maxValue > 70 && maxValue < 140)
			{
				n = n;
			}
		}
	}
*/
/*	
	mapFinder.AddDescriptorInfo(0xd0, "RPM", XT_ENGINE_SPEED, 40.0f, 0);

	mapFinder.AddMapInfo(0x7281, "Temp x Time comp", MT_FUEL, 1.0, 0);

	mapFinder.AutodetectMapType();
*/
/*
		//print out unused regions
	int curr = coff;
	int lastPos = coff;
	bool lastState = mapFinder.UsedOffset(curr);
	while(curr < SIZE)
	{
		bool newState = mapFinder.UsedOffset(curr);
		if(newState != lastState)
		{
			if(newState)
			{
				//just got 0->1 transition
				printf("%x - %x (%d)\n", lastPos, curr, curr - lastPos);
			}
			lastPos = curr;
			lastState = newState;
		}
		curr++;
	}
*/

	char xdfName[256];
	strcpy(xdfName, binName);
	if(strrchr(xdfName, '.'))
		strcpy(strrchr(xdfName, '.'), ".xdf");
	else
		strcat(xdfName, ".xdf");

	XMLDump dump(xdfName);
	DumpHeader(dump, 0, 0xffff, "pokus");


//		mapFinder.DumpTables(dump);
	dump.EndElement();
	dump.Close();


	return true;
}

void hovno();

int _tmain(int argc, _TCHAR* argv[])
{
	hovno();

	return 0;
}
