#pragma once

#include "disa.h"

namespace M6811
{
enum OpCode
{
	INVALID = 0,
	ABA = 1,
	ABX,
	ABY,
	
	ADCA,
	ADCB,
	ADDA,
	ADDB,
	ADDD,

	ANDA,
	ANDB,
	
	ASL,
	
	ASLA,
	ASLB,
	ASLD,
	
	ASR,
	
	ASRA,
	ASRB,
	
	BCC,
	
	BCLR,

	BCS,
	BEQ,
	BGE,
	BGT,
	BHI,
	BHS,

	BITA,
	BITB,
	
	BLE,
	BLO,
	BLS,
	BLT,
	BMI,
	BNE,
	BPL,
	BRA,
	
	BRCLR,

	BRN,

	BRSET,

	BSET,

	BSR,
	BVC,
	BVS,

	CBA,
	CLC,
	CLI,
	
	CLR,
	
	CLRA,
	CLRB,
	CLV,
	
	CMPA,
	CMPB,

	COM,
	COMA,
	COMB,
	
	CPD,
	CPX,
	CPY,
	
	DAA,
	
	DEC,
	DECA,
	DECB,

	DES,
	DEX,
	DEY,
	
	EORA,
	EORB,
	
	FDIV,
	IDIV,
	
	INC,
	INCA,
	INCB,

	INS,
	INX,
	INY,

	JMP,
	JSR,

	LDAA,
	LDAB,
	LDD,
	LDS,
	LDX,
	LDY,

	LSL,
	LSLA,
	LSLB,
	LSLD,
	
	LSR,
	LSRA,
	LSRB,
	LSRD,
	
	MUL,
	
	NEG,
	NEGA,
	NEGB,
	
	NOP,
	
	ORAA,
	ORAB,
	
	PSHA,
	PSHB,
	PSHX,
	PSHY,
	
	PULA,
	PULB,
	PULX,
	PULY,

	ROL,
	ROLA,
	ROLB,
	
	ROR,
	RORA,
	RORB,
	
	RTI,
	RTS,
	
	SBA  ,
	SBCA ,
	SBCB ,
	SEC  ,
	SEI  ,
	SEV  ,
	STAA ,
	STAB ,
	STD  ,
	STOP ,
	STS  ,
	STX  ,
	STY  ,
	SUBA ,
	SUBB ,
	SUBD ,
	SWI  ,
	TAB  ,
	TAP  ,
	TBA  ,
	TEST ,
	TPA  ,
	TST  ,
	TSTA ,
	TSTB ,
	TSX  ,
	TSY  ,
	TXS  ,
	TYS  ,
	WAI  ,
	XGDX ,
	XGDY ,
};

enum Mode
{
	NONE = 0,
	INH = 1,
	IMM,
	DIR,
	REL,
	EXT,
	IDX,
	IDY
};

enum Action
{
	A_READ	= 1,
	A_WRITE = 2,
	A_AREG  = 4,
	A_BREG  = 8,
	A_DREG  = 16,
	A_XREG  = 32,
	A_YREG  = 64,
	A_SREG  = 128,
	A_BRANCH= 256,
	A_SUB		= 512,
};

struct OpCodeDef: public Disa::OpCodeDef
{
	OpCode			code;
	const char*	name;
	Mode				mode;
	uint				act;
	ubyte				prefix;
	ubyte				inst;
	ubyte				length;
	ubyte				reg;
	const char*	desc;

	OpCodeDef(): code(INVALID), name(nullptr), mode(NONE), act(0), prefix(0), inst(0), length(0), reg(0), desc(nullptr)
	{
	}	
	OpCodeDef(OpCode c, const char* n, Mode m, ubyte p, ubyte i, ubyte l, const char* d): code(c), name(n), mode(m), act(0), prefix(p), inst(i), length(l), reg(0), desc(d)
	{	
	}

	uint InstLength() const
	{
		return (prefix ? 2 : 1);
	}

	uint OperLength() const
	{
		return length - InstLength();
	}

	uint OperOffset(uint offset) const
	{
		return offset + InstLength();
	}
};

struct VectorDef
{
	const char*	name;
	ushort			offset;
	VectorDef(): name(nullptr), offset(0)
	{
	}	
	VectorDef(const char* n, ushort o): name(n), offset(o)
	{
	}	
};

struct IOPortDef
{
	const char*	name;
	ushort			offset;
	IOPortDef(): name(nullptr), offset(0)
	{
	}	
	IOPortDef(const char* n, ushort o): name(n), offset(o)
	{
	}	
};

struct CodeLine
{
	uint offset;
	const OpCodeDef* op;
};


enum Family
{
	M68HC11F1,
};

class Context: public Disa::Context
{
private:
	void InitDefaults(Family f);

public:
	std::vector<CodeLine> code;

	Context(Family fam, uint len): Disa::Context(len)
	{
		InitDefaults(fam);
	}

	uint FindCode(uint from, const OpCode* patt1, uint size, uint maxDist = 0) const;
	std::vector<uint> FindCode(const OpCode* code, uint size, std::set<uint>* used = nullptr) const;

	void Finalize();
};

uint DecodeTarget(const ubyte* data, uint offset, const OpCodeDef& op);
inline uint DecodeTarget(const ubyte* data, const CodeLine& line)
{
	return DecodeTarget(data, line.offset, *line.op);
}

void DisassembleVectorFunctions(Context& ctx, const ubyte* buff, uint size);
bool Disassemble(Context& ctx, const ubyte* data, uint length, uint startOffset);
void Print(Context& ctx, const ubyte* data, std::vector<CodeLine>& code);
const OpCodeDef* FindOpCode(const ubyte* data, uint startOffset, uint length);
bool IsValid(const OpCodeDef& def, const ubyte* data, uint offset, uint length, uint& operOffset);
const VectorDef* GetVector(uint n);

class CodeIterator
{
	const ubyte* data;
	uint length;
	uint offset;


public:
	CodeIterator(const ubyte* d, uint len, uint off): data(d), length(len), offset(off)
	{
	}

	bool IsEnd() const
	{
		return offset >= length;
	}

	const OpCodeDef* Next(uint& off)
	{
		const OpCodeDef* op = FindOpCode(data, offset, length);
		if(op)
		{
			uint operOffset;
			if(!IsValid(*op, data, offset, length, operOffset))
				return nullptr;
			offset += op->length;
			off = offset;
		}
		return op;
	}
	uint Offset() const
	{
		return offset;
	}
};

}
