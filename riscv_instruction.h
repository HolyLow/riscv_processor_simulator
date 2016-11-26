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
#ifndef __RISCV_INSTRUCTION_H__
#define __RISCV_INSTRUCTION_H__
#include "memory_system.h"
#include "debug.h"
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
/* some tool macro */
#define MAX(a,b)  (a>b?a:b)
#define MIN(a,b)  (a<b?a:b)

// instruction type
typedef enum
{
	R_TYPE, R4_TYPE, I_TYPE, S_TYPE, SB_TYPE, U_TYPE, UJ_TYPE, NOT_DEFINED
}INSTYPE;

/* a tool, create a binary number like this :   */
/*                                              */
/*     value:  000... 00000111...1111000...000  */
/*  position:  31...       y  ...   x ...  210  */
/*                                              */
/*  for all k if (31>=x>=k>=y>=0), bit(k) = 1,  */
/*                      otherwise, bit(k) = 0   */
#define ONES(x,y)        (((1<<(x+1))-1)-((1<<y)-1))

/* same in all instructions */
#define OPCODE(inst)     inst&ONES(6,0)              // 7
#define FUNCT3(inst)     ((inst&ONES(14,12))>>12)    // 3
#define FUNCT7(inst)     ((inst&ONES(31,25))>>25)    // 7
#define FUNCT6(inst)     ((inst&ONES(31,26))>>26)    // 6
#define RD(inst)         ((inst&ONES(11,7))>>7)      // 5
#define RS1(inst)        ((inst&ONES(19,15))>>15)    // 5
#define RS2(inst)        ((inst&ONES(24,20))>>20)    // 5
#define SHAMT64(inst)    ((inst&ONES(25,20))>>20)    // 6, RV64I
#define SHAMT32(inst)    ((inst&ONES(24,20))>>20)    // 5, RV64I
#define IMM_SIGN(inst)   ((inst>>31)&1)              // sign of immediate
/* floating-point */
#define CSR(inst)        ((inst&ONES(31,20))>>20)    // 12
#define FUNCT5(inst)     ((inst&ONES(31,27))>>27)    // 5
#define FMT(inst)        ((inst&ONES(26,25))>>25)    // 2
#define FUNCT2(inst)     ((inst&ONES(26,25))>>25)    // 2
#define RM(inst)         ((inst&ONES(14,12))>>12)    // 3
#define RS3(inst)        ((inst&ONES(31,27))>>27)    // 5
#define WIDTH(inst)      ((inst&ONES(14,12))>>12)    // 3


/* --------------------   immediates  ------------------------    */


/* R-type                               */
/* funct7--rrs2--rrs1--funct3--rd--opcode */
/*    7-----5----5-----3------5----7    */

// no immediate

/* I-type                               */
/* imm[11:0]--rrs1--funct3--rd--opcode   */
/*    12-------5-----3-----5-----7      */
#define I_IMM(inst)      (((inst&ONES(31,20)) >> 20) | (IMM_SIGN(inst)*ONES(31,11)))

/* S-type                                        */
/* imm[11:5]--rrs2--rrs1--funct3--imm[4:0]--opcode */
/*     7-------5----5-----3--------5--------7    */
#define S_IMM(inst)      (((inst&ONES(11,7))>>7) | ((inst&ONES(30,25))>>20) | (IMM_SIGN(inst)*ONES(31,11)))

/* SB-type */
/* imm[12|10:5]--rs2--rs1--funct3--imm[4:1|11]--opcode */
/*     7----------5----5-----3-----------5--------7    */
#define SB_IMM(inst)     (((inst&ONES(11,8))>>7) | ((inst&ONES(30,25))>>20) | ((inst&ONES(7,7))<<4) | (IMM_SIGN(inst)*ONES(31,12)))

/* U-type                 */
/* imm[31:12]--rd--opcode */
/*    20--------5----7    */
#define U_IMM(inst)      (inst&ONES(31,12))

/* J-type */
#define J_IMM(inst)      (((inst&ONES(30,21))>>20) | ((inst&ONES(20,20))>>9) | (inst&ONES(19,12)) | (IMM_SIGN(inst)*ONES(31,20)))

