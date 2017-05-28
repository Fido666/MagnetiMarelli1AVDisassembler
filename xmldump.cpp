#include "stdafx.h"
#include "xmldump.h"

//---------------------------------------------------------------------
XMLDump::XMLDump(const char* name)
{
	depth = 0;
	file = fopen(name, "wt");
}

//---------------------------------------------------------------------
void XMLDump::Close()
{
	if(file)
		fclose(file);
	file = nullptr;
}

//---------------------------------------------------------------------
void XMLDump::HandleParentElement()
{
	if(depth > 0)
	{
		if(stack[depth - 1].count == 0)
			fputs(">\n", file);
		stack[depth - 1].count++;
	}
}

//---------------------------------------------------------------------
void XMLDump::HandleIdent()
{
	for(int n = 0; n < depth; n++)
		fputc(' ', file);
}

//---------------------------------------------------------------------
void XMLDump::BeginElement(const char* elem)
{
	HandleParentElement();
	HandleIdent();
	for(int n = 0; n < depth; n++)
		fputc(' ', file);

	fprintf(file, "<%s", elem);
	stack[depth].name = elem;
	stack[depth++].count = 0;
}

//---------------------------------------------------------------------
void XMLDump::EndElement()
{
	--depth;
	if(stack[depth].count == 0)
	{
		fputs(" />\n", file);
	}
	else
	{
		HandleIdent();
		fprintf(file, "</%s>\n", stack[depth].name);
	}
}

//---------------------------------------------------------------------
void XMLDump::AddElementValue(const char* name, const char* value)
{
	BeginElement(name);
	AddValue(value);
	EndElement();
}
//---------------------------------------------------------------------
void XMLDump::AddElementValue(const char* name, int value)
{
	BeginElement(name);
	char temp[32];
	sprintf(temp, "%d", value);
	AddValue(temp);
	EndElement();
}
//---------------------------------------------------------------------
void XMLDump::AddElementValue(const char* name, float value)
{
	BeginElement(name);
	char buff[64];
	Float2Str(buff, value);
	AddValue(buff);
	EndElement();
}

//---------------------------------------------------------------------
void XMLDump::AddValue(const char* value)
{
	if(stack[depth - 1].count == 0)
	{
		stack[depth - 1].count++;
		fputc('>', file);
	}
	fputs(value, file);
}


//---------------------------------------------------------------------
void Float2Str(char* buff, float v)
{
	sprintf(buff, "%f", v);
	int l = strlen(buff);
	for(int n = l - 1; n >= 0; n--)
	{
		if(buff[n] != '0')
			break;
		if(n > 0 && buff[n - 1] != '.')
			buff[n] = 0;
	}
}

