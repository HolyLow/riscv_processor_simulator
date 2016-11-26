/*******************************************************************/
/* To make it easy to add a new instruction, we define AND write   */
/* all the codes about instructions in file "riscv_instruction.h"  */
/* for definition, AND in file "riscv_instruction.c" for           */
/* implementation. If you want to add a new instruction to our     */
/* simulator, you could just simply follow the steps below:        */
/*                                                                 */
/*  1. Add the opcode of your new instruction(maybe your funct3    */
/*     AND funct7 as well, if there are any conflict with current  */
/*     opcodes) to the appropriate place(according to your         */
/*     instruction type) in the function GetINSTYPE in             */
/*     "riscv_instruction.c";                                      */
/*  2. Add your instruction entrance to function XX_Execute        */
/*     according to your instruction type. For example, if your    */
/*     instruction is R_TYPE, then your are welcome to the         */
/*     function R_Execute. You should add your entrance according  */
/*     to your opcode, funct3, AND maybe funct7 as well;           */
/*  3. Add the definition of your instruction to                   */
/*     "riscv_instruction.h", AND your implementation to           */
/*     "riscv_instruction.c".                                      */
/* If you take the steps above correctly, your new instruction     */
/* will work!                                                      */
/*******************************************************************/
#include "riscv_instruction.h"

// something for debug
extern bool debug_flag;
extern unsigned long int pause_addr;

// a flag which shows whether syscall exit happened
int EXIT_HAPPENED = FALSE;

void Error_NoDef(Riscv64_decoder* riscv_decoder)
{
	printf("Instruction %x not defined: opcode(0x%x), funct3(0x%x), funct7(0x%x), rs2(0x%x)\n",
	        riscv_decoder->inst, riscv_decoder->opcode, riscv_decoder->funct3, riscv_decoder->funct7, riscv_decoder->rs2);
	// exit(1);
}
// return the instuction type according to the opcode
INSTYPE GetINSTYPE(Riscv64_decoder* riscv_decoder)
{
	switch(riscv_decoder->opcode)
	{
		case 0x33: // b0110011
		case 0x53: // b1010011 fp
		case 0x3b: // b0111011
			// printf("R_TYPE RETURNED;");
			return R_TYPE;

		case 0x43: // b1000011 fp
		case 0x47: // b1000111 fp
		case 0x4b: // b1001011 fp
		case 0x4f: // b1001111 fp
			return R4_TYPE;

		case 0x67: // b1100111
		case 0x03: // b0000011
		case 0x73: // b1110011
		case 0x07: // b0000111 fp
			return I_TYPE;

		case 0x23: // b0100011
		case 0x27: // b0100111 fp
			return S_TYPE;

		case 0x63: // b1100011
			return SB_TYPE;

		case 0x37: // b0110111
		case 0x17: // b0010111
			return U_TYPE;

		case 0x6F: // b1101111
			return UJ_TYPE;

		case 0x13: // b0010011
			switch(riscv_decoder->funct3)
			{
				case 1: // b001
				case 5: // b101
					return R_TYPE;
				case 0: // b000
				case 2: // b010
				case 3: // b011
				case 4: // b100
				case 6: // b110
				case 7: // b111
					return I_TYPE;
				default:
					return NOT_DEFINED;
			}

		case 0x1b: // b0011011
			switch(riscv_decoder->funct3)
			{
				case 1: // b001
				case 5: // b101
					return R_TYPE;
				case 0: // b000
					return I_TYPE;
				default:
					return NOT_DEFINED;
			}
		default:
			return NOT_DEFINED;
	}
}