/* UJ-type */
#define UJ_IMM(inst)     (((inst&ONES(30,21))>>20) | ((inst&ONES(20,20))>>9) | (inst&ONES(19,12)) | (IMM_SIGN(inst)*ONES(31,20)))
// same as J-type?

// error when an undefined instruction occurs
void Error_NoDef(Riscv64_decoder*);
// return the instruction tyoe according to the decoder
INSTYPE GetINSTYPE(Riscv64_decoder*);

// execute different instructions according to their types
void R_execute(Riscv64_decoder*, Riscv64_register*, Riscv64_memory*);
void R4_execute(Riscv64_decoder*, Riscv64_register*, Riscv64_memory*);
void I_execute(Riscv64_decoder*, Riscv64_register*, Riscv64_memory*);
void S_execute(Riscv64_decoder*, Riscv64_register*, Riscv64_memory*);
void SB_execute(Riscv64_decoder*, Riscv64_register*, Riscv64_memory*);
void U_execute(Riscv64_decoder*, Riscv64_register*, Riscv64_memory*);
void UJ_execute(Riscv64_decoder*, Riscv64_register*, Riscv64_memory*);



/*********************************************/
/*                                           */
/* functions for instructions  RV32I base    */
/*                                           */
/*********************************************/
/* @param rd,rrs1,rrs2 stAND for the index of a register */

/* Loads */
void lb(Riscv64_register*, Riscv64_memory*, int rd, int rs1, int imm);  // byte
void lh(Riscv64_register*, Riscv64_memory*, int rd, int rs1, int imm);  // halfword
void lw(Riscv64_register*, Riscv64_memory*, int ld, int rs1, int imm);  // word
void lbu(Riscv64_register*, Riscv64_memory*, int ld, int rs1, int imm); // byte unsigned
void lhu(Riscv64_register*, Riscv64_memory*, int ld, int rs1, int imm); // half unsigned

/* Stores */
void sb(Riscv64_register*, Riscv64_memory*, int rs1, int rs2, int imm);  // byte
void sh(Riscv64_register*, Riscv64_memory*, int rs1, int rs2, int imm);  // halfword
void sw(Riscv64_register*, Riscv64_memory*, int rs1, int rs2, int imm);  // word

/* Arithmetic */
void add(Riscv64_register*, int rd, int rs1, int rs2);               // add
void addi(Riscv64_register*, int rd, int rs1, int imm);              // add immediate
void sub(Riscv64_register*, int rd, int rs1, int rs2);               // subtract
void lui(Riscv64_register*, Riscv64_memory*, int rd, int imm);       // load upper immediate
void auipc(Riscv64_register*, Riscv64_memory*, int rd, int imm);     // add upper immediate to pc

/* Logical */
void XOR(Riscv64_register*, int rd, int rs1, int rs2);        // xor
void xori(Riscv64_register*, int rd, int rs1, int imm);       // xor immediate
void OR(Riscv64_register*, int rd, int rs1, int rs2);         // or
void ori(Riscv64_register*, int rd, int rs1, int imm);        // or immediate
void AND(Riscv64_register*, int rd, int rs1, int rs2);        // AND
void ANDi(Riscv64_register*, int rd, int rs1, int imm);       // AND immediate

/* Shifts */
void sll(Riscv64_register*, int rd, int rs1, int rs2);         // shift left
void slli(Riscv64_register*, int rd, int rs1, int shamt64);      // shift left immediate
void srl(Riscv64_register*, int rd, int rs1, int rs2);         // shift right
void srli(Riscv64_register*, int rd, int rs1, int shamt64);      // shift right immediate
void sra(Riscv64_register*, int rd, int rs1, int rs2);         // shift right arithmetic
void srai(Riscv64_register*, int rd, int rs1, int shamt64);      // shift right arithmetic immediate

/* Compare */
void slt(Riscv64_register*, int rd, int rs1, int rs2);         // set <
void slti(Riscv64_register*, int rd, int rs1, int imm);        // set < immediate
void sltu(Riscv64_register*, int rd, int rs1, int rs2);        // set < unsigned
void sltiu(Riscv64_register*, int rd, int rs1, int imm);       // set < unsigned immediate

