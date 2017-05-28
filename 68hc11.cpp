#include "stdafx.h"
#include "68hc11.h"
#include "utils.h"
#include <set>
#include <algorithm>

//#define PRINT_REFS


namespace M6811
{

//---------------------------------------------------------------------
	bool IsValid(const OpCodeDef& def, const ubyte* data, uint offset, uint length, uint& operOffset)
	{
		//do it fit?
		if((length - offset) < def.length)
			return false;

		if(def.prefix)
		{
			//prefix match?
			if(data[offset] != def.prefix)
				return false;
			offset++;
		}
		//instruction match?
		if(data[offset] != def.inst)
			return false;

		offset++;
		operOffset = offset;

		if(def.mode == REL)
		{
			//check out of bound relative offset
			char relOffset = (char)data[offset];
			offset++;
			int absOffset = (int)offset + relOffset;
			if(absOffset < 0 || absOffset >= (int)length)
				return false;
		}

		return true;
	}

	void DecodeIMM8(const ubyte* data, uint offset, ubyte& ret)
	{
		ret = data[offset];
	}
	void DecodeIMM16(const ubyte* data, uint offset, ushort& ret)
	{
		ret = Disa::ReadUShortBE(data, offset);
	}

	void DecodeDIR(const ubyte* data, uint offset, ubyte& dd)
	{
		dd = data[offset];
	}

	void DecodeDIR(const ubyte* data, uint offset, ubyte& dd, ubyte& mm)
	{
		dd = data[offset];
		mm = data[offset + 1];
	}

	void DecodeDIR(const ubyte* data, uint offset, ubyte& dd, ubyte& mm, ubyte& rr)
	{
		dd = data[offset];
		mm = data[offset + 1];
		rr = data[offset + 2];
	}

//---------------------------------------------------------------------
	uint DecodeTarget(const ubyte* data, uint offset, const OpCodeDef& op)
	{
		uint operOffset = op.OperOffset(offset);
		switch(op.mode)
		{
		case IMM:
			if(op.OperLength() == 2)
				return Disa::ReadUShortBE(data, operOffset);
			else
				return data[operOffset];

		case EXT:
			return Disa::ReadUShortBE(data, operOffset);

		case DIR:
			return (byte)data[offset + op.length - 1] + offset + op.length;;
		
		case REL:
			return (byte)data[operOffset] + offset + op.length;

		case IDX:
		case IDY:
			if(op.OperLength() == 3)
			{
				return data[offset + op.length - 1] + offset + op.length;
			}
		}
		return -1;
	}

	void DecodeREL(const ubyte* data, uint offset, byte& rel)
	{
		rel = data[offset];
	}


