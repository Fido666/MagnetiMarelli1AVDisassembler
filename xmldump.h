#pragma once
#include "types.h"

class XMLDump
{
	struct stackitem
	{
		const char* name;
		int count;
	};

	stackitem stack[8];
	int depth;
	FILE* file;

	void HandleParentElement();
	void HandleIdent();

public:
	XMLDump(const char* name);
	~XMLDump()
	{
		Close();
	}
	void Close();
	void BeginElement(const char* elem);
	void EndElement();

	void AddAttribute(const char* name)
	{
		fprintf(file, " %s=", name);
	}
	void AddAttrValue(const char* value)
	{
		fprintf(file, "\"%s\"", value);
	}
	void AddAttrValue(int value)
	{
		fprintf(file, "\"%d\"", value);
	}
	void AddAttribute(const char* name, const char* value)
	{
		AddAttribute(name);
		AddAttrValue(value);
	}
	void AddAttribute(const char* name, float value)
	{
		AddAttribute(name);
		char buff[64];
		Float2Str(buff, value);
		AddAttrValue(buff);
	}
	void AddAttribute(const char* name, int value)
	{
		AddAttribute(name);
		AddAttrValue(value);
	}
	void AddValue(const char* value);
	void AddElementValue(const char* name, const char* value);
	void AddElementValue(const char* name, int value);
	void AddElementValue(const char* name, float value);

};