/* Branches */
void beq(Riscv64_register*, Riscv64_memory*, int rs1, int rs2, int imm);
void bne(Riscv64_register*, Riscv64_memory*, int rs1, int rs2, int imm);
void blt(Riscv64_register*, Riscv64_memory*, int rs1, int rs2, int imm);
void bge(Riscv64_register*, Riscv64_memory*, int rs1, int rs2, int imm);
void bltu(Riscv64_register*, Riscv64_memory*, int rs1, int rs2, int imm);
void bgeu(Riscv64_register*, Riscv64_memory*, int rs1, int rs2, int imm);


/* Jump & Link */
void jal(Riscv64_register*, Riscv64_memory*, int rd, int imm);
void jalr(Riscv64_register*, Riscv64_memory*, int rd, int rs1, int imm);

/* System */
void scall(Riscv64_register*, Riscv64_memory*);

/*********************************************/
/*                                           */
/* functions for instructions RV32M          */
/*                                           */
/*********************************************/
void mul(Riscv64_register* riscv_register, int rd, int rs1, int rs2);    // xlen*xlen -> lower xlen to rd
void mulh(Riscv64_register* riscv_register, int rd, int rs1, int rs2);   // signed * signed  high
void mulhsu(Riscv64_register* riscv_register, int rd, int rs1, int rs2); // unsigned * unsigned  high
void mulhu(Riscv64_register* riscv_register, int rd, int rs1, int rs2);  // signed *unsigned  high
void divd(Riscv64_register* riscv_register, int rd, int rs1, int rs2);    // signed / signed
void divu(Riscv64_register* riscv_register, int rd, int rs1, int rs2);   // unsigned / unsigned
void rem(Riscv64_register* riscv_register, int rd, int rs1, int rs2);    // signed / signed  remainder
void remu(Riscv64_register* riscv_register, int rd, int rs1, int rs2);   // unsigned / unsgined remainder



/*********************************************/
/*                                           */
/* functions for instructions  RV64I         */
/*                                           */
/*********************************************/

/* Loads */
void lwu(Riscv64_register*, Riscv64_memory*, int rd, int rs1, int imm);
void ld(Riscv64_register*, Riscv64_memory*, int rd, int rs1, int imm);

/* Stores */
void sd(Riscv64_register*, Riscv64_memory*, int rs1, int rs2, int imm);


/* Arithmetic */
void addw(Riscv64_register*, int rd, int rs1, int rs2);
void addiw(Riscv64_register*, int rd, int rs1, int imm);
void subw(Riscv64_register*, int rd, int rs1, int rs2);

/* Shifts */
void sllw(Riscv64_register*, int rd, int rs1, int rs2);
void slliw(Riscv64_register*, int rd, int rs1, int shamt32);
void srlw(Riscv64_register*, int rd, int rs1, int rs2);
void srliw(Riscv64_register*, int rd, int rs1, int shamt32);
void sraw(Riscv64_register*, int rd, int rs1, int rs2);
void sraiw(Riscv64_register*, int rd, int rs1, int shamt32);



/*********************************************/
/*                                           */
/* functions for instructions RV64M          */
/*                                           */
/*********************************************/
void mulw(Riscv64_register*, int rd, int rs1, int rs2);
void divw(Riscv64_register*, int rd, int rs1, int rs2);
void divuw(Riscv64_register*, int rd, int rs1, int rs2);
void remw(Riscv64_register*, int rd, int rs1, int rs2);
void remuw(Riscv64_register*, int rd, int rs1, int rs2);



/*********************************************/
/*                                           */
/* functions for instructions RV32F          */
/*                                           */
/*********************************************/

/* Load */
void flw(Riscv64_register*, Riscv64_memory*, int rd, int rs1, int imm);

/* Store */
void fsw(Riscv64_register*, Riscv64_memory*, int rs1, int rs2, int imm);

