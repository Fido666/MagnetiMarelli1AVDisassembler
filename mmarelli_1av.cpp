#include "stdafx.h"
#include "decode.h"
#include "xmldump.h"
#include <assert.h>
#include "68hc11.h"


using namespace M6811;

//---------------------------------------------------------------------
struct LUTFn
{
	uint offset;
	DataType dtype;

	bool operator<(const LUTFn& other) const
	{
		return (offset < other.offset);
	}
	LUTFn(): offset(0), dtype(DT_UNKNOWN){}
	LUTFn(uint off): offset(off), dtype(DT_UNKNOWN) {}
	LUTFn(uint off, DataType dt): offset(off), dtype(dt) {}
};

std::vector<uint> FindFunction(M6811::Context& ctx, M6811::OpCode* code, uint size, const char* name, DataType dt, std::set<LUTFn>& fns)
{
	std::vector<uint> funcs = ctx.FindCode(code, size);

	for(uint n = 0; n < funcs.size(); n++)
	{
		char fnName[256];
		if(n == 0)
			strcpy(fnName, name);
		else
			sprintf(fnName, "%s_%d", name, (n+1));

		ctx.SetLocationName(ctx.code[funcs[n]].offset, fnName);
		fns.insert(LUTFn(ctx.code[funcs[n]].offset, dt));
	}
	return funcs;
}


#define ArrayCount(a) (sizeof(a) / sizeof(a[0]))

std::set<LUTFn> lutfns;

//---------------------------------------------------------------------
DataType LUTType(uint offset)
{
	auto it = lutfns.find(LUTFn(offset));
	if(it != lutfns.end())
		return (*it).dtype;

	return DT_UNKNOWN;
}

//---------------------------------------------------------------------
void FindLUTFunctions(M6811::Context& ctx)
{
	uint from = 0;

	//0xe778
	//0xe871
	M6811::OpCode LUT3DCode[] = {
		DES, DES, DES, DES, TSY, LDX, LDAA, CMPA, BLS, LDAA, STAA,
		LDAB, CMPB, BLS,LDAB, STAB,
		CPD, BNE, LDAA, JMP,
		CMPA,	BNE, DECB, ABX, LDD, STD, STD, BRA,
		CMPB, BNE, DECA, LDAB, INCB, MUL, ABX, STX, CLC, ADCA, STAA, LDX, LDAA, STAA, STAA, LDAB, INCB, ABX, LDAA, STAA, STAA, BRA,
		DECA, LDAB, INCB, MUL, ABX, STX, CLC, ADCA, STAA, LDX, LDAB, DECB, ABX, LDD, STD, LDAB, INCB, ABX, LDD, STD,
		LDAA, LDAB, SBA, BCS
	};
	FindFunction(ctx, LUT3DCode, ArrayCount(LUT3DCode), "LUT3D_8bitU", DT_UBYTE, lutfns);

	M6811::OpCode LUT3DCode2[] = {
		DES, DES, DES, DES, TSY, LDX, LDAA, CMPA, BLS, LDAA, STAA,
		LDAB, CMPB, BLS,LDAB, STAB,
		CPD, BNE, LDAA, JMP,
		CMPA,	BNE, DECB, ABX, LDD, STD, STD, BRA,
		CMPB, BNE, DECA, LDAB, INCB, MUL, ABX, STX, CLC, ADCA, STAA, LDX, LDAA, STAA, STAA, LDAB, INCB, ABX, LDAA, STAA, STAA, BRA,
		DECA, LDAB, INCB, MUL, ABX, STX, CLC, ADCA, STAA, LDX, LDAB, DECB, ABX, LDD, STD, LDAB, INCB, ABX, LDD, STD,
		LDAA, LDAB, SBA, BLT
	};
	FindFunction(ctx, LUT3DCode2, ArrayCount(LUT3DCode2), "LUT3D_8bitS", DT_BYTE, lutfns);

	//0xe5de
	M6811::OpCode LUT2DCode1[] = {
		DES, DES, TSY, LDX, LDAB, BEQ, CMPB, BLS, LDAB, STAB, BRA, LDD, CPD, BCC, INC, BRA};
	FindFunction(ctx, LUT2DCode1, ArrayCount(LUT2DCode1), "LUT2D_16Bit", DT_USHORT, lutfns);

	//0xe6c6
	M6811::OpCode LUT2DCode2[] = {DES, DES, DES, DES, TSY, LDX, LDAB, BNE, LDD, BRA, CMPB, BLS};
	FindFunction(ctx, LUT2DCode2, ArrayCount(LUT2DCode2), "LUT2D_16Bit2", DT_USHORT, lutfns);

	//0xe68b
	M6811::OpCode LUT2DCode3[] = {
		TSY, LDX, LDAB, BNE, LDAA, BRA, CMPB, BLS, LDAB, ABX, LDAA, BRA, DECB, ABX, LDAA, LDAB, SBA, BLT};
	FindFunction(ctx, LUT2DCode3, ArrayCount(LUT2DCode3), "LUT2D_8bitU", DT_UBYTE, lutfns);

	//0xe650
	M6811::OpCode LUT2DCode4[] = {
		TSY, LDX, LDAB, BNE, LDAA, BRA, CMPB, BLS, LDAB, ABX, LDAA, BRA, DECB, ABX, LDAA, LDAB, SBA, BCS};
	FindFunction(ctx, LUT2DCode4, ArrayCount(LUT2DCode4), "LUT2D_8bitS", DT_BYTE, lutfns);
}

