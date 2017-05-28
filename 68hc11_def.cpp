#include "stdafx.h"
#include "68hc11.h"

namespace M6811
{
#define OPCODE_LIST(XX) \
XX(ABA  , INH, 0x00, 0x1b, 1, "a = a + b") \
XX(ABX  , INH, 0x00, 0x3a, 1, "x = b + x") \
XX(ABY  , INH, 0x18, 0x3a, 2, "y = b + y") \
XX(ADCA , IMM, 0x00, 0x89, 2, "a = a + %s + c") \
XX(ADCA , DIR, 0x00, 0x99, 2, "a = a + %s + c") \
XX(ADCA , EXT, 0x00, 0xb9, 3, "a = a + %s + c") \
XX(ADCA , IDX, 0x00, 0xa9, 2, "a = a + %s + c") \
XX(ADCA , IDY, 0x18, 0xa9, 3, "a = a + %s + c") \
XX(ADCB , IMM, 0x00, 0xc9, 2, "b = b + %s + c") \
XX(ADCB , DIR, 0x00, 0xd9, 2, "b = b + %s + c") \
XX(ADCB , EXT, 0x00, 0xf9, 3, "b = b + %s + c") \
XX(ADCB , IDX, 0x00, 0xe9, 2, "b = b + %s + c") \
XX(ADCB , IDY, 0x18, 0xe9, 3, "b = b + %s + c") \
XX(ADDA , IMM, 0x00, 0x8b, 2, "a = a + %s") \
XX(ADDA , DIR, 0x00, 0x9b, 2, "a = a + %s") \
XX(ADDA , EXT, 0x00, 0xbb, 3, "a = a + %s") \
XX(ADDA , IDX, 0x00, 0xab, 2, "a = a + %s") \
XX(ADDA , IDY, 0x18, 0xab, 3, "a = a + %s") \
XX(ADDB , IMM, 0x00, 0xcb, 2, "b = b + %s") \
XX(ADDB , DIR, 0x00, 0xdb, 2, "b = b + %s") \
XX(ADDB , EXT, 0x00, 0xfb, 3, "b = b + %s") \
XX(ADDB , IDX, 0x00, 0xeb, 2, "b = b + %s") \
XX(ADDB , IDY, 0x18, 0xeb, 3, "b = b + %s") \
XX(ADDD , IMM, 0x00, 0xc3, 3, "d = d + %s") \
XX(ADDD , DIR, 0x00, 0xd3, 2, "d = d + %s") \
XX(ADDD , EXT, 0x00, 0xf3, 3, "d = d + %s") \
XX(ADDD , IDX, 0x00, 0xe3, 2, "d = d + %s") \
XX(ADDD , IDY, 0x18, 0xe3, 3, "d = d + %s") \
XX(ANDA , IMM, 0x00, 0x84, 2, "a = a & %s") \
XX(ANDA , DIR, 0x00, 0x94, 2, "a = a & %s") \
XX(ANDA , EXT, 0x00, 0xb4, 3, "a = a & %s") \
XX(ANDA , IDX, 0x00, 0xa4, 2, "a = a & %s") \
XX(ANDA , IDY, 0x18, 0xa4, 3, "a = a & %s") \
XX(ANDB , IMM, 0x00, 0xc4, 2, "b = b & %s") \
XX(ANDB , DIR, 0x00, 0xd4, 2, "b = b & %s") \
XX(ANDB , EXT, 0x00, 0xf4, 3, "b = b & %s") \
XX(ANDB , IDX, 0x00, 0xe4, 2, "b = b & %s") \
XX(ANDB , IDY, 0x18, 0xe4, 3, "b = b & %s") \
XX(ASL  , EXT, 0x00, 0x78, 3, "%s <<= 1") \
XX(ASL  , IDX, 0x00, 0x68, 2, "%s <<= 1") \
XX(ASL  , IDY, 0x18, 0x68, 3, "%s <<= 1") \
XX(ASLA , INH, 0x00, 0x48, 1, "a <<= 1") \
XX(ASLB , INH, 0x00, 0x58, 1, "b <<= 1") \
XX(ASLD , INH, 0x00, 0x05, 1, "d <<= 1") \
XX(ASR  , EXT, 0x00, 0x77, 3, "%s >>= 1") \
XX(ASR  , IDX, 0x00, 0x67, 2, "%s >>= 1") \
XX(ASR  , IDY, 0x18, 0x67, 3, "%s >>= 1") \
XX(ASRA , INH, 0x00, 0x47, 1, "a >>= 1") \
XX(ASRB , INH, 0x00, 0x57, 1, "a >>= 1") \
XX(BCC  , REL, 0x00, 0x24, 2, "") \
XX(BCLR , DIR, 0x00, 0x15, 3, "") \
XX(BCLR , IDX, 0x00, 0x1d, 3, "") \
XX(BCLR , IDY, 0x18, 0x1d, 4, "") \
XX(BCS  , REL, 0x00, 0x25, 2, "") \
XX(BEQ  , REL, 0x00, 0x27, 2, "") \
XX(BGE  , REL, 0x00, 0x2c, 2, "") \
XX(BGT  , REL, 0x00, 0x2e, 2, "") \
XX(BHI  , REL, 0x00, 0x22, 2, "") \
XX(BHS  , REL, 0x00, 0x24, 2, "") \
XX(BITA , IMM, 0x00, 0x85, 2, "") \
XX(BITA , DIR, 0x00, 0x95, 2, "") \
XX(BITA , EXT, 0x00, 0xb5, 3, "") \
XX(BITA , IDX, 0x00, 0xa5, 2, "") \
XX(BITA , IDY, 0x18, 0xa5, 3, "") \
XX(BITB , IMM, 0x00, 0xc5, 2, "") \
XX(BITB , DIR, 0x00, 0xd5, 2, "") \
XX(BITB , EXT, 0x00, 0xf5, 3, "") \
XX(BITB , IDX, 0x00, 0xe5, 2, "") \
XX(BITB , IDY, 0x18, 0xe5, 3, "") \
XX(BLE  , REL, 0x00, 0x2f, 2, "") \
XX(BLO  , REL, 0x00, 0x25, 2, "") \
XX(BLS  , REL, 0x00, 0x23, 2, "") \
XX(BLT  , REL, 0x00, 0x2d, 2, "") \
XX(BMI  , REL, 0x00, 0x2b, 2, "") \
XX(BNE  , REL, 0x00, 0x26, 2, "") \
XX(BPL  , REL, 0x00, 0x2a, 2, "") \
XX(BRA  , REL, 0x00, 0x20, 2, "") \
XX(BRCLR, DIR, 0x00, 0x13, 4, "") \
XX(BRCLR, IDX, 0x00, 0x1f, 4, "") \
XX(BRCLR, IDY, 0x18, 0x1f, 5, "") \
XX(BRN  , REL, 0x00, 0x21, 2, "") \
XX(BRSET, DIR, 0x00, 0x12, 4, "") \
XX(BRSET, IDX, 0x00, 0x1e, 4, "") \
XX(BRSET, IDY, 0x18, 0x1e, 5, "") \
XX(BSET , DIR, 0x00, 0x14, 3, "") \
XX(BSET , IDX, 0x00, 0x1c, 3, "") \
XX(BSET , IDY, 0x18, 0x1c, 4, "") \
XX(BSR  , REL, 0x00, 0x8d, 2, "") \
XX(BVC  , REL, 0x00, 0x28, 2, "") \
XX(BVS  , REL, 0x00, 0x29, 2, "") \
XX(CBA  , INH, 0x00, 0x11, 1, "") \
XX(CLC  , INH, 0x00, 0x0c, 1, "") \
XX(CLI  , INH, 0x00, 0x0e, 1, "") \
XX(CLR  , EXT, 0x00, 0x7f, 3, "") \
XX(CLR  , IDX, 0x00, 0x6f, 2, "") \
XX(CLR  , IDY, 0x18, 0x6f, 3, "") \
XX(CLRA , INH, 0x00, 0x4f, 1, "") \
XX(CLRB , INH, 0x00, 0x5f, 1, "") \
XX(CLV  , INH, 0x00, 0x0a, 1, "") \
XX(CMPA , IMM, 0x00, 0x81, 2, "") \
XX(CMPA , DIR, 0x00, 0x91, 2, "") \
XX(CMPA , EXT, 0x00, 0xb1, 3, "") \
XX(CMPA , IDX, 0x00, 0xa1, 2, "") \
XX(CMPA , IDY, 0x18, 0xa1, 3, "") \
XX(CMPB , IMM, 0x00, 0xc1, 2, "") \
XX(CMPB , DIR, 0x00, 0xd1, 2, "") \
XX(CMPB , EXT, 0x00, 0xf1, 3, "") \
XX(CMPB , IDX, 0x00, 0xe1, 2, "") \
XX(CMPB , IDY, 0x18, 0xe1, 3, "") \
XX(COM  , EXT, 0x00, 0x73, 3, "") \
XX(COM  , IDX, 0x00, 0x63, 2, "") \
XX(COM  , IDY, 0x18, 0x63, 3, "") \
XX(COMA , INH, 0x00, 0x43, 1, "") \
XX(COMB , INH, 0x00, 0x53, 1, "") \
XX(CPD  , IMM, 0x1a, 0x83, 4, "") \
XX(CPD  , DIR, 0x1a, 0x93, 3, "") \
XX(CPD  , EXT, 0x1a, 0xb3, 4, "") \
XX(CPD  , IDX, 0x1a, 0xa3, 3, "") \
XX(CPD  , IDY, 0xcd, 0xa3, 3, "") \
XX(CPX  , IMM, 0x00, 0x8c, 3, "") \
XX(CPX  , DIR, 0x00, 0x9c, 2, "") \
XX(CPX  , EXT, 0x00, 0xbc, 3, "") \
XX(CPX  , IDX, 0x00, 0xac, 2, "") \
XX(CPX  , IDY, 0xcd, 0xac, 3, "") \
XX(CPY  , IMM, 0x18, 0x8c, 4, "") \
XX(CPY  , DIR, 0x18, 0x9c, 3, "") \
XX(CPY  , EXT, 0x18, 0xbc, 4, "") \
XX(CPY  , IDX, 0x1a, 0xac, 3, "") \
XX(CPY  , IDY, 0x18, 0xac, 3, "") \
XX(DAA  , INH, 0x00, 0x19, 1, "") \
XX(DEC  , EXT, 0x00, 0x7a, 3, "") \
XX(DEC  , IDX, 0x00, 0x6a, 2, "") \
XX(DEC  , IDY, 0x18, 0x6a, 3, "") \
XX(DECA , INH, 0x00, 0x4a, 1, "") \
XX(DECB , INH, 0x00, 0x5a, 1, "") \
XX(DES  , INH, 0x00, 0x34, 1, "") \
XX(DEX  , INH, 0x00, 0x09, 1, "") \
XX(DEY  , INH, 0x18, 0x09, 1, "") \
XX(EORA , IMM, 0x00, 0x88, 2, "") \
XX(EORA , DIR, 0x00, 0x98, 2, "") \
XX(EORA , EXT, 0x00, 0xb8, 3, "") \
XX(EORA , IDX, 0x00, 0xa8, 2, "") \
XX(EORA , IDY, 0x18, 0xa8, 3, "") \
XX(EORB , IMM, 0x00, 0xc8, 2, "") \
XX(EORB , DIR, 0x00, 0xd8, 2, "") \
XX(EORB , EXT, 0x00, 0xf8, 3, "") \
XX(EORB , IDX, 0x00, 0xe8, 2, "") \
XX(EORB , IDY, 0x18, 0xe8, 3, "") \
XX(FDIV , INH, 0x00, 0x03, 1, "") \
XX(IDIV , INH, 0x00, 0x02, 1, "") \
XX(INC  , EXT, 0x00, 0x7c, 3, "%s++") \
XX(INC  , IDX, 0x00, 0x6c, 2, "%s++") \
XX(INC  , IDY, 0x18, 0x6c, 3, "%s++") \
XX(INCA , INH, 0x00, 0x4c, 1, "a++") \
XX(INCB , INH, 0x00, 0x5c, 1, "b++") \
XX(INS  , INH, 0x00, 0x31, 1, "s++") \
XX(INX  , INH, 0x00, 0x08, 1, "") \
XX(INY  , INH, 0x18, 0x08, 2, "") \
XX(JMP  , EXT, 0x00, 0x7e, 3, "") \
XX(JMP  , IDX, 0x00, 0x6e, 2, "") \
XX(JMP  , IDY, 0x18, 0x6e, 3, "") \
XX(JSR  , DIR, 0x00, 0x9d, 2, "") \
XX(JSR  , EXT, 0x00, 0xbd, 3, "") \
XX(JSR  , IDX, 0x00, 0xad, 2, "") \
XX(JSR  , IDY, 0x18, 0xad, 3, "") \
XX(LDAA , IMM, 0x00, 0x86, 2, "") \
XX(LDAA , DIR, 0x00, 0x96, 2, "") \
XX(LDAA , EXT, 0x00, 0xb6, 3, "") \
XX(LDAA , IDX, 0x00, 0xa6, 2, "") \
XX(LDAA , IDY, 0x18, 0xa6, 3, "") \
XX(LDAB , IMM, 0x00, 0xc6, 2, "") \
XX(LDAB , DIR, 0x00, 0xd6, 2, "") \
XX(LDAB , EXT, 0x00, 0xf6, 3, "") \
XX(LDAB , IDX, 0x00, 0xe6, 2, "") \
XX(LDAB , IDY, 0x18, 0xe6, 3, "") \
XX(LDD  , IMM, 0x00, 0xcc, 3, "d = %s") \
XX(LDD  , DIR, 0x00, 0xdc, 2, "d = %s") \
XX(LDD  , EXT, 0x00, 0xfc, 3, "d = %s") \
XX(LDD  , IDX, 0x00, 0xec, 2, "d = %s") \
XX(LDD  , IDY, 0x18, 0xec, 3, "d = %s") \
XX(LDS  , IMM, 0x00, 0x8e, 3, "s = %s") \
XX(LDS  , DIR, 0x00, 0x9e, 2, "s = %s") \
XX(LDS  , EXT, 0x00, 0xbe, 3, "s = %s") \
XX(LDS  , IDX, 0x00, 0xae, 2, "s = %s") \
XX(LDS  , IDY, 0x18, 0xae, 3, "s = %s") \
XX(LDX  , IMM, 0x00, 0xce, 3, "x = %s") \
XX(LDX  , DIR, 0x00, 0xde, 2, "x = %s") \
XX(LDX  , EXT, 0x00, 0xfe, 3, "x = %s") \
XX(LDX  , IDX, 0x00, 0xee, 2, "x = %s") \
XX(LDX  , IDY, 0xcd, 0xee, 3, "x = %s") \
XX(LDY  , IMM, 0x18, 0xce, 4, "y = %s") \
XX(LDY  , DIR, 0x18, 0xde, 3, "y = %s") \
XX(LDY  , EXT, 0x18, 0xfe, 4, "y = %s") \
XX(LDY  , IDX, 0x1a, 0xee, 3, "y = %s") \
XX(LDY  , IDY, 0x18, 0xee, 3, "y = %s") \
XX(LSL  , EXT, 0x00, 0x78, 3, "") \
XX(LSL  , IDX, 0x00, 0x68, 2, "") \
XX(LSL  , IDY, 0x18, 0x68, 3, "") \
XX(LSLA , INH, 0x00, 0x48, 1, "") \
XX(LSLB , INH, 0x00, 0x58, 1, "") \
XX(LSLD , INH, 0x00, 0x05, 1, "") \
XX(LSR  , EXT, 0x00, 0x74, 3, "") \
XX(LSR  , IDX, 0x00, 0x64, 2, "") \
XX(LSR  , IDY, 0x18, 0x64, 3, "") \
XX(LSRA , INH, 0x00, 0x44, 1, "") \
XX(LSRB , INH, 0x00, 0x54, 1, "") \
XX(LSRD , INH, 0x00, 0x04, 1, "") \
XX(MUL  , INH, 0x00, 0x3d, 1, "") \
XX(NEG  , EXT, 0x00, 0x70, 3, "") \
XX(NEG  , IDX, 0x00, 0x60, 2, "") \
XX(NEG  , IDY, 0x18, 0x60, 3, "") \
XX(NEGA , INH, 0x00, 0x40, 1, "") \
XX(NEGB , INH, 0x00, 0x50, 1, "") \
XX(NOP  , INH, 0x00, 0x01, 1, "") \
XX(ORAA , IMM, 0x00, 0x8a, 2, "") \
XX(ORAA , DIR, 0x00, 0x9a, 2, "") \
XX(ORAA , EXT, 0x00, 0xba, 3, "") \
XX(ORAA , IDX, 0x00, 0xaa, 2, "") \
XX(ORAA , IDY, 0x18, 0xaa, 3, "") \
XX(ORAB , IMM, 0x00, 0xca, 2, "") \
XX(ORAB , DIR, 0x00, 0xda, 2, "") \
XX(ORAB , EXT, 0x00, 0xfa, 3, "") \
XX(ORAB , IDX, 0x00, 0xea, 2, "") \
XX(ORAB , IDY, 0x18, 0xea, 3, "") \
XX(PSHA , INH, 0x00, 0x36, 1, "") \
XX(PSHB , INH, 0x00, 0x37, 1, "") \
XX(PSHX , INH, 0x00, 0x3c, 1, "") \
XX(PSHY , INH, 0x18, 0x3c, 1, "") \
XX(PULA , INH, 0x00, 0x32, 1, "") \
XX(PULB , INH, 0x00, 0x33, 1, "") \
XX(PULX , INH, 0x00, 0x38, 1, "") \
XX(PULY , INH, 0x18, 0x38, 1, "") \
XX(ROL  , EXT, 0x00, 0x79, 3, "") \
XX(ROL  , IDX, 0x00, 0x69, 2, "") \
XX(ROL  , IDY, 0x18, 0x69, 3, "") \
XX(ROLA , INH, 0x00, 0x49, 1, "") \
XX(ROLB , INH, 0x00, 0x59, 1, "") \
XX(ROR  , EXT, 0x00, 0x76, 3, "") \
XX(ROR  , IDX, 0x00, 0x66, 2, "") \
XX(ROR  , IDY, 0x18, 0x66, 3, "") \
XX(RORA , INH, 0x00, 0x46, 1, "") \
XX(RORB , INH, 0x00, 0x56, 1, "") \
XX(RTI  , INH, 0x00, 0x3b, 1, "") \
XX(RTS  , INH, 0x00, 0x39, 1, "") \
XX(SBA  , INH, 0x00, 0x10, 1, "") \
XX(SBCA , IMM, 0x00, 0x82, 2, "") \
XX(SBCA , DIR, 0x00, 0x92, 2, "") \
XX(SBCA , EXT, 0x00, 0xb2, 3, "") \
XX(SBCA , IDX, 0x00, 0xa2, 2, "") \
XX(SBCA , IDY, 0x18, 0xa2, 3, "") \
XX(SBCB , IMM, 0x00, 0xc2, 2, "") \
XX(SBCB , DIR, 0x00, 0xd2, 2, "") \
XX(SBCB , EXT, 0x00, 0xf2, 3, "") \
XX(SBCB , IDX, 0x00, 0xe2, 2, "") \
XX(SBCB , IDY, 0x18, 0xe2, 3, "") \
XX(SEC  , INH, 0x00, 0x0d, 1, "") \
XX(SEI  , INH, 0x00, 0x0f, 1, "") \
XX(SEV  , INH, 0x00, 0x0b, 1, "") \
XX(STAA , DIR, 0x00, 0x97, 2, "") \
XX(STAA , EXT, 0x00, 0xb7, 3, "") \
XX(STAA , IDX, 0x00, 0xa7, 2, "") \
XX(STAA , IDY, 0x18, 0xa7, 3, "") \
XX(STAB , DIR, 0x00, 0xd7, 2, "") \
XX(STAB , EXT, 0x00, 0xf7, 3, "") \
XX(STAB , IDX, 0x00, 0xe7, 2, "") \
XX(STAB , IDY, 0x18, 0xe7, 3, "") \
XX(STD  , DIR, 0x00, 0xdd, 2, "") \
XX(STD  , EXT, 0x00, 0xfd, 3, "") \
XX(STD  , IDX, 0x00, 0xed, 2, "") \
XX(STD  , IDY, 0x18, 0xed, 3, "") \
XX(STOP , INH, 0x00, 0xcf, 1, "") \
XX(STS  , DIR, 0x00, 0x9f, 2, "") \
XX(STS  , EXT, 0x00, 0xbf, 3, "") \
XX(STS  , IDX, 0x00, 0xaf, 2, "") \
XX(STS  , IDY, 0x18, 0xaf, 3, "") \
XX(STX  , DIR, 0x00, 0xdf, 2, "") \
XX(STX  , EXT, 0x00, 0xff, 3, "") \
XX(STX  , IDX, 0x00, 0xef, 2, "") \
XX(STX  , IDY, 0xcd, 0xef, 3, "") \
XX(STY  , DIR, 0x18, 0xdf, 3, "") \
XX(STY  , EXT, 0x18, 0xff, 4, "") \
XX(STY  , IDX, 0x1a, 0xef, 3, "") \
XX(STY  , IDY, 0x18, 0xef, 3, "") \
XX(SUBA , IMM, 0x00, 0x80, 2, "") \
XX(SUBA , DIR, 0x00, 0x90, 2, "") \
XX(SUBA , EXT, 0x00, 0xb0, 3, "") \
XX(SUBA , IDX, 0x00, 0xa0, 2, "") \
XX(SUBA , IDY, 0x18, 0xa0, 3, "") \
XX(SUBB , IMM, 0x00, 0xc0, 2, "") \
XX(SUBB , DIR, 0x00, 0xd0, 2, "") \
XX(SUBB , EXT, 0x00, 0xf0, 3, "") \
XX(SUBB , IDX, 0x00, 0xe0, 2, "") \
XX(SUBB , IDY, 0x18, 0xe0, 3, "") \
XX(SUBD , IMM, 0x00, 0x83, 3, "") \
XX(SUBD , DIR, 0x00, 0x93, 2, "") \
XX(SUBD , EXT, 0x00, 0xb3, 3, "") \
XX(SUBD , IDX, 0x00, 0xa3, 2, "") \
XX(SUBD , IDY, 0x18, 0xa3, 3, "") \
XX(SWI  , INH, 0x00, 0x3f, 1, "") \
XX(TAB  , INH, 0x00, 0x16, 1, "") \
XX(TAP  , INH, 0x00, 0x06, 1, "") \
XX(TBA  , INH, 0x00, 0x17, 1, "") \
XX(TEST , INH, 0x00, 0x00, 1, "") \
XX(TPA  , INH, 0x00, 0x07, 1, "") \
XX(TST  , EXT, 0x00, 0x7d, 3, "") \
XX(TST  , IDX, 0x00, 0x6d, 2, "") \
XX(TST  , IDY, 0x18, 0x6d, 3, "") \
XX(TSTA , INH, 0x00, 0x4d, 1, "") \
XX(TSTB , INH, 0x00, 0x5d, 1, "") \
XX(TSX  , INH, 0x00, 0x30, 1, "") \
XX(TSY  , INH, 0x18, 0x30, 2, "") \
XX(TXS  , INH, 0x00, 0x35, 1, "") \
XX(TYS  , INH, 0x18, 0x35, 2, "") \
XX(WAI  , INH, 0x00, 0x3e, 1, "") \
XX(XGDX , INH, 0x00, 0x8f, 1, "") \
XX(XGDY , INH, 0x18, 0x8f, 2, "") \
                            
#define OPCODE_DEF_CS(c, m, p, i, l, desc) \
	OpCodeDef(c, #c, m, p, i, l, desc),
	 
static const OpCodeDef OpcodeDefs[]=
{
	OPCODE_LIST(OPCODE_DEF_CS)
	OpCodeDef()
};	

//---------------------------------------------------------------------
#define VECTOR_LIST(XX) \
XX(VSCI, 0xffd6)\
XX(VSPI, 0xffd8)\
XX(VPAIE, 0xffda)\
XX(VPAO, 0xffdc)\
XX(VTOF, 0xffde)\
XX(VTOC5, 0xffe0)\
XX(VTOC4, 0xffe2)\
XX(VTOC3, 0xffe4)\
XX(VTOC2, 0xffe6)\
XX(VTOC1, 0xffe8)\
XX(VTIC3, 0xffea)\
XX(VTIC2, 0xffec)\
XX(VTIC1, 0xffee)\
XX(VRTI, 0xfff0)\
XX(VIRQ, 0xfff2)\
XX(VXIRQ, 0xfff4)\
XX(VSWI, 0xfff6)\
XX(VILLOP, 0xfff8)\
XX(VCOM, 0xfffa)\
XX(VCLM, 0xfffc)\
XX(VRESET, 0xfffe)\

#define VECTOR_DEF_CS(m, o) \
	VectorDef(#m, o),
	 
static const VectorDef VectorDefs[]=
{
	VECTOR_LIST(VECTOR_DEF_CS)
	VectorDef()
};	


//---------------------------------------------------------------------
#define _IO_BASE    0x1000
#define IOPORT_LIST(XX) \
XX(PORTA , 0x00) \
XX(PIOC  , 0x02) \
XX(PORTC , 0x03) \
XX(PORTB , 0x04) \
XX(PORTCL, 0x05) \
XX(DDRC  , 0x07) \
XX(PORTD , 0x08) \
XX(DDRD  , 0x09) \
XX(PORTE , 0x0A) \
XX(CFORC , 0x0B) \
XX(OC1M  , 0x0C) \
XX(OC1D  , 0x0D) \
XX(TCNT  , 0x0E) \
XX(TIC1  , 0x10) \
XX(TIC2  , 0x12) \
XX(TIC3  , 0x14) \
XX(TOC1  , 0x16) \
XX(TOC2  , 0x18) \
XX(TOC3  , 0x1A) \
XX(TOC4  , 0x1C) \
XX(TOC5  , 0x1E) \
XX(TCTL1 , 0x20) \
XX(TCTL2 , 0x21) \
XX(TMSK1 , 0x22) \
XX(TFLG1 , 0x23) \
XX(TMSK2 , 0x24) \
XX(TFLG2 , 0x25) \
XX(PACTL , 0x26) \
XX(PACNT , 0x27) \
XX(SPCR  , 0x28) \
XX(SPSR  , 0x29) \
XX(SPDR  , 0x2A) \
XX(BAUD  , 0x2B) \
XX(SCCR1 , 0x2C) \
XX(SCCR2 , 0x2D) \
XX(SCSR  , 0x2E) \
XX(SCDR  , 0x2F) \
XX(ADCTL , 0x30) \
XX(ADR1  , 0x31) \
XX(ADR2  , 0x32) \
XX(ADR3  , 0x33) \
XX(ADR4  , 0x34) \
XX(BPROT , 0x35) \
XX(EPROG , 0x36) \
XX(OPTION, 0x39) \
XX(COPRST, 0x3A) \
XX(PPROG , 0x3B) \
XX(HPRIO , 0x3C) \
XX(INIT  , 0x3D) \
XX(TEST1 , 0x3E) \
XX(CONFIG, 0x3F) \

#define IOPORT_DEF_CS(m, o) \
	IOPortDef(#m, _IO_BASE+o),
	 
static const IOPortDef IOPortDefs[]=
{
	IOPORT_LIST(IOPORT_DEF_CS)
	IOPortDef()
};	

//---------------------------------------------------------------------
const OpCodeDef* FindOpCode(const ubyte* data, uint startOffset, uint length)
{
	uint space = length - startOffset;

	for(uint n = 0; OpcodeDefs[n].length; n++)
	{
		const OpCodeDef& def = OpcodeDefs[n];

		//do it fit?
		if(space < def.length)
			continue;

		uint offset = startOffset;
		if(def.prefix)
		{
			//prefix match?
			if(data[offset] != def.prefix)
				continue;
			offset++;
		}
		//instruction match?
		if(data[offset] != def.inst)
			continue;

		return &def;
	}
	return nullptr;
}

//---------------------------------------------------------------------
const VectorDef* GetVector(uint n)
{
	if(!VectorDefs[n].name)
		return nullptr;
	
	return &VectorDefs[n];
}

//---------------------------------------------------------------------
void Context::InitDefaults(Family f)
{
	AddMemory(0x0000, 0x0400, Disa::MT_RAM, Disa::MP_R|Disa::MP_W|Disa::MP_X);
	AddMemory(0x0400, 0x10000, Disa::MT_ROM, Disa::MP_R|Disa::MP_X);
	AddMemory(0x1000, 0x1060, Disa::MT_IO, Disa::MP_R|Disa::MP_W);
	AddMemory(0x1800, 0x2000, Disa::MT_IO, Disa::MP_R|Disa::MP_W);

	for(uint n = 0; IOPortDefs[n].name; n++)
	{
		AddDataLocation(IOPortDefs[n].name, IOPortDefs[n].offset, DT_BYTE, 1);
	}

	for(uint n = 0; VectorDefs[n].name; n++)
	{
		AddDataLocation(VectorDefs[n].name, VectorDefs[n].offset, DT_SHORT, 1);
	}
}


}