	struct CodeCompare
	{
		bool operator()(const CodeLine& a, const CodeLine& b) const
		{
			return a.offset < b.offset;
		}
	};

//---------------------------------------------------------------------
	void LocationName(const Disa::Location* loc, char* buff)
	{
		if(loc->type == Disa::LT_SUB)
		{
			if(loc->name.empty())
				sprintf(buff, "sub_%04x", loc->offset);
			else
				sprintf(buff, "%s", loc->name.c_str());
		}
		else if(loc->type == Disa::LT_LABEL)
		{
			sprintf(buff, "loc_%04x", loc->offset);
		}
		else if(loc->type == Disa::LT_DATA)
		{
			if(loc->name.empty())
				sprintf(buff, "byte_%04x", loc->offset);
			else
				sprintf(buff, "%s", loc->name.c_str());
		}
	}


//---------------------------------------------------------------------
	void PrintLine(uint offset, const char* data)
	{
		Disa::Dump line;


		line.Begin(offset);
		line.AddString(data);
		line.End();
//		printf("%04x %s\n", offset, data);
	}

//---------------------------------------------------------------------
	void PrintLine(uint offset, const char* inst, const char* param)
	{
		char lineBuff[256];
		int lineOffset = 0;

		sprintf(lineBuff, "%04x ", offset);
		lineOffset += 5;

		memset(lineBuff + lineOffset, ' ', 13 - lineOffset);
		lineOffset = 13;

		int ll = Utils::lowercpy(lineBuff + lineOffset, inst);
		lineOffset += ll;

		memset(lineBuff + lineOffset, ' ', 21 - lineOffset);
		lineOffset = 21;

		ll = strlen(param);
		memcpy(lineBuff + lineOffset, param, ll);
		lineOffset += ll;
		lineBuff[lineOffset] = 0;

		printf(lineBuff);
		printf("\n");
	}

//---------------------------------------------------------------------
	void PrintRef(uint offset, const Disa::Reference& ref, const Disa::Location* rloc)
	{
		char lineBuff[256];
		int lineOffset = 0;

		sprintf(lineBuff, "%04x ", offset);
		lineOffset += 5;

		memset(lineBuff + lineOffset, ' ', 34 - lineOffset);
		lineOffset = 34;

		//; DATA	XREF: sub_DA35+5r
//;	CODE XREF: sub_604C+13j
		if(ref.type == Disa::RT_CALL || ref.type == Disa::RT_JUMP)
		{
			if(rloc)
			{
				char locName[256];
				LocationName(rloc, locName);
				sprintf(lineBuff + lineOffset, "; CODE XREF: %s:0x%04x\n", locName, ref.offset);
			}
			else
				sprintf(lineBuff + lineOffset, "; CODE XREF: 0x%04x\n", ref.offset);
			printf(lineBuff);
		}
	}

//---------------------------------------------------------------------
	void ImmToString(uint value, char* buff)
	{
		if(value < 10)
			sprintf(buff, "%d", value);
		else
			sprintf(buff, "$%02x", value);
	}

//---------------------------------------------------------------------
	void Print(Context& ctx, const ubyte* data, std::vector<CodeLine>& code)
	{
		Disa::Dump dline;

		for(auto iter = code.begin(); iter != code.end(); ++iter)
		{
			const CodeLine& line = *iter;

			Disa::Location* loc = ctx.FindLocation(line.offset);

			dline.SetOffset(line.offset);

			char buff[256];
			if(loc)
			{
				if(loc->type == Disa::LT_SUB)
				{
//					dline.SubMarker();
					PrintLine(line.offset, ";	===============	S U B R	O U T I	N E =======================================");
					PrintLine(line.offset, "");
					PrintLine(line.offset, "");
				}
				else
//					dline.LabelMarker();
					PrintLine(line.offset, ";	---------------------------------------------------------------------------");

				LocationName(loc, buff);
				strcat(buff, ":");
				PrintLine(line.offset, buff);
#ifdef PRINT_REFS
				for(auto ref = loc->references.begin(); ref != loc->references.end(); ++ref)
				{
					Disa::Location* rloc = ctx.FindParentLocation((*ref).offset);
					PrintRef(line.offset, *ref, rloc);
				}
#endif
			}

			char params[256] = "";

			if(line.offset == 0x45b7)
				data=data;

			uint oper;
			switch(line.op->mode)
			{
			case INH:
				break;

			case DIR:
				{
					uint operOffset = line.offset + line.op->InstLength();

					//dd
					Disa::Location* tloc = ctx.FindLocation(data[operOffset]);
					if(tloc)
					{
						LocationName(tloc, params);
					}
					else
					{
						sprintf(params, "$%02x", (uint)data[operOffset]);
					}

					//mm?
					if(line.op->OperLength() > 1)
					{
						strcat(params, " ");
						operOffset++;
						ImmToString(data[operOffset], params + strlen(params));
					}

					//rr?
					if(line.op->OperLength() > 2)
					{
						strcat(params, " ");
						operOffset++;
						uint oper = DecodeTarget(data, line.offset, *line.op);
						tloc = ctx.FindLocation(oper);

						if(tloc)
						{
							LocationName(tloc, params + strlen(params));
						}
						else
						{
							sprintf(params + strlen(params), "$%04x", oper);
						}
					}
				}
				break;

			case REL:
			case EXT:
				{
					oper = DecodeTarget(data, line.offset, *line.op);
					Disa::Location* tloc = ctx.FindLocation(oper);
					if(tloc)
					{
						LocationName(tloc, params);
					}
					else
					{
						sprintf(params, "$%04x", oper);
					}
				}
				break;

			case IMM:
				if(line.op->OperLength() == 2)
				{
					uint operOffset = line.op->OperOffset(line.offset);
					oper = Disa::ReadUShortBE(data, operOffset);
				}
				else
				{
					oper = data[line.op->OperOffset(line.offset)];
				}
				params[0] = '#';
				ImmToString(oper, params+1);
				break;

			case IDX:
			case IDY:
				{
					uint operOffset = line.offset + line.op->InstLength();

					//ff
					ImmToString(data[operOffset], params);

					if(line.op->mode == IDX)
						strcat(params, ",x");
					else
						strcat(params, ",y");

					//mm?
					if(line.op->OperLength() > 1)
					{
						strcat(params, " ");
						operOffset++;
						ImmToString(data[operOffset], params + strlen(params));
					}

					//rr?
					if(line.op->OperLength() > 2)
					{
						strcat(params, " ");
						operOffset++;
						uint oper = DecodeTarget(data, line.offset, *line.op);
						Disa::Location* tloc = ctx.FindLocation(oper);

						if(tloc)
						{
							LocationName(tloc, params + strlen(params));
						}
						else
						{
							sprintf(params + strlen(params), "$%04x", oper);
						}
					}
				}
				break;
			};

			PrintLine(line.offset, line.op->name, params);
		}
	}

//---------------------------------------------------------------------
	bool Disassemble(Context& ctx, const ubyte* data, uint length, uint startOffset)
	{
		uint offset = startOffset;
		Disa::TLocationSet stack;
		Disa::TLocationSet labels;
		std::set<CodeLine, CodeCompare> code;
		bool valid = true;

		while(true)
		{
			CodeIterator iter(data, length, offset);

			while(!iter.IsEnd())
			{
				uint offset = iter.Offset();

				if(offset == 0x573e)
					offset=offset;
				if(offset == 0x4516)
					offset=offset;

				if(ctx.Usage(offset) == Disa::DU_CODE)
					break;

				if(ctx.Usage(offset) != Disa::DU_EMPTY)
				{
					valid = false;
					break;
				}
				uint currOffset = offset;
				const OpCodeDef* op = iter.Next(offset);
				if(!op)
				{
					valid = false;
					break;
				}

				ctx.Use(currOffset, Disa::DU_CODE, op->length);

				CodeLine line;
				line.offset = currOffset;
				line.op = op;
				code.insert(line);

				if(op->mode == REL)
				{
					//only branch instructions here
					uint target = DecodeTarget(data, currOffset, *op);

					Disa::Location* loc = ctx.HandleTarget(op->code == BSR ? Disa::LT_SUB : Disa::LT_LABEL, target, currOffset);

					if(loc)
						labels.insert(loc);

					if(op->code == BRA)
					{
						break;
					}
				}
			
				if(op->code == JMP)
				{
					if(op->mode == EXT)
					{
						uint target = DecodeTarget(data, currOffset, *op);
						Disa::Location* loc = ctx.HandleTarget(Disa::LT_LABEL, target, currOffset);

						if(loc)
							labels.insert(loc);
					}
					break;
				}

				if(op->code == BRCLR || op->code == BRSET)
				{
					uint target = DecodeTarget(data, currOffset, *op);
					Disa::Location* loc = ctx.HandleTarget(Disa::LT_LABEL, target, currOffset);

					if(loc)
						stack.insert(loc);
				}

				if(op->code == JSR)
				{
					uint target = DecodeTarget(data, currOffset, *op);
					Disa::Location* loc = ctx.HandleTarget(Disa::LT_SUB, target, currOffset);

					if(loc)
						stack.insert(loc);
				}

				if(op->code == RTS || op->code == RTI)
				{
					break;
				}
			}
			if(labels.empty())
				break;

			const auto last = labels.rbegin(); 
			offset = (*last)->offset;
			labels.erase(--labels.end());
		}

		if(valid)
		{
			ctx.code.insert(ctx.code.end(), code.begin(), code.end());

			for(auto i = stack.begin(); i != stack.end(); ++i)
			{
				if(!Disassemble(ctx, data, length, (*i)->offset))
					return false;
			}
			return true;
		}
		return false;
	}

//---------------------------------------------------------------------
	uint Context::FindCode(uint from, const OpCode* patt1, uint size, uint maxDist) const
	{
		uint count = code.size() - size;
		for(uint n = from; n < count; n++)
		{
			if(maxDist && (n - from) > maxDist)
				break;

			bool found = true;
			for(uint c = 0; c < size; c++)
			{
				if(code[n + c].op->code != patt1[c])
				{
					found = false;
					break;
				}
			}
			if(found)
			{
				return n;
			}
		}
		return -1;
	}