// execute R_TYPE instructions
void R_execute(Riscv64_decoder* riscv_decoder, Riscv64_register* riscv_register, Riscv64_memory* riscv_memory)
{
	switch(riscv_decoder->opcode)
	{
		case 0x33: // b0110011
			switch(riscv_decoder->funct3)
			{
				case 0: // b000
					switch(riscv_decoder->funct7)
					{
						case 0x00: // b0000000
							add(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							#ifdef DEBUG
							DEBUG_INST("add", "d12", riscv_decoder, riscv_register);
							#endif
							break;
						case 0x01: // b0000001
							mul(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							#ifdef DEBUG
							DEBUG_INST("mul", "d12", riscv_decoder, riscv_register);
							#endif
							break;
						case 0x20: // b0100000
							sub(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							#ifdef DEBUG
							DEBUG_INST("sub", "d12", riscv_decoder, riscv_register);
							#endif
							break;
						default:
							Error_NoDef(riscv_decoder);
					}
					break;
				case 1: // b001
					switch(riscv_decoder->funct7)
					{
						case 0x00: // b0000000
							sll(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							#ifdef DEBUG
							DEBUG_INST("sll", "d12", riscv_decoder, riscv_register);
							#endif
							break;
						case 0x01: // b0000001
							mulh(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							#ifdef DEBUG
							DEBUG_INST("mulh", "d12", riscv_decoder, riscv_register);
							#endif
							break;
						default:
							Error_NoDef(riscv_decoder);
					}
					break;
				case 2: // b010
					switch (riscv_decoder->funct7)
					{
						case 0x00: // b0000000
							slt(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							#ifdef DEBUG
							DEBUG_INST("slt", "d12", riscv_decoder, riscv_register);
							#endif
							break;
						case 0x01: // b0000001
							mulhsu(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							#ifdef DEBUG
							DEBUG_INST("mulhsu", "d12", riscv_decoder, riscv_register);
							#endif
							break;
						default:
							Error_NoDef(riscv_decoder);
					}
					break;
				case 3: // b011
					switch (riscv_decoder->funct7)
					{
						case 0x00: // b0000000
							sltu(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							#ifdef DEBUG
							DEBUG_INST("sltu", "d12", riscv_decoder, riscv_register);
							#endif
							break;
						case 0x01: // b0000001
							mulhu(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							#ifdef DEBUG
							DEBUG_INST("mulhu", "d12", riscv_decoder, riscv_register);
							#endif
							break;
						default:
							Error_NoDef(riscv_decoder);
					}
					break;
				case 4: // b100
					switch (riscv_decoder->funct7)
					{
						case 0x00: // b0000000
							XOR(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							#ifdef DEBUG
							DEBUG_INST("xor", "d12", riscv_decoder, riscv_register);
							#endif
							break;
						case 0x01: // b0000001
							divd(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							#ifdef DEBUG
							DEBUG_INST("divd", "d12", riscv_decoder, riscv_register);
							#endif
							break;
						default:
							Error_NoDef(riscv_decoder);
					}
					break;
				case 5: // b101
					switch (riscv_decoder->funct7)
					{
						case 0x00: // b0000000
							srl(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							#ifdef DEBUG
							DEBUG_INST("srl", "d12", riscv_decoder, riscv_register);
							#endif
							break;
						case 0x01: // b0000001
							divu(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							#ifdef DEBUG
							DEBUG_INST("divu", "d12", riscv_decoder, riscv_register);
							#endif
							break;
						case 0x20: // b0100000
							sra(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							#ifdef DEBUG
							DEBUG_INST("sra", "d12", riscv_decoder, riscv_register);
							#endif
							break;
						default:
							Error_NoDef(riscv_decoder);
					}
					break;
				case 6: // b110
					switch (riscv_decoder->funct7)
					{
						case 0x00: // b0000000
							OR(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							#ifdef DEBUG
							DEBUG_INST("or", "d12", riscv_decoder, riscv_register);
							#endif
							break;
						case 0x01: // b0000001
							rem(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							#ifdef DEBUG
							DEBUG_INST("rem", "d12", riscv_decoder, riscv_register);
							#endif
							break;
						default:
							Error_NoDef(riscv_decoder);
					}
					break;
				case 7: // b111
					switch (riscv_decoder->funct7)
					{
						case 0x00: // b0000000
							AND(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							#ifdef DEBUG
							DEBUG_INST("AND", "d12", riscv_decoder, riscv_register);
							#endif
							break;
						case 0x01: // b0000001
							remu(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							#ifdef DEBUG
							DEBUG_INST("remu", "d12", riscv_decoder, riscv_register);
							#endif
							break;
						default:
							Error_NoDef(riscv_decoder);
					}
					break;
				default:
					Error_NoDef(riscv_decoder);
			}
			break;
		case 0x53: // b1010011 fp
			switch(riscv_decoder->funct7)
			{
				case 0x00: // b0000000 fadd_S
					fadd_S(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
					break;
				case 0x01: // b0000001 fadd_D
					fadd_D(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
					break;
				case 0x04: // b0000100 fsub_S
					fsub_S(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
					break;
				case 0x05: // b0000101 fsub_D
					fsub_D(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
					break;
				case 0x08: // b0001000 fmul_S
					fmul_S(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
					break;
				case 0x09: // b0001001 fnum_D
					fmul_D(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
					break;
				case 0x0c: // b0001100 fdiv_S
					fdiv_S(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
					break;
				case 0x0d: // b0001101 fdiv_D
					fdiv_D(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
					break;
				case 0x2c: // b0101100 fsqrt_S
					fsqrt_S(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
					break;
				case 0x2d: // b0101101 fsqrt_D
					fsqrt_D(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
					break;
				case 0x14: // b0010100
					switch(riscv_decoder->funct3)
					{
						case 0: // b000 fmin_S
							fmin_S(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							break;
						case 1: // b001 fmax_S
							fmax_S(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							break;
						default:
							Error_NoDef(riscv_decoder);
					}
					break;
				case 0x15: // b0010101
					switch(riscv_decoder->funct3)
					{
						case 0: // b000 fmin_D
							fmin_D(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							break;
						case 1: // b001 fmax_D
							fmax_D(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							break;
						default:
							Error_NoDef(riscv_decoder);
					}
					break;
				case 0x20: // b0100000
					fcvt_S_D(riscv_register, riscv_decoder->rd, riscv_decoder->rs1);
					break;
				case 0x21: // b0100001
					fcvt_D_S(riscv_register, riscv_decoder->rd, riscv_decoder->rs1);
					break;
				case 0x50: // b1010000
					switch(riscv_decoder->funct3)
					{
						case 0: // b000 fle_S
							fle_S(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							break;
						case 1: // b001 flt_S
							flt_S(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							break;
						case 2: // b010 feq_S
							feq_S(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							break;
						default:
							Error_NoDef(riscv_decoder);
					}
					break;
				case 0x51: // b1010001
					switch(riscv_decoder->funct3)
					{
						case 0: // b000 fle_D
							fle_D(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							break;
						case 1: // b001 flt_D
							flt_D(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							break;
						case 2: // b010 feq_D
							feq_D(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							break;
						default:
							Error_NoDef(riscv_decoder);
					}
					break;
				case 0x60: // b1100000
					switch(riscv_decoder->rs2)
					{
						case 0: // b00000 fcvt_W_S
							fcvt_W_S(riscv_register, riscv_decoder->rd, riscv_decoder->rs1);
							break;
						case 1: // b00001 fcvt_WU_S
							fcvt_WU_S(riscv_register, riscv_decoder->rd, riscv_decoder->rs1);
							break;
						default:
							Error_NoDef(riscv_decoder);
					}
					break;
				case 0x61: // b1100001
					switch(riscv_decoder->rs2)
					{
						case 0: // b00000 fcvt_W_D
							fcvt_W_D(riscv_register, riscv_decoder->rd, riscv_decoder->rs1);
							break;
						case 1: // b00001 fcvt_WU_D
							fcvt_WU_D(riscv_register, riscv_decoder->rd, riscv_decoder->rs1);
							break;
						default:
							Error_NoDef(riscv_decoder);
					}
					break;
				case 0x68: // b1101000
					switch(riscv_decoder->rs2)
					{
						case 0: // b00000 fcvt_S_W
							fcvt_S_W(riscv_register, riscv_decoder->rd, riscv_decoder->rs1);
							break;
						case 1: // b00001 fcvt_S_WU
							fcvt_S_WU(riscv_register, riscv_decoder->rd, riscv_decoder->rs1);
							break;
						default:
							Error_NoDef(riscv_decoder);
					}
					break;
				case 0x69: // b1101001
					switch(riscv_decoder->rs2)
					{
						case 0: // b00000 fcvt_D_W
							fcvt_D_W(riscv_register, riscv_decoder->rd, riscv_decoder->rs1);
							break;
						case 1: // b00001 fcvt_D_WU
							fcvt_D_WU(riscv_register, riscv_decoder->rd, riscv_decoder->rs1);
							break;
						default:
							Error_NoDef(riscv_decoder);
					}
					break;
				default:
					Error_NoDef(riscv_decoder);
			}
			break;
		case 0x3b: // b0111011
			switch(riscv_decoder->funct3)
			{
				case 0:
					switch(riscv_decoder->funct7)
					{
						case 0x00: // b0000000
							addw(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							break;
						case 0x20: // b0100000
							subw(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							break;
						default:
							Error_NoDef(riscv_decoder);
					}
					break;
				case 1:
					sllw(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
					break;
				case 4:
					divw(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
					break;
				case 5:
					switch(riscv_decoder->funct7)
					{
						case 0x00: // b0000000
							srlw(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							break;
						case 0x01: // b0000001
							divuw(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							break;
						case 0x20: // b0100000
							sraw(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
							break;
						default:
							Error_NoDef(riscv_decoder);
					}
					break;
				case 6:
					remw(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
					break;
				case 7:
					remuw(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2);
					break;
				default:
					Error_NoDef(riscv_decoder);
			}
			break;
		case 0x1b: // b0010011
			switch(riscv_decoder->funct3)
			{
				case 1: // b001
					slliw(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->shamt32);
					break;
				case 5: // b101
					switch(riscv_decoder->funct7)
					{
						case 0x00: // b0000000
							srliw(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->shamt32);
							break;
						case 0x20: // b0100000
							sraiw(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->shamt32);
							break;
						default:
							Error_NoDef(riscv_decoder);
					}
					break;
				default:
					Error_NoDef(riscv_decoder);
			}
			break;
		case 0x13: // b0010011
			switch(riscv_decoder->funct3)
			{
				case 1: // b001
					switch(riscv_decoder->funct6)
					{
						case 0x00: // b0000000
							slli(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->shamt64);
							#ifdef DEBUG
							DEBUG_INST("slli", "d1s", riscv_decoder, riscv_register);
							#endif
							break;
						default:
							Error_NoDef(riscv_decoder);
					}
					break;
				case 5: // b101
					switch(riscv_decoder->funct6)
					{
						case 0x00: // b000000
							srli(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->shamt64);
							#ifdef DEBUG
							DEBUG_INST("srli", "d1s", riscv_decoder, riscv_register);
							#endif
							break;
						case 0x10: // b010000
							srai(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->shamt64);
							#ifdef DEBUG
							DEBUG_INST("srai", "d1s", riscv_decoder, riscv_register);
							#endif
							break;
						default:
							Error_NoDef(riscv_decoder);
					}
					break;
				default:
					Error_NoDef(riscv_decoder);
			}
			break;
		default:
			Error_NoDef(riscv_decoder);
	}
}
// execute R4_TYPE instructions
void R4_execute(Riscv64_decoder* riscv_decoder, Riscv64_register* riscv_register, Riscv64_memory* riscv_memory)
{
	switch(riscv_decoder->opcode)
	{
		case 0x43: // b1000011 fp
			switch(riscv_decoder->funct2)
			{
				case 0: // b00
					fmadd_S(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2, riscv_decoder->rs3);
					break;
				case 1: // b01
					fmadd_D(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2, riscv_decoder->rs3);
					break;
				default:
					Error_NoDef(riscv_decoder);
			}
			break;
		case 0x47: // b1000111 fp
			switch(riscv_decoder->funct2)
			{
				case 0: // b00
					fmsub_S(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2, riscv_decoder->rs3);
					break;
				case 1: // b01
					fmsub_D(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2, riscv_decoder->rs3);
					break;
				default:
					Error_NoDef(riscv_decoder);
			}
			break;
		case 0x4b: // b1001011 fp
			switch(riscv_decoder->funct2)
			{
				case 0: // b00
					fnmsub_S(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2, riscv_decoder->rs3);
					break;
				case 1: // b01
					fnmsub_D(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2, riscv_decoder->rs3);
					break;
				default:
					Error_NoDef(riscv_decoder);
			}
			break;
		case 0x4f: // b1001111 fp
			switch(riscv_decoder->funct2)
			{
				case 0: // b00
					fnmadd_S(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2, riscv_decoder->rs3);
					break;
				case 1: // b01
					fnmadd_D(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->rs2, riscv_decoder->rs3);
					break;
				default:
					Error_NoDef(riscv_decoder);
			}
			break;
		default:
			Error_NoDef(riscv_decoder);
	}
}
// execute I_TYPE instructions
void I_execute(Riscv64_decoder* riscv_decoder, Riscv64_register* riscv_register, Riscv64_memory* riscv_memory)
{
	switch(riscv_decoder->opcode)
	{
		case 0x67: // b1100111
			switch(riscv_decoder->funct3)
			{
				case 0: // b000
					jalr(riscv_register, riscv_memory, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->I_immediate);
					#ifdef DEBUG
					DEBUG_INST("jalr", "d1i", riscv_decoder, riscv_register);
					#endif
					break;
				default:
					Error_NoDef(riscv_decoder);
			}
			break;
		case 0x03: // b0000011
			switch(riscv_decoder->funct3)
			{
				case 0: // b000
					lb(riscv_register, riscv_memory, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->I_immediate);
					#ifdef DEBUG
					DEBUG_INST("lb", "d1i", riscv_decoder, riscv_register);
					#endif
					break;
				case 1: // b001
					lh(riscv_register, riscv_memory, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->I_immediate);
					#ifdef DEBUG
					DEBUG_INST("lh", "d1i", riscv_decoder, riscv_register);
					#endif
					break;
				case 2: // b010
					lw(riscv_register, riscv_memory, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->I_immediate);
					#ifdef DEBUG
					DEBUG_INST("lw", "d1i", riscv_decoder, riscv_register);
					#endif
					break;
				case 3: // b011
					ld(riscv_register, riscv_memory, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->I_immediate);
					#ifdef DEBUG
					DEBUG_INST("ld", "d1i", riscv_decoder, riscv_register);
					#endif
					break;
				case 4: // b100
					lbu(riscv_register, riscv_memory, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->I_immediate);
					#ifdef DEBUG
					DEBUG_INST("lbu", "d1i", riscv_decoder, riscv_register);
					#endif
					break;
				case 5: // b101
					lhu(riscv_register, riscv_memory, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->I_immediate);
					#ifdef DEBUG
					DEBUG_INST("lhu", "d1i", riscv_decoder, riscv_register);
					#endif
					break;
				case 6: // b110
					lwu(riscv_register, riscv_memory, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->I_immediate);
					#ifdef DEBUG
					DEBUG_INST("lwu", "d1i", riscv_decoder, riscv_register);
					#endif
					break;
				default:
					Error_NoDef(riscv_decoder);
			}
			break;
		case 0x73: // b1110011
			switch(riscv_decoder->funct3)
			{
				case 0: // b000
					scall(riscv_register, riscv_memory);
					break;
				default:
					Error_NoDef(riscv_decoder);
			}
		case 0x1b: // b0011011
			addiw(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->I_immediate);
			#ifdef DEBUG
			DEBUG_INST("addi", "d1i", riscv_decoder, riscv_register);
			#endif
			break;
		case 0x13: // b0010011
			switch(riscv_decoder->funct3)
			{
				case 0: // b000
					addi(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->I_immediate);
					#ifdef DEBUG
					DEBUG_INST("addi", "d1i", riscv_decoder, riscv_register);
					#endif
					break;
				case 2: // b010
					slti(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->I_immediate);
					#ifdef DEBUG
					DEBUG_INST("slti", "d1i", riscv_decoder, riscv_register);
					#endif
					break;
				case 3: // b011
					sltiu(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->I_immediate);
					#ifdef DEBUG
					DEBUG_INST("sltiu", "d1i", riscv_decoder, riscv_register);
					#endif
					break;
				case 4: // b100
					xori(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->I_immediate);
					#ifdef DEBUG
					DEBUG_INST("xori", "d1i", riscv_decoder, riscv_register);
					#endif
					break;
				case 6: // b110
					ori(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->I_immediate);
					#ifdef DEBUG
					DEBUG_INST("ori", "d1i", riscv_decoder, riscv_register);
					#endif
					break;
				case 7: // b111
					ANDi(riscv_register, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->I_immediate);
					#ifdef DEBUG
					DEBUG_INST("ANDi", "d1i", riscv_decoder, riscv_register);
					#endif
					break;
				default:
					Error_NoDef(riscv_decoder);
			}
			break;
		case 0x07: // b0000111
			switch(riscv_decoder->funct3)
			{
				case 2: // b010 flw
					flw(riscv_register, riscv_memory, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->I_immediate);
					break;
				case 3: // b011 fld
					fld(riscv_register, riscv_memory, riscv_decoder->rd, riscv_decoder->rs1, riscv_decoder->I_immediate);
					break;
				default:
					Error_NoDef(riscv_decoder);
			}
			break;
		default:
			Error_NoDef(riscv_decoder);
	}
}
// execute S_TYPE instructions
void S_execute(Riscv64_decoder* riscv_decoder, Riscv64_register* riscv_register, Riscv64_memory* riscv_memory)
{
	switch(riscv_decoder->opcode)
	{
		case 0x23: // b0100011
			switch(riscv_decoder->funct3)
			{
				case 0: // b000
					sb(riscv_register, riscv_memory, riscv_decoder->rs1, riscv_decoder->rs2, riscv_decoder->S_immediate);
					#ifdef DEBUG
					DEBUG_INST("sb", "12i", riscv_decoder, riscv_register);
					#endif
					break;
				case 1: // b001
					sh(riscv_register, riscv_memory, riscv_decoder->rs1, riscv_decoder->rs2, riscv_decoder->S_immediate);
					#ifdef DEBUG
					DEBUG_INST("sh", "12i", riscv_decoder, riscv_register);
					#endif
					break;
				case 2: // b010
					sw(riscv_register, riscv_memory, riscv_decoder->rs1, riscv_decoder->rs2, riscv_decoder->S_immediate);
					#ifdef DEBUG
					DEBUG_INST("sw", "12i", riscv_decoder, riscv_register);
					#endif
					break;
				case 3: // b011
					sd(riscv_register, riscv_memory, riscv_decoder->rs1, riscv_decoder->rs2, riscv_decoder->S_immediate);
					#ifdef DEBUG
					DEBUG_INST("sd", "12i", riscv_decoder, riscv_register);
					#endif
					break;
				default:
					Error_NoDef(riscv_decoder);
			}
			break;
		case 0x27:
			switch(riscv_decoder->funct3)
			{
				case 2: // b010 fsw
					fsw(riscv_register, riscv_memory, riscv_decoder->rs1, riscv_decoder->rs2, riscv_decoder->S_immediate);
					break;
				case 3: // b011 fsd
					fsd(riscv_register, riscv_memory, riscv_decoder->rs1, riscv_decoder->rs2, riscv_decoder->S_immediate);
					break;
				default:
					Error_NoDef(riscv_decoder);
			}
			break;
		default:
			Error_NoDef(riscv_decoder);
	}
}
// execute SB_TYPE instructions
void SB_execute(Riscv64_decoder* riscv_decoder, Riscv64_register* riscv_register, Riscv64_memory* riscv_memory)
{
	switch(riscv_decoder->opcode)
	{
		case 0x63: // b1100011
			switch(riscv_decoder->funct3)
			{
				case 0: // b000
					beq(riscv_register, riscv_memory, riscv_decoder->rs1, riscv_decoder->rs2, riscv_decoder->SB_immediate);
					#ifdef DEBUG
					DEBUG_INST("beq", "12i", riscv_decoder, riscv_register);
					#endif
					break;
				case 1: // b001
					bne(riscv_register, riscv_memory, riscv_decoder->rs1, riscv_decoder->rs2, riscv_decoder->SB_immediate);
					#ifdef DEBUG
					DEBUG_INST("bne", "12i", riscv_decoder, riscv_register);
					#endif
					break;
				case 4: // b400
					blt(riscv_register, riscv_memory, riscv_decoder->rs1, riscv_decoder->rs2, riscv_decoder->SB_immediate);
					#ifdef DEBUG
					DEBUG_INST("blt", "12i", riscv_decoder, riscv_register);
					#endif
					break;
				case 5: // b101
					bge(riscv_register, riscv_memory, riscv_decoder->rs1, riscv_decoder->rs2, riscv_decoder->SB_immediate);
					#ifdef DEBUG
					DEBUG_INST("bge", "12i", riscv_decoder, riscv_register);
					#endif
					break;
				case 6: // b110
					bltu(riscv_register, riscv_memory, riscv_decoder->rs1, riscv_decoder->rs2, riscv_decoder->SB_immediate);
					#ifdef DEBUG
					DEBUG_INST("bltu", "12i", riscv_decoder, riscv_register);
					#endif
					break;
				case 7: // b111
					bgeu(riscv_register, riscv_memory, riscv_decoder->rs1, riscv_decoder->rs2, riscv_decoder->SB_immediate);
					#ifdef DEBUG
					DEBUG_INST("bgeu", "12i", riscv_decoder, riscv_register);
					#endif
					break;
				default:
					Error_NoDef(riscv_decoder);
			}
			break;
		default:
			Error_NoDef(riscv_decoder);
	}
}
// execute U_TYPE instructions
void U_execute(Riscv64_decoder* riscv_decoder, Riscv64_register* riscv_register, Riscv64_memory* riscv_memory)
{
	switch(riscv_decoder->opcode)
	{
		case 0x37: // b0110111
			lui(riscv_register, riscv_memory, riscv_decoder->rd, riscv_decoder->U_immediate);
			#ifdef DEBUG
			DEBUG_INST("lui", "di", riscv_decoder, riscv_register);
			#endif
			break;
		case 0x17: // b0010111
			auipc(riscv_register, riscv_memory, riscv_decoder->rd, riscv_decoder->U_immediate);
			#ifdef DEBUG
			DEBUG_INST("auipc", "di", riscv_decoder, riscv_register);
			#endif
			break;
		default:
			Error_NoDef(riscv_decoder);
	}
}
// execute UJ_TYPE instructions
void UJ_execute(Riscv64_decoder* riscv_decoder, Riscv64_register* riscv_register, Riscv64_memory* riscv_memory)
{
	switch(riscv_decoder->opcode)
	{
		case 0x6F: // b1101111
			jal(riscv_register, riscv_memory, riscv_decoder->rd, riscv_decoder->UJ_immediate);
			#ifdef DEBUG
			DEBUG_INST("jal", "di", riscv_decoder, riscv_register);
			#endif
			break;
		default:
			Error_NoDef(riscv_decoder);
	}
}

/*********************************************/
/*                                           */
/* functions for instructions  RV32I base    */
/*                                           */
/*********************************************/
/* @param rd,rs1,rs2 stAND for the index of a register */

/* Loads */
void lb(Riscv64_register* riscv_register, Riscv64_memory* riscv_memory, int rd, int rs1, int imm)  // byte
{
	reg64 reg_value = get_register_general(riscv_register, rs1);
	reg8 load_value = get_memory_reg8(riscv_memory, (byte*)(reg_value + imm));
	set_register_general(riscv_register, rd, (long int)load_value);
}
void lh(Riscv64_register* riscv_register, Riscv64_memory* riscv_memory, int rd, int rs1, int imm)  // halfword
{
	reg64 reg_value = get_register_general(riscv_register, rs1);
	reg16 load_value = get_memory_reg16(riscv_memory, (byte*)(reg_value + imm));
	set_register_general(riscv_register, rd, (long int)load_value);
}
void lw(Riscv64_register* riscv_register, Riscv64_memory* riscv_memory, int rd, int rs1, int imm)  // word
{
	reg64 reg_value = get_register_general(riscv_register, rs1);
	reg32 load_value = get_memory_reg32(riscv_memory, (byte*)(reg_value + imm));
	set_register_general(riscv_register, rd, (long int)load_value);
}
void lbu(Riscv64_register* riscv_register, Riscv64_memory* riscv_memory, int rd, int rs1, int imm) // byte unsigned
{
	reg64 reg_value = get_register_general(riscv_register, rs1);
	reg8 load_value = get_memory_reg8(riscv_memory, (byte*)(reg_value + imm));
	set_register_general(riscv_register, rd, (unsigned long int)load_value);
}
void lhu(Riscv64_register* riscv_register, Riscv64_memory* riscv_memory, int rd, int rs1, int imm) // half unsigned
{
	reg64 reg_value = get_register_general(riscv_register, rs1);
	reg16 load_value = get_memory_reg16(riscv_memory, (byte*)(reg_value + imm));
	set_register_general(riscv_register, rd, (unsigned long int)load_value);
}

/* Stores */
void sb(Riscv64_register* riscv_register, Riscv64_memory* riscv_memory, int rs1, int rs2, int imm)  // byte
{
	reg64 reg_value = get_register_general(riscv_register, rs1);
	reg8 store_value = (reg8)get_register_general(riscv_register, rs2);
	set_memory_reg8(riscv_memory, (byte*)(reg_value + imm), store_value);
}
void sh(Riscv64_register* riscv_register, Riscv64_memory* riscv_memory, int rs1, int rs2, int imm)  // halfword
{
	reg64 reg_value = get_register_general(riscv_register, rs1);
	reg16 store_value = (reg16)get_register_general(riscv_register, rs2);
	set_memory_reg16(riscv_memory, (byte*)(reg_value + imm), store_value);
}
void sw(Riscv64_register* riscv_register, Riscv64_memory* riscv_memory, int rs1, int rs2, int imm)  // word
{
	reg64 reg_value = get_register_general(riscv_register, rs1);
	reg32 store_value = (reg32)get_register_general(riscv_register, rs2);
	set_memory_reg32(riscv_memory, (byte*)(reg_value + imm), store_value);
}

/* Arithmetic */
void add(Riscv64_register* riscv_register, int rd, int rs1, int rs2)               // add
{
	riscv_register->x[rd] = riscv_register->x[rs1] + riscv_register->x[rs2];
}

void addi(Riscv64_register* riscv_register, int rd, int rs1, int imm)              // add immediate
{
	riscv_register->x[rd] = riscv_register->x[rs1] + imm;
}
void sub(Riscv64_register* riscv_register, int rd, int rs1, int rs2)              // subtract
{
	riscv_register->x[rd] = riscv_register->x[rs1] - riscv_register->x[rs2];
}
void lui(Riscv64_register* riscv_register, Riscv64_memory* riscv_memory, int rd, int imm)       // load upper immediate
{
	set_register_general(riscv_register, rd, (long int)imm);
}
void auipc(Riscv64_register* riscv_register, Riscv64_memory* riscv_memory, int rd, int imm)     // add upper immediate to pc
{
	reg64 reg_value = get_register_pc(riscv_register) - sizeof(instruction);  // a bit tricky here, as the pc has self-increased in the fetch stage,
	                                                                          // we have to subtract it to get the current pc
	set_register_general(riscv_register, rd, (long int)(reg_value + imm));
}

/* Logical */
void XOR(Riscv64_register* riscv_register, int rd, int rs1, int rs2)        // xor
{
	riscv_register->x[rd] = riscv_register->x[rs1] ^ riscv_register->x[rs2];
}
void xori(Riscv64_register* riscv_register, int rd, int rs1, int imm)       // xor immediate
{
	riscv_register->x[rd] = riscv_register->x[rs1] ^ (long int)imm;
}
void OR(Riscv64_register* riscv_register, int rd, int rs1, int rs2)         // or
{
	riscv_register->x[rd] = riscv_register->x[rs1] | riscv_register->x[rs2];
}
void ori(Riscv64_register* riscv_register, int rd, int rs1, int imm)        // or immediate
{
	riscv_register->x[rd] = riscv_register->x[rs1] | (long int)imm;
}
void AND(Riscv64_register* riscv_register, int rd, int rs1, int rs2)        // AND
{
	riscv_register->x[rd] = riscv_register->x[rs1] & riscv_register->x[rs2];
}
void ANDi(Riscv64_register* riscv_register, int rd, int rs1, int imm)       // AND immediate
{
	riscv_register->x[rd] = riscv_register->x[rs1] & (long int)imm;
}

/* Shifts */
void sll(Riscv64_register* riscv_register, int rd, int rs1, int rs2)        // shift left
{
	reg64 shift_value = get_register_general(riscv_register, rs2) & 0x3F; // get the lower 6 bits of register rs2
	long int reg_value = get_register_general(riscv_register, rs1);
	set_register_general(riscv_register, rd, reg_value << shift_value);
}
void slli(Riscv64_register* riscv_register, int rd, int rs1, int shamt64)      // shift left immediate
{
	long int reg_value = get_register_general(riscv_register, rs1);
	set_register_general(riscv_register, rd, reg_value << shamt64);
}
void srl(Riscv64_register* riscv_register, int rd, int rs1, int rs2)        // shift right
{
	reg64 shift_value = get_register_general(riscv_register, rs2) & 0x3F; // get the lower 6 bits of register rs2
	unsigned long int reg_value = get_register_general(riscv_register, rs1);
	set_register_general(riscv_register, rd, reg_value >> shift_value);
}
void srli(Riscv64_register* riscv_register, int rd, int rs1, int shamt64)      // shift right immediate
{
	unsigned long int reg_value = get_register_general(riscv_register, rs1);
	set_register_general(riscv_register, rd, reg_value >> shamt64);
}
void sra(Riscv64_register* riscv_register, int rd, int rs1, int rs2)         // shift right arithmetic
{
	reg64 shift_value = get_register_general(riscv_register, rs2) & 0x3F; // get the lower 5 bits of register rs2
	long int reg_value = get_register_general(riscv_register, rs1);
	set_register_general(riscv_register, rd, reg_value >> shift_value);
}
void srai(Riscv64_register* riscv_register, int rd, int rs1, int shamt64)      // shift right arithmetic immediate
{
	long int reg_value = get_register_general(riscv_register, rs1);
	set_register_general(riscv_register, rd, reg_value >> shamt64);
}

/* Compare */
void slt(Riscv64_register* riscv_register, int rd, int rs1, int rs2)         // set <
{
	if((long int)(riscv_register->x[rs1] - riscv_register->x[rs2]) < 0)
		set_register_general(riscv_register, rd, 1);
	else
		set_register_general(riscv_register, rd, 0);
}
void slti(Riscv64_register* riscv_register, int rd, int rs1, int imm)        // set < immediate
{
	if((long int)(riscv_register->x[rs1] - (long int)imm) < 0)
		set_register_general(riscv_register, rd, 1);
	else
		set_register_general(riscv_register, rd, 0);
}
void sltu(Riscv64_register* riscv_register, int rd, int rs1, int rs2)        // set < unsigned
{
	if(riscv_register->x[rs1] < riscv_register->x[rs2])
		set_register_general(riscv_register, rd, 1);
	else
		set_register_general(riscv_register, rd, 0);
}
void sltiu(Riscv64_register* riscv_register, int rd, int rs1, int imm)       // set < unsigned immediate
{
	if(riscv_register->x[rs1] < (unsigned long int)imm)
		set_register_general(riscv_register, rd, 1);
	else
		set_register_general(riscv_register, rd, 0);
}

/* Branches */
void beq(Riscv64_register* riscv_register, Riscv64_memory* riscv_memory, int rs1, int rs2, int imm)
{
	reg64 reg_value = get_register_pc(riscv_register) - sizeof(instruction) + (long int)imm;  // a bit tricky here, as the pc has self-increased in the fetch stage,
	                                                                                          // we have to subtract it to get the current pc
	if(riscv_register->x[rs1] - riscv_register->x[rs2] == 0)
		set_register_pc(riscv_register, reg_value);
}
void bne(Riscv64_register* riscv_register, Riscv64_memory* riscv_memory, int rs1, int rs2, int imm)
{
	reg64 reg_value = get_register_pc(riscv_register) - sizeof(instruction) + (long int)imm;  // a bit tricky here, as the pc has self-increased in the fetch stage,
	                                                                                          // we have to subtract it to get the current pc
	if(riscv_register->x[rs1] - riscv_register->x[rs2] != 0)
		set_register_pc(riscv_register, reg_value);
}
void blt(Riscv64_register* riscv_register, Riscv64_memory* riscv_memory, int rs1, int rs2, int imm)
{
	reg64 reg_value = get_register_pc(riscv_register) - sizeof(instruction) + (long int)imm;  // a bit tricky here, as the pc has self-increased in the fetch stage,
	                                                                                          // we have to subtract it to get the current pc
	if((long int)(riscv_register->x[rs1] - riscv_register->x[rs2]) < 0)
		set_register_pc(riscv_register, reg_value);
}
void bge(Riscv64_register* riscv_register, Riscv64_memory* riscv_memory, int rs1, int rs2, int imm)
{
	reg64 reg_value = get_register_pc(riscv_register) - sizeof(instruction) + (long int)imm;  // a bit tricky here, as the pc has self-increased in the fetch stage,
	                                                                                          // we have to subtract it to get the current pc
	if((long int)(riscv_register->x[rs1] - riscv_register->x[rs2]) >= 0)
		set_register_pc(riscv_register, reg_value);
}
void bltu(Riscv64_register* riscv_register, Riscv64_memory* riscv_memory, int rs1, int rs2, int imm)
{
	reg64 reg_value = get_register_pc(riscv_register) - sizeof(instruction) + (long int)imm;  // a bit tricky here, as the pc has self-increased in the fetch stage,
	                                                                              // we have to subtract it to get the current pc
	if(riscv_register->x[rs1] < riscv_register->x[rs2])
		set_register_pc(riscv_register, reg_value);
}
void bgeu(Riscv64_register* riscv_register, Riscv64_memory* riscv_memory, int rs1, int rs2, int imm)
{
	reg64 reg_value = get_register_pc(riscv_register) - sizeof(instruction) + (long int)imm;  // a bit tricky here, as the pc has self-increased in the fetch stage,
	                                                                                          // we have to subtract it to get the current pc
	if(riscv_register->x[rs1] > riscv_register->x[rs2])
		set_register_pc(riscv_register, reg_value);
}

/* Jump & Link */
void jal(Riscv64_register* riscv_register, Riscv64_memory* riscv_memory, int rd, int imm)
{
	reg64 reg_value = get_register_pc(riscv_register);
	if (rd != 0) // check whether it is a 'j' instruction, if so, cancel writing the register rd
		set_register_general(riscv_register, rd, reg_value);
	reg_value = reg_value - sizeof(instruction) + (long int)imm; // a bit tricky here, as the pc has self-increased in the fetch stage,
	                                                             // we have to subtract it to get the current pc
	set_register_pc(riscv_register, reg_value);
}
void jalr(Riscv64_register* riscv_register, Riscv64_memory* riscv_memory, int rd, int rs1, int imm)
{
	reg64 reg_value = get_register_pc(riscv_register);
	if (rd != 0) // check whether the register writing is needed
		set_register_general(riscv_register, rd, reg_value);
	reg_value = get_register_general(riscv_register, rs1) + (long int)imm;
	if(reg_value & 1) // check the least significant bit of reg_value, if it is 1, than set it to 0
		reg_value ^= 1;
	set_register_pc(riscv_register, reg_value);
}

/* System */
void scall(Riscv64_register* riscv_register, Riscv64_memory* riscv_memory)
{
	#ifdef DEBUG
	printf("syscall happened!\n");
	#endif

	switch(riscv_register->x[17])
	{
		case 93: // exit
			#ifdef DEBUG
			printf("exit parameters: a1=%d, a2=%d, a3=%d\n", riscv_register->x[11], riscv_register->x[12], riscv_register->x[13]);
			#endif
			EXIT_HAPPENED = TRUE;
			break;
		case 63: // read
			riscv_register->x[10] = read(riscv_register->x[10], (void*)get_actual_addr(riscv_memory, riscv_register->x[11]), riscv_register->x[12]);
			break;
		case 64: // write
			riscv_register->x[10] = write(riscv_register->x[10], (void*)get_actual_addr(riscv_memory ,riscv_register->x[11]), riscv_register->x[12]);
			break;
		case 169: // time
		{

			struct timeval *tv_p;
			tv_p = (struct timeval*)get_actual_addr(riscv_memory, riscv_register->x[10]);
            riscv_register->x[10] = gettimeofday(tv_p, NULL);
            break;
		}
        case 214: // brk
        {
        	riscv_memory->edata = riscv_register->x[10];
        	break;
        }
        case 57: // close file
        	break;
        case 80: // fstat
        	break;
        case 62: // lseek
        	riscv_register->x[10] = (reg64)lseek((int)riscv_register->x[10], (off_t)riscv_register->x[11], (int)riscv_register->x[12]);
        	break;
		default:
			printf("System call type %d not defined!", (int)riscv_register->x[17]);
			// Error_NoDef(riscv_decoder);
	}
}

/*********************************************/
/*                                           */
/* functions for instructions RV32M          */
/*                                           */
/*********************************************/
void mul(Riscv64_register* riscv_register, int rd, int rs1, int rs2)    // xlen*xlen -> lower xlen to rd
{
	int value_rs1 = (int)get_register_general(riscv_register, rs1);
	int value_rs2 = (int)get_register_general(riscv_register, rs2);
	int result = value_rs1 * value_rs2;
	set_register_general(riscv_register, rd, (reg64)result);
}
void mulh(Riscv64_register* riscv_register, int rd, int rs1, int rs2)   // signed * signed  high
{
	int value_rs1 = (int)get_register_general(riscv_register, rs1);
	int value_rs2 = (int)get_register_general(riscv_register, rs2);
	long long result = (long long)value_rs1 * (long long)value_rs2;
	result >>= 32;
	set_register_general(riscv_register, rd, (reg64)result);
}
void mulhsu(Riscv64_register* riscv_register, int rd, int rs1, int rs2) // unsigned * unsigned  high
{
	unsigned int value_rs1 = (unsigned int)get_register_general(riscv_register, rs1);
	unsigned int value_rs2 = (unsigned int)get_register_general(riscv_register, rs2);
	unsigned long long result = (unsigned long long)value_rs1 * (unsigned long long)value_rs2;
	result >>= 32;
	set_register_general(riscv_register, rd, (reg64)result);
}
void mulhu(Riscv64_register* riscv_register, int rd, int rs1, int rs2)  // signed *unsigned  high
{
	int value_rs1 = (int)get_register_general(riscv_register, rs1); // multiplicAND
	unsigned int value_rs2 = (unsigned int)get_register_general(riscv_register, rs2); // multiplier
	long long result = (long long)value_rs1 * (unsigned long long)value_rs2;
	result >>= 32;
	set_register_general(riscv_register, rd, (reg64)result);
}
void divd(Riscv64_register* riscv_register, int rd, int rs1, int rs2)    // signed / signed
{
	int value_rs1 = (int)get_register_general(riscv_register, rs1);
	int value_rs2 = (int)get_register_general(riscv_register, rs2);
	int result = value_rs1 / value_rs2;
	set_register_general(riscv_register, rd, (reg64)result);
}
void divu(Riscv64_register* riscv_register, int rd, int rs1, int rs2)   // unsigned / unsigned
{
	unsigned int value_rs1 = (unsigned int)get_register_general(riscv_register, rs1);
	unsigned int value_rs2 = (unsigned int)get_register_general(riscv_register, rs2);
	unsigned int result = value_rs1 / value_rs2;
	set_register_general(riscv_register, rd, (reg64)result);
}
void rem(Riscv64_register* riscv_register, int rd, int rs1, int rs2)    // signed / signed  remainder
{
	int value_rs1 = (int)get_register_general(riscv_register, rs1);
	int value_rs2 = (int)get_register_general(riscv_register, rs2);
	int result = value_rs1 % value_rs2;
	set_register_general(riscv_register, rd, (reg64)result);
}
void remu(Riscv64_register* riscv_register, int rd, int rs1, int rs2)   // unsigned / unsgined remainder
{
	unsigned int value_rs1 = (unsigned int)get_register_general(riscv_register, rs1);
	unsigned int value_rs2 = (unsigned int)get_register_general(riscv_register, rs2);
	unsigned int result = value_rs1 % value_rs2;
	set_register_general(riscv_register, rd, (reg64)result);
}

/*********************************************/
/*                                           */
/* functions for instructions  RV64I         */
/*                                           */
/*********************************************/

/* Loads */
void lwu(Riscv64_register* riscv_register, Riscv64_memory* riscv_memory, int rd, int rs1, int imm)
{
	reg64 reg_value = get_register_general(riscv_register, rs1);
	reg32 load_value = get_memory_reg32(riscv_memory, (byte*)(reg_value + imm));
	set_register_general(riscv_register, rd, (unsigned long int)load_value);
}
void ld(Riscv64_register* riscv_register, Riscv64_memory* riscv_memory, int rd, int rs1, int imm)
{
	reg64 reg_value = get_register_general(riscv_register, rs1);
	reg64 load_value = get_memory_reg64(riscv_memory, (byte*)(reg_value + imm));
	set_register_general(riscv_register, rd, load_value);
}
/* Stores */
void sd(Riscv64_register* riscv_register, Riscv64_memory* riscv_memory, int rs1, int rs2, int imm)
{
	reg64 reg_value = get_register_general(riscv_register, rs1);
	reg64 store_value = get_register_general(riscv_register, rs2);
	set_memory_reg64(riscv_memory, (byte*)(reg_value + imm), store_value);
}

/* Arithmetic */
void addw(Riscv64_register* riscv_register, int rd, int rs1, int rs2)
{
	riscv_register->x[rd] = (unsigned long int)((int)riscv_register->x[rs1] + (int)riscv_register->x[rs2]);
}
void addiw(Riscv64_register* riscv_register, int rd, int rs1, int imm)
{
	riscv_register->x[rd] = (unsigned long int)((int)riscv_register->x[rs1] + (int)imm);
}
void subw(Riscv64_register* riscv_register, int rd, int rs1, int rs2)
{
	riscv_register->x[rd] = (unsigned long int)((int)riscv_register->x[rs1] - (int)riscv_register->x[rs2]);
}

/* Shifts */
void sllw(Riscv64_register* riscv_register, int rd, int rs1, int rs2)
{
	reg64 shift_value = get_register_general(riscv_register, rs2) & 0x1F; // get the lower 5 bits of register rs2
	int reg_value = (int)get_register_general(riscv_register, rs1);
	set_register_general(riscv_register, rd, (long int)(reg_value << shift_value));
}
void slliw(Riscv64_register* riscv_register, int rd, int rs1, int shamt32)
{
	int reg_value = (int)get_register_general(riscv_register, rs1);
	set_register_general(riscv_register, rd, (long int)(reg_value << shamt32));
}
void srlw(Riscv64_register* riscv_register, int rd, int rs1, int rs2)
{
	reg64 shift_value = get_register_general(riscv_register, rs2) & 0x1F; // get the lower 5 bits of register rs2
	unsigned int reg_value = (unsigned int)get_register_general(riscv_register, rs1);
	set_register_general(riscv_register, rd, (long int)(reg_value >> shift_value));
}
void srliw(Riscv64_register* riscv_register, int rd, int rs1, int shamt32)
{
	unsigned int reg_value = (unsigned int)get_register_general(riscv_register, rs1);
	set_register_general(riscv_register, rd, (long int)(reg_value >> shamt32));
}
void sraw(Riscv64_register* riscv_register, int rd, int rs1, int rs2)
{
	reg64 shift_value = get_register_general(riscv_register, rs2) & 0x1F; // get the lower 5 bits of register rs2
	int reg_value = get_register_general(riscv_register, rs1);
	set_register_general(riscv_register, rd, (long int)(reg_value >> shift_value));
}
void sraiw(Riscv64_register* riscv_register, int rd, int rs1, int shamt32)
{
	int reg_value = (int)get_register_general(riscv_register, rs1);
	set_register_general(riscv_register, rd, (long int)(reg_value >> shamt32));
}


/*********************************************/
/*                                           */
/* functions for instructions RV64M          */
/*                                           */
/*********************************************/
void mulw(Riscv64_register* riscv_register, int rd, int rs1, int rs2)
{
	int value_rs1 = (int)get_register_general(riscv_register, rs1);
	int value_rs2 = (int)get_register_general(riscv_register, rs2);
	int result = value_rs1 * value_rs2;
	set_register_general(riscv_register, rd, (long int)result);
}
void divw(Riscv64_register* riscv_register, int rd, int rs1, int rs2)
{
	int value_rs1 = (int)get_register_general(riscv_register, rs1);
	int value_rs2 = (int)get_register_general(riscv_register, rs2);
	int result = value_rs1 / value_rs2;
	set_register_general(riscv_register, rd, (long int)result);
}
void divuw(Riscv64_register* riscv_register, int rd, int rs1, int rs2)
{
	unsigned int value_rs1 = (unsigned int)get_register_general(riscv_register, rs1);
	unsigned int value_rs2 = (unsigned int)get_register_general(riscv_register, rs2);
	unsigned int result = value_rs1 / value_rs2;
	set_register_general(riscv_register, rd, (unsigned long int)result);
}
void remw(Riscv64_register* riscv_register, int rd, int rs1, int rs2)
{
	int value_rs1 = (int)get_register_general(riscv_register, rs1);
	int value_rs2 = (int)get_register_general(riscv_register, rs2);
	int result = value_rs1 % value_rs2;
	set_register_general(riscv_register, rd, (long int)result);
}
void remuw(Riscv64_register* riscv_register, int rd, int rs1, int rs2)
{
	unsigned int value_rs1 = (unsigned int)get_register_general(riscv_register, rs1);
	unsigned int value_rs2 = (unsigned int)get_register_general(riscv_register, rs2);
	unsigned int result = value_rs1 % value_rs2;
	set_register_general(riscv_register, rd, (unsigned long int)result);
}


/*********************************************/
/*                                           */
/* functions for instructions RV32F          */
/*                                           */
/*********************************************/

/* Load */
void flw(Riscv64_register* riscv_register, Riscv64_memory* riscv_memory, int rd, int rs1, int imm)
{
	reg64 reg_value = get_register_general(riscv_register, rs1);
	reg32 load_value = get_memory_reg32(riscv_memory, (byte*)(reg_value + imm));
	set_register_fp(riscv_register, rd, (unsigned long int)load_value);
}

/* Store */
void fsw(Riscv64_register* riscv_register, Riscv64_memory* riscv_memory, int rs1, int rs2, int imm)
{
	reg64 reg_value = get_register_general(riscv_register, rs1);
	reg32 store_value = (reg32)get_register_fp(riscv_register, rs2);
	set_memory_reg32(riscv_memory, (byte*)(reg_value + imm), store_value);
}

/* Arithmetics */
void fadd_S(Riscv64_register* riscv_register, int rd, int rs1, int rs2)
{
	float temp = *((float*)&riscv_register->f[rs1]) + *((float*)&riscv_register->f[rs2]);
	riscv_register->f[rd] = (reg64)(*(unsigned int*)&temp);
}
void fsub_S(Riscv64_register* riscv_register, int rd, int rs1, int rs2)
{
	float temp = *((float*)&riscv_register->f[rs1]) - *((float*)&riscv_register->f[rs2]);
	riscv_register->f[rd] = (reg64)(*(unsigned int*)&temp);
}
void fmul_S(Riscv64_register* riscv_register, int rd, int rs1, int rs2)
{
	float temp = *((float*)&riscv_register->f[rs1]) * *((float*)&riscv_register->f[rs2]);
	riscv_register->f[rd] = (reg64)(*(unsigned int*)&temp);
}
void fdiv_S(Riscv64_register* riscv_register, int rd, int rs1, int rs2)
{
	float temp = *((float*)&riscv_register->f[rs1]) / *((float*)&riscv_register->f[rs2]);
	riscv_register->f[rd] = (reg64)(*(unsigned int*)&temp);
}
void fmin_S(Riscv64_register* riscv_register, int rd, int rs1, int rs2)
{
	float temp = MIN(*((float*)&riscv_register->f[rs1]), *((float*)&riscv_register->f[rs2]));
	riscv_register->f[rd] = (reg64)(*(unsigned int*)&temp);
}
void fmax_S(Riscv64_register* riscv_register, int rd, int rs1, int rs2)
{
	float temp = MAX(*((float*)&riscv_register->f[rs1]), *((float*)&riscv_register->f[rs2]));
	riscv_register->f[rd] = (reg64)(*(unsigned int*)&temp);
}
void fsqrt_S(Riscv64_register* riscv_register, int rd, int rs1, int rs2)
{
	float temp = sqrt(*((float*)&riscv_register->f[rs1]));
	riscv_register->f[rd] = (reg64)(*(unsigned int*)&temp);
}

void fmadd_S(Riscv64_register* riscv_register, int rd, int rs1, int rs2, int rs3)  // rs1*rs2+rs3
{
	float temp = *((float*)&riscv_register->f[rs1]) * *((float*)&riscv_register->f[rs2]) + *((float*)&riscv_register->f[rs3]);
	riscv_register->f[rd] = (reg64)(*(unsigned int*)&temp);
}
void fmsub_S(Riscv64_register* riscv_register, int rd, int rs1, int rs2, int rs3)  // rs1*rs2-rs3
{
	float temp = *((float*)&riscv_register->f[rs1]) * *((float*)&riscv_register->f[rs2]) - *((float*)&riscv_register->f[rs3]);
	riscv_register->f[rd] = (reg64)(*(unsigned int*)&temp);
}
void fnmadd_S(Riscv64_register* riscv_register, int rd, int rs1, int rs2, int rs3) // -(rs1*rs2+rs3)
{
	float temp = -(*((float*)&riscv_register->f[rs1]) * *((float*)&riscv_register->f[rs2]) + *((float*)&riscv_register->f[rs3]));
	riscv_register->f[rd] = (reg64)(*(unsigned int*)&temp);
}
void fnmsub_S(Riscv64_register* riscv_register, int rd, int rs1, int rs2, int rs3) // -(rs1*rs2-rs3)
{
	float temp = -(*((float*)&riscv_register->f[rs1]) * *((float*)&riscv_register->f[rs2]) - *((float*)&riscv_register->f[rs3]));
	riscv_register->f[rd] = (reg64)(*(unsigned int*)&temp);
}

/* Type Conversion */
void fcvt_W_S(Riscv64_register* riscv_register, int rd, int rs1)  // single-precision fp   -> signed word(32-bit)
{
	reg64 src_reg64 = get_register_fp(riscv_register, rs1);
	float src_float = *((float*)&src_reg64);
	int dest_int = (int)src_float;
	set_register_general(riscv_register, rd, (long int)dest_int);
}
void fcvt_WU_S(Riscv64_register* riscv_register, int rd, int rs1) // single-precision fp   -> unsigned word(32-bit)
{
	reg64 src_reg64 = get_register_fp(riscv_register, rs1);
	float src_float = *((float*)&src_reg64);
	unsigned int dest_uint = (unsigned int)src_float;
	set_register_general(riscv_register, rd, (unsigned long int)dest_uint);
}
void fcvt_S_W(Riscv64_register* riscv_register, int rd, int rs1)  // signed word(32-bit)   -> single-precision fp
{
	reg64 src_reg64 = get_register_general(riscv_register, rs1);
	int src_int = (int)src_reg64;
	float dest_float = (float)src_int;
	set_register_fp(riscv_register, rd, (unsigned long int)(*((unsigned int*)&dest_float)));
}
void fcvt_S_WU(Riscv64_register* riscv_register, int rd, int rs1) // unsigned word(32-bit) -> single-precision fp
{
	reg64 src_reg64 = get_register_general(riscv_register, rs1);
	unsigned int src_uint = (unsigned int)src_reg64;
	float dest_float = (float)src_uint;
	set_register_fp(riscv_register, rd, (unsigned long int)(*((unsigned int*)&dest_float)));
}

/* Compare */
void feq_S(Riscv64_register* riscv_register, int rd, int rs1, int rs2) // ==
{
	reg64 src1_reg64 = get_register_fp(riscv_register, rs1);
	float src1_float = *((float*)&src1_reg64);
	reg64 src2_reg64 = get_register_fp(riscv_register, rs2);
	float src2_float = *((float*)&src2_reg64);
	if(src1_float == src2_float)
		set_register_general(riscv_register, rd, 1);
	else
		set_register_general(riscv_register, rd, 0);
}
void flt_S(Riscv64_register* riscv_register, int rd, int rs1, int rs2) // <
{
	reg64 src1_reg64 = get_register_fp(riscv_register, rs1);
	float src1_float = *((float*)&src1_reg64);
	reg64 src2_reg64 = get_register_fp(riscv_register, rs2);
	float src2_float = *((float*)&src2_reg64);
	if(src1_float < src2_float)
		set_register_general(riscv_register, rd, 1);
	else
		set_register_general(riscv_register, rd, 0);
}
void fle_S(Riscv64_register* riscv_register, int rd, int rs1, int rs2) // <=
{
	reg64 src1_reg64 = get_register_fp(riscv_register, rs1);
	float src1_float = *((float*)&src1_reg64);
	reg64 src2_reg64 = get_register_fp(riscv_register, rs2);
	float src2_float = *((float*)&src2_reg64);
	if(src1_float <= src2_float)
		set_register_general(riscv_register, rd, 1);
	else
		set_register_general(riscv_register, rd, 0);
}


/*********************************************/
/*                                           */
/* functions for instructions RV32D          */
/*                                           */
/*********************************************/

/* Load */
void fld(Riscv64_register* riscv_register, Riscv64_memory* riscv_memory, int rd, int rs1, int imm)
{
	reg64 reg_value = get_register_general(riscv_register, rs1);
	reg64 load_value = get_memory_reg64(riscv_memory, (byte*)(reg_value + imm));
	set_register_fp(riscv_register, rd, (unsigned long int)load_value);
}

/* Store */
void fsd(Riscv64_register* riscv_register, Riscv64_memory* riscv_memory, int rs1, int rs2, int imm)
{
	reg64 reg_value = get_register_general(riscv_register, rs1);
	reg64 store_value = (reg64)get_register_fp(riscv_register, rs2);
	set_memory_reg64(riscv_memory, (byte*)(reg_value + imm), store_value);
}

/* Arithmetics */
void fadd_D(Riscv64_register* riscv_register, int rd, int rs1, int rs2)
{
	double temp = *((double*)&riscv_register->f[rs1]) + *((double*)&riscv_register->f[rs2]);
	riscv_register->f[rd] = (reg64)(*(unsigned long int*)&temp);
}
void fsub_D(Riscv64_register* riscv_register, int rd, int rs1, int rs2)
{
	double temp = *((double*)&riscv_register->f[rs1]) - *((double*)&riscv_register->f[rs2]);
	riscv_register->f[rd] = (reg64)(*(unsigned long int*)&temp);
}
void fmul_D(Riscv64_register* riscv_register, int rd, int rs1, int rs2)
{
	double temp = *((double*)&riscv_register->f[rs1]) * *((double*)&riscv_register->f[rs2]);
	riscv_register->f[rd] = (reg64)(*(unsigned long int*)&temp);
}
void fdiv_D(Riscv64_register* riscv_register, int rd, int rs1, int rs2)
{
	double temp = *((double*)&riscv_register->f[rs1]) / *((double*)&riscv_register->f[rs2]);
	riscv_register->f[rd] = (reg64)(*(unsigned long int*)&temp);
}
void fmin_D(Riscv64_register* riscv_register, int rd, int rs1, int rs2)
{
	double temp = MIN(*((double*)&riscv_register->f[rs1]), *((double*)&riscv_register->f[rs2]));
	riscv_register->f[rd] = (reg64)(*(unsigned long int*)&temp);
}
void fmax_D(Riscv64_register* riscv_register, int rd, int rs1, int rs2)
{
	double temp = MAX(*((double*)&riscv_register->f[rs1]), *((double*)&riscv_register->f[rs2]));
	riscv_register->f[rd] = (reg64)(*(unsigned long int*)&temp);
}
void fsqrt_D(Riscv64_register* riscv_register, int rd, int rs1, int rs2)
{
	double temp = sqrt(*((double*)&riscv_register->f[rs1]));
	riscv_register->f[rd] = (reg64)(*(unsigned long int*)&temp);
}

void fmadd_D(Riscv64_register* riscv_register, int rd, int rs1, int rs2, int rs3)  // rs1*rs2+rs3
{
	double temp = *((double*)&riscv_register->f[rs1]) * *((double*)&riscv_register->f[rs2]) + *((double*)&riscv_register->f[rs3]);
	riscv_register->f[rd] = (reg64)(*(unsigned long int*)&temp);
}
void fmsub_D(Riscv64_register* riscv_register, int rd, int rs1, int rs2, int rs3)  // rs1*rs2-rs3
{
	double temp = *((double*)&riscv_register->f[rs1]) * *((double*)&riscv_register->f[rs2]) - *((double*)&riscv_register->f[rs3]);
	riscv_register->f[rd] = (reg64)(*(unsigned long int*)&temp);
}
void fnmadd_D(Riscv64_register* riscv_register, int rd, int rs1, int rs2, int rs3) // -(rs1*rs2+rs3)
{
	double temp = -(*((double*)&riscv_register->f[rs1]) * *((double*)&riscv_register->f[rs2]) + *((double*)&riscv_register->f[rs3]));
	riscv_register->f[rd] = (reg64)(*(unsigned long int*)&temp);
}
void fnmsub_D(Riscv64_register* riscv_register, int rd, int rs1, int rs2, int rs3) // -(rs1*rs2-rs3)
{
	double temp = -(*((double*)&riscv_register->f[rs1]) * *((double*)&riscv_register->f[rs2]) - *((double*)&riscv_register->f[rs3]));
	riscv_register->f[rd] = (reg64)(*(unsigned long int*)&temp);
}

/* Type Conversion */
void fcvt_S_D(Riscv64_register* riscv_register, int rd, int rs1) // double-precision fp -> single-precision fp
{
	reg64 src_reg64 = get_register_fp(riscv_register, rs1);
	double src_double = *((double*)&src_reg64);
	float dest_float = (float)src_double;
	set_register_general(riscv_register, rd, (unsigned long int)dest_float);
}
void fcvt_D_S(Riscv64_register* riscv_register, int rd, int rs1) // single-precision fp -> double-precision fp
{
	reg64 src_reg64 = get_register_fp(riscv_register, rs1);
	float src_float = *((float*)&src_reg64);
	double dest_double = (double)src_float;
	set_register_general(riscv_register, rd, (unsigned long int)dest_double);
}
void fcvt_W_D(Riscv64_register* riscv_register, int rd, int rs1)  // double-precision fp   -> single word(32-bit)
{
	reg64 src_reg64 = get_register_fp(riscv_register, rs1);
	double src_double = *((double*)&src_reg64);
	int dest_int = (int)src_double;
	set_register_general(riscv_register, rd, (long int)dest_int);
}
void fcvt_WU_D(Riscv64_register* riscv_register, int rd, int rs1) // double-precision fp   -> unsigned word(32-bit)
{
	reg64 src_reg64 = get_register_fp(riscv_register, rs1);
	double src_double = *((double*)&src_reg64);
	unsigned int dest_uint = (unsigned int)src_double;
	set_register_general(riscv_register, rd, (unsigned long int)dest_uint);
}
void fcvt_D_W(Riscv64_register* riscv_register, int rd, int rs1)  // signed word(32-bit)   -> double-precision fp
{
	reg64 src_reg64 = get_register_general(riscv_register, rs1);
	int src_int = (int)src_reg64;
	double dest_double = (double)src_int;
	set_register_fp(riscv_register, rd, (unsigned long int)(*((unsigned int*)&dest_double)));
}
void fcvt_D_WU(Riscv64_register* riscv_register, int rd, int rs1) // unsigned word(32-bit) -> double-precision fp
{
	reg64 src_reg64 = get_register_general(riscv_register, rs1);
	unsigned int src_uint = (unsigned int)src_reg64;
	double dest_double = (double)src_uint;
	set_register_fp(riscv_register, rd, (unsigned long int)(*((unsigned int*)&dest_double)));
}

/* Compare */
void feq_D(Riscv64_register* riscv_register, int rd, int rs1, int rs2) // ==
{
	reg64 src1_reg64 = get_register_fp(riscv_register, rs1);
	double src1_double = *((double*)&src1_reg64);
	reg64 src2_reg64 = get_register_fp(riscv_register, rs2);
	double src2_double = *((double*)&src2_reg64);
	if(src1_double == src2_double)
		set_register_general(riscv_register, rd, 1);
	else
		set_register_general(riscv_register, rd, 0);
}
void flt_D(Riscv64_register* riscv_register, int rd, int rs1, int rs2) // <
{
	reg64 src1_reg64 = get_register_fp(riscv_register, rs1);
	double src1_double = *((double*)&src1_reg64);
	reg64 src2_reg64 = get_register_fp(riscv_register, rs2);
	double src2_double = *((double*)&src2_reg64);
	if(src1_double < src2_double)
		set_register_general(riscv_register, rd, 1);
	else
		set_register_general(riscv_register, rd, 0);
}
void fle_D(Riscv64_register* riscv_register, int rd, int rs1, int rs2) // <=
{
	reg64 src1_reg64 = get_register_fp(riscv_register, rs1);
	double src1_double = *((double*)&src1_reg64);
	reg64 src2_reg64 = get_register_fp(riscv_register, rs2);
	double src2_double = *((double*)&src2_reg64);
	if(src1_double <= src2_double)
		set_register_general(riscv_register, rd, 1);
	else
		set_register_general(riscv_register, rd, 0);
}