void FuncName(Context& ctx, char* fnName, const ubyte* buff, const CodeLine& code, const char* defName)
{
	uint trget = DecodeTarget(buff, code);
	if(trget != -1)
	{
		Disa::Location* loc = ctx.FindLocation(trget);
		if(loc)
		{
			strcpy(fnName, loc->name.c_str());
			return;
		}
	}
	strcpy(fnName, defName);
}

void Add2DTable(Context& ctx, const ubyte* mapAddr, DataType dt, uint width)
{
//	char buff[256];
//			TableName("map2d", buff);
	//ctx.AddDataLocation(buff, mapAddr, Disa::DT_BYTE, width);
}

void hovno()
{
	const uint SIZE = 65536;
	ubyte buff[SIZE];
	
	FILE* f = fopen("Magneti_Marelli_1AV.BIN", "rb");
//	FILE* f = fopen("032906030AJ_MotorExcel.bin", "rb");

	fread(buff, SIZE, 1, f);
	fclose(f);

	M6811::Context ctx(M6811::M68HC11F1, SIZE);

	DisassembleVectorFunctions(ctx, buff, SIZE);

	{
		//e276
		M6811::OpCode jmpTable[] = {
		ASLB, LDX, ABX, LDD, XGDX, JMP
		};
		uint off = ctx.FindCode(0, jmpTable, ArrayCount(jmpTable));
		if(off != -1)
		{
			uint memOffset = ctx.code[off].offset;
			uint jmpTab = Disa::ReadUShortBE(buff, memOffset + 2);

			for(int j = 0; j < 13; j++)
			{
				uint joff = Disa::ReadUShortBE(buff, jmpTab);
				jmpTab += 2;
				M6811::Disassemble(ctx, buff, SIZE, joff);
			}
		}
	}

	ctx.Finalize();

#if 1
	FindLUTFunctions(ctx);

	MapFinder mapFinder(buff, SIZE);

	struct FoundMap
	{
		uint address;
		uint xinput, yinput, output; 
		int dims;
		uint width, height;
		FoundMap(): address(0), xinput(0), yinput(0), output(0),dims(0),width(0),height(0) {}
		FoundMap(uint addr, uint wid, uint inp, uint outp): address(addr), xinput(inp), yinput(0), output(outp),dims(1),width(wid),height(0) {}
		FoundMap(uint addr, uint wid, uint hei, uint xinp, uint yinp, uint outp): address(addr), xinput(xinp), yinput(yinp), output(outp),dims(2),width(wid),height(hei) {}

		bool operator<(const FoundMap& other) const
		{
			if(address < other.address) return true;
			if(address > other.address) return false;
			if(xinput < other.xinput) return true;
			if(xinput > other.xinput) return false;
			if(yinput < other.yinput) return true;
			if(yinput > other.yinput) return false;
			if(output < other.output) return true;
			if(output > other.output) return false;
			return false;
		}
	};

	std::set<FoundMap> found;
	//std::vector<FoundMap> found;
	std::set<uint> used;

	char fnName[256];

	uint rpmAddr = 0;
	///////////////////////
	{
		static const M6811::OpCode limiterPatt[] = {
			LDD, CPD, BCS, LDD, BRA,
			SUBD, BCC, LDD, BRA,
			ASLD, ASLD, ASLD
		};
	
		std::vector<uint> code = ctx.FindCode(limiterPatt, ArrayCount(limiterPatt), &used);
		if(!code.empty())
		{
			const CodeLine* lines = &ctx.code[code[0]];
			rpmAddr = DecodeTarget(buff, lines[0]);
			uint rpmLim1 = DecodeTarget(buff, lines[1]);
			uint rpmLim2 = DecodeTarget(buff, lines[5]);
			ctx.AddProcedure("RPMLimiter", lines->offset);
			ctx.AddDataLocation("RPM", rpmAddr, DT_USHORT, 1);
			ctx.AddDataLocation("RPMLim1", rpmLim1, DT_USHORT, 1);
			ctx.AddDataLocation("RPMLim2", rpmLim2, DT_USHORT, 1);
		}
	}
	///////////////////////
	{
		static const M6811::OpCode historyRPMPatt[] = {
			LDD, STD,
			LDD, STD,
			LDD, STD,
			LDD, STD
		};
	
		std::vector<uint> code = ctx.FindCode(historyRPMPatt, ArrayCount(historyRPMPatt), &used);
		for(uint n = 0; n < code.size(); n++)
		{
			const CodeLine* lines = &ctx.code[code[n]];
			if(DecodeTarget(buff, lines[6]) == rpmAddr)
			{
				ctx.AddDataLocation("RPMHist[1]", DecodeTarget(buff, lines[7]), DT_USHORT, 1);
				ctx.AddDataLocation("RPMHist[2]", DecodeTarget(buff, lines[5]), DT_USHORT, 1);
				ctx.AddDataLocation("RPMHist[3]", DecodeTarget(buff, lines[3]), DT_USHORT, 1);
				ctx.AddDataLocation("RPMHist[4]", DecodeTarget(buff, lines[1]), DT_USHORT, 1);
			}
		}
	}
	///////////////////////
	{
		static const M6811::OpCode middleRPMPatt[] = {
			LDD, CPD, //load??? loadMiddle
			BCC, LDD,
			CPD, BCC,
			BSR, BRA,
			JSR, RTS
		};
	
		std::vector<uint> code = ctx.FindCode(middleRPMPatt, ArrayCount(middleRPMPatt), &used);
		for(uint n = 0; n < code.size(); n++)
		{
			const CodeLine* lines = &ctx.code[code[n]];
			if(DecodeTarget(buff, lines[3]) == rpmAddr)
			{
				ctx.AddDataLocation("RPM3500", DecodeTarget(buff, lines[4]), DT_USHORT, 1);
			}
		}
	}

	///////////////////////
	{
		static const M6811::OpCode patt2[] = {
			LDX, PSHX,
			LDD, PSHB, PSHA,
			LDAA, PSHA,
			LDAA, PSHA,
			JSR,
			PULA, STAA};
	
		std::vector<uint> code = ctx.FindCode(patt2, ArrayCount(patt2), &used);
		for(uint n = 0; n < code.size(); n++)
		{
			const CodeLine* lines = &ctx.code[code[n]];
			uint mapAddr = DecodeTarget(buff, lines[0]);
			uint parm1 = DecodeTarget(buff, lines[2]);
			//uint parm2 = DecodeTarget(buff, lines[5]);
			uint width = DecodeTarget(buff, lines[7]) + 1;
			uint target = DecodeTarget(buff, lines[11]) - 1;
			FuncName(ctx, fnName, buff, lines[9], "lut2d");

			printf("%04x %x = %s(%x %x) ;%d\n", lines->offset, target, fnName, mapAddr, parm1, width);
			found.insert(FoundMap(mapAddr, width, parm1, target));
			ctx.AddDataLocation(mapAddr, 
//			Add2DTable(ctx, mapAddr, width);
		}	
	}

	{
		static const M6811::OpCode patt3[] = {
			LDX, PSHX,
			LDAA, CLRB, PSHB, PSHA,
			LDAA, PSHA,
			LDAA, PSHA,
			JSR,
			PULA, STAA};
		std::vector<uint> code = ctx.FindCode(patt3, ArrayCount(patt3), &used);
		for(uint n = 0; n < code.size(); n++)
		{
			const CodeLine* lines = &ctx.code[code[n]];

			uint mapAddr = DecodeTarget(buff, lines[0]);
			uint parm1 = DecodeTarget(buff, lines[2]);
			uint parm2 = DecodeTarget(buff, lines[6]);
			uint width = DecodeTarget(buff, lines[8]) + 1;
			uint target = DecodeTarget(buff, lines[12]) - 1;
			FuncName(ctx, fnName, buff, lines[10], "lut2d");

			printf("%04x %x = %s(%x %x %x) ;%d\n", lines->offset, target, fnName, mapAddr, parm1, parm2, width);
			found.insert(FoundMap(mapAddr, width, parm1, target));
		}	
	}

	///////////////////////
	{
		static const M6811::OpCode patt4[] = {
			LDX, PSHX,
			LDAA, PSHA,
			LDAA, PSHA,
			LDAA, PSHA,
			JSR,
			PULX, STX};
	
		std::vector<uint> code = ctx.FindCode(patt4, ArrayCount(patt4), &used);
		for(uint n = 0; n < code.size(); n++)
		{
			const CodeLine* lines = &ctx.code[code[n]];
			uint mapAddr = DecodeTarget(buff, lines[0]);
			uint parm1 = DecodeTarget(buff, lines[2]);
			uint width = DecodeTarget(buff, lines[6]) + 1;
			uint target = DecodeTarget(buff, lines[10]) - 1;
			FuncName(ctx, fnName, buff, lines[8], "lut2d");

			printf("%04x %x = %s(%x %x) ;%d\n", lines->offset, target, fnName, mapAddr, parm1, width);
			found.insert(FoundMap(mapAddr, width, parm1, target));
		}	
	}

	///////////////////////
	{
		static const M6811::OpCode patt5[] = {
			LDX, PSHX,
			LDAA, PSHA,
			LDAA, PSHA,
			LDAA, PSHA,
			JSR,
			PULA, STAA,
			PULA, STAA
		};
	
		std::vector<uint> code = ctx.FindCode(patt5, ArrayCount(patt5), &used);
		for(uint n = 0; n < code.size(); n++)
		{
			const CodeLine* lines = &ctx.code[code[n]];
			uint mapAddr = DecodeTarget(buff, lines[0]);
			uint parm1 = DecodeTarget(buff, lines[2]);
			uint width = DecodeTarget(buff, lines[6]) + 1;
			uint target = DecodeTarget(buff, lines[10]) - 1;
			FuncName(ctx, fnName, buff, lines[8], "lut2d");

			printf("%04x %x = %s(%x %x) ;%d\n", lines->offset, target, fnName, mapAddr, parm1, width);
			found.insert(FoundMap(mapAddr, width, parm1, target));
		}	
	}

	///////////////////////
	{
		static const M6811::OpCode patt6[] = {
			LDX, PSHX,
			LDAA, PSHA,
			LDAA, PSHA,
			LDAA, PSHA,
			JSR,
			PULA, STAA
		};
	
		std::vector<uint> code = ctx.FindCode(patt6, ArrayCount(patt6), &used);
		for(uint n = 0; n < code.size(); n++)
		{
			const CodeLine* lines = &ctx.code[code[n]];
			uint mapAddr = DecodeTarget(buff, lines[0]);
			uint parm1 = DecodeTarget(buff, lines[2]);
			uint width = DecodeTarget(buff, lines[6]) + 1;
			uint target = DecodeTarget(buff, lines[10]);
			FuncName(ctx, fnName, buff, lines[8], "lut2d");

			printf("%04x %x = %s(%x %x) ;%d\n", lines->offset, target, fnName, mapAddr, parm1, width);
			found.insert(FoundMap(mapAddr, width, parm1, target));
		}	
	}

	{
		static const M6811::OpCode patt1[] = {
			LDX, PSHX, //map addr 
			LDAA, PSHA, //x param.h
			LDAA, PSHA, //x param.l
			LDAA, PSHA, //width - 1
			LDAA, PSHA,	//y param.h
			LDAA, PSHA,	//y param.l
			LDAA, PSHA, //height - 1
			JSR, PULA		//LUT3D1
		};

		std::vector<uint> code = ctx.FindCode(patt1, ArrayCount(patt1), &used);
		for(uint n = 0; n < code.size(); n++)
		{
			const CodeLine* lines = &ctx.code[code[n]];

			uint mapAddr = DecodeTarget(buff, lines[0]);
			uint xparm = DecodeTarget(buff, lines[2]);
			uint width = DecodeTarget(buff, lines[6]) + 1;
			uint yparm = DecodeTarget(buff, lines[8]);
			uint height = DecodeTarget(buff, lines[12]) + 1;
			uint target = -1;
			FuncName(ctx, fnName, buff, lines[14], "lut3d");

			//try to search for nearest STAA
			uint decoded = 0;
			for(uint i = 16; decoded < 15; decoded++)
			{
				//resolve "else" branch
				if(lines[i].op->code == M6811::BRA)
				{
					uint braTarget = DecodeTarget(buff, lines[i]);
					while(lines[i].offset < braTarget)
						i++;
					continue;
				}

				if(lines[i].op->code == M6811::STAA)
				{
					target = DecodeTarget(buff, lines[i]);
					break;
				}
				i++;
			}

			bool done = false;
			if(target == -1)
			{
				//search for result+something
				static const M6811::OpCode st[] = {
					LDAB, LDX, ABX, STAA 
					};
				uint stcode = ctx.FindCode(code[n] + 16, st, ArrayCount(st), 12);
				if(stcode != -1)
				{
					uint otherAddr = DecodeTarget(buff, ctx.code[stcode + 0]);
					target = DecodeTarget(buff, ctx.code[stcode + 1]);
					if(otherAddr != -1 && target != -1)
					{
						printf("%04x %x = [%x] + %s(%x, %x, %x) ;%d x %d\n",lines->offset, target, otherAddr, fnName, mapAddr, xparm, yparm, width, height);
						done = true;
					}
				}
			}
			if(!done)
				printf("%04x %x = %s(%x, %x, %x) ;%d x %d\n",lines->offset, target, fnName, mapAddr, xparm, yparm, width, height);
			found.insert(FoundMap(mapAddr, width, height, target, xparm, yparm));
		}
	}


#endif

	for(auto it = found.begin(); it != found.end(); ++it)
	{
		FoundMap f = (*it);
		uint address = f.address;
	}

	const FoundMap* ptr = &(*found.begin());
	M6811::Print(ctx, buff, ctx.code);
}