/* Arithmetics */
void fadd_S(Riscv64_register*, int rd, int rs1, int rs2);
void fsub_S(Riscv64_register*, int rd, int rs1, int rs2);
void fmul_S(Riscv64_register*, int rd, int rs1, int rs2);
void fdiv_S(Riscv64_register*, int rd, int rs1, int rs2);
void fmin_S(Riscv64_register*, int rd, int rs1, int rs2);
void fmax_S(Riscv64_register*, int rd, int rs1, int rs2);
void fsqrt_S(Riscv64_register*, int rd, int rs1, int rs2);

void fmadd_S(Riscv64_register*, int rd, int rs1, int rs2, int rs3);  // rs1*rs2+rs3
void fmsub_S(Riscv64_register*, int rd, int rs1, int rs2, int rs3);  // rs1*rs2-rs3
void fnmadd_S(Riscv64_register*, int rd, int rs1, int rs2, int rs3); // -(rs1*rs2+rs3)
void fnmsub_S(Riscv64_register*, int rd, int rs1, int rs2, int rs3); // -(rs1*rs2-rs3)

/* Type Conversion */
void fcvt_W_S(Riscv64_register*, int rd, int rs1);  // single-precision fp   -> signed word(32-bit)
void fcvt_WU_S(Riscv64_register*, int rd, int rs1); // single-precision fp   -> unsigned word(32-bit)
void fcvt_S_W(Riscv64_register*, int rd, int rs1);  // signed word(32-bit)   -> single-precision fp
void fcvt_S_WU(Riscv64_register*, int rd, int rs1); // unsigned word(32-bit) -> single-precision fp

/* Compare */
void feq_S(Riscv64_register*, int rd, int rs1, int rs2); // ==
void flt_S(Riscv64_register*, int rd, int rs1, int rs2); // <
void fle_S(Riscv64_register*, int rd, int rs1, int rs2); // <=


/*********************************************/
/*                                           */
/* functions for instructions RV32D          */
/*                                           */
/*********************************************/

/* Load */
void fld(Riscv64_register*, Riscv64_memory*, int rd, int rs1, int imm);

/* Store */
void fsd(Riscv64_register*, Riscv64_memory*, int rs1, int rs2, int imm);

/* Arithmetics */
void fadd_D(Riscv64_register*, int rd, int rs1, int rs2);
void fsub_D(Riscv64_register*, int rd, int rs1, int rs2);
void fmul_D(Riscv64_register*, int rd, int rs1, int rs2);
void fdiv_D(Riscv64_register*, int rd, int rs1, int rs2);
void fmin_D(Riscv64_register*, int rd, int rs1, int rs2);
void fmax_D(Riscv64_register*, int rd, int rs1, int rs2);
void fsqrt_D(Riscv64_register*, int rd, int rs1, int rs2);

void fmadd_D(Riscv64_register*, int rd, int rs1, int rs2, int rs3);  // rs1*rs2+rs3
void fmsub_D(Riscv64_register*, int rd, int rs1, int rs2, int rs3);  // rs1*rs2-rs3
void fnmadd_D(Riscv64_register*, int rd, int rs1, int rs2, int rs3); // -(rs1*rs2+rs3)
void fnmsub_D(Riscv64_register*, int rd, int rs1, int rs2, int rs3); // -(rs1*rs2-rs3)

/* Type Conversion */
void fcvt_S_D(Riscv64_register*, int rd, int rs1); // double-precision fp -> single-precision fp
void fcvt_D_S(Riscv64_register*, int rd, int rs1); // single-precision fp -> double-precision fp
void fcvt_W_D(Riscv64_register*, int rd, int rs1);  // double-precision fp   -> signed word(32-bit)
void fcvt_WU_D(Riscv64_register*, int rd, int rs1); // double-precision fp   -> unsigned word(32-bit)
void fcvt_D_W(Riscv64_register*, int rd, int rs1);  // signed word(32-bit)   -> double-precision fp
void fcvt_D_WU(Riscv64_register*, int rd, int rs1); // unsigned word(32-bit) -> double-precision fp

/* Compare */
void feq_D(Riscv64_register*, int rd, int rs1, int rs2); // ==
void flt_D(Riscv64_register*, int rd, int rs1, int rs2); // <
void fle_D(Riscv64_register*, int rd, int rs1, int rs2); // <=

#endif