	struct SortCode
	{
		bool operator()(const CodeLine& a, const CodeLine& b) const
		{
			return a.offset < b.offset;
		}
	};

	void Context::Finalize()
	{
		std::sort(code.begin(), code.end(), SortCode());
	}

//---------------------------------------------------------------------
std::vector<uint> Context::FindCode(const OpCode* code, uint size, std::set<uint>* used) const
{
	std::vector<uint> offsets;

	uint var = 1;
	uint from = 0;

	while(true)
	{
		uint off = FindCode(from, code, size);
		if(off == -1)
			break;
		from = off + 1;

		if(used)
		{
			if(used->find(off) != used->end())
					continue;
			used->insert(off);
		}
		offsets.push_back(off);
	}
	return offsets;
}


//---------------------------------------------------------------------
void DisassembleVectorFunctions(Context& ctx, const ubyte* buff, uint size)
{
	for(uint n = 0;; n++)
	{
		const VectorDef* vec = GetVector(n);
		if(!vec)
			break;
		
		uint offset = Disa::ReadUShortBE(buff, vec->offset);

		if(ctx.MemType(offset) == Disa::MT_ROM)
		{
			char name[256] = "__";
			strcat(name, vec->name);

			ctx.AddProcedure(name, offset);
			M6811::Disassemble(ctx, buff, 65536, offset);
		}
	}
}
}







