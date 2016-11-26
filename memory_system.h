#ifndef __MEMORY_SYSTEM_H__
#define __MEMORY_SYSTEM_H__
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "memory.h"
#include "cache.h"

typedef unsigned char      reg8;
typedef unsigned short int reg16;
typedef unsigned int       reg32;
typedef unsigned long int  reg64;

// typedef unsigned char bool;
typedef unsigned char byte;
typedef unsigned int instruction;

#define FALSE 0
#define TRUE  1

// abi
#define zero x[0]    // Hard-wired zero
#define ra   x[1]    // return address
#define sp   x[2]    // stack pointer
#define gp   x[3]    // global pointer
#define tp   x[4]    // thread pointer
#define t0   x[5]    // temporaries
#define t1   x[6]    // ..
#define t2   x[7]    // ..
#define s0   x[8]    // saved register
#define fp   x[8]    // frame pointer
#define s1   x[9]    // saved register
#define a0   x[10]   // function argument/return value
#define a1   x[11]   // function argument/return value
#define a2   x[12]   // function argument
#define a3   x[13]   // ..
#define a4   x[14]   // ..
#define a5   x[15]   // ..
#define a6   x[16]   // ..
#define a7   x[17]   // ..
#define s2   x[18]   // saved register
#define s3   x[19]   // ..
#define s4   x[20]   // ..
#define s5   x[21]   // ..
#define s6   x[22]   // ..
#define s7   x[23]   // ..
#define s8   x[24]   // ..
#define s9   x[25]   // ..
#define s10  x[26]   // ..
#define s11  x[27]   // ..
#define t3   x[28]   // temporaries
#define t4   x[29]   // ..
#define t5   x[30]   // ..
#define t6   x[31]   // ..

// memory size 128Mb
#define MEM_SIZE 1<<27           // 0x0800 0000
#define STACK_BOTTOM 0x6000000   // virtual address of stack
#define TRUE 1
#define FALSE 0

// decoder
typedef struct riscv64_decoder{
	instruction inst;
	int opcode;
	int funct3;
	int funct7;
	int funct6;
	int rd;
	int rs1;
	int rs2;
	int shamt64;
	int shamt32;
	int I_immediate;
	int S_immediate;
	int SB_immediate;
	int U_immediate;
	int UJ_immediate;
	int immediate;
	// floating-point
	int csr;
	int funct5;
	int funct2;
	int fmt;
	int rm;
	int rs3;
	int width;
} Riscv64_decoder;


// register file
typedef struct riscv64_register{
	// integer
	reg64 pc;
	reg64 x[32];
	// floating point
	reg64 fcsr;
	reg64 f[32];
} Riscv64_register;

// memory
typedef struct riscv64_memory{
	// main memory
	long int mem_size;
	byte *memory;
	byte *stack_bottom;
	// top of the heap
	byte* edata;

} Riscv64_memory;


/*********************************************/
/*                                           */
/* initialization and gc                     */
/*                                           */
/*********************************************/

void init_decoder(Riscv64_decoder**);
void init_memory(Riscv64_memory**);
void init_register(Riscv64_register**, Riscv64_memory*);
void init_cache_simulator();
void cachesimulator_output();
void delete_memory_system(Riscv64_decoder*, Riscv64_register*, Riscv64_memory*); // free the memory

/*********************************************/
/*                                           */
/* functions for decoder                     */
/*                                           */
/*********************************************/
void decode_instruction(Riscv64_decoder*);


/*********************************************/
/*                                           */
/* functions for memory                      */
/*                                           */
/*********************************************/

/* note: actual_addr stands for the addr in your machine  */
/*       while virtual_addr for the addr in the simulator */
byte* get_actual_addr(Riscv64_memory*, byte* virtual_addr); // return the actual addr in this machine
byte* get_virtual_addr(Riscv64_memory*, byte* actual_addr); // invese function of the function above
bool out_of_memory_virtual(Riscv64_memory*, byte* virtual_addr);
bool out_of_memory_actual(Riscv64_memory*, byte* actual_addr); // judge if the actual address is out of virtual memory
void check_valid_memory_virtual(Riscv64_memory*, byte* virtual_addr); // check if the virtual memory is valid, if not exit(1)

/* note: the only way to access memory is through vitual_addr */
void  set_memory_reg8(Riscv64_memory*, byte* virtual_addr, reg8 value);
reg8  get_memory_reg8(Riscv64_memory*, byte* virtual_addr);
void  set_memory_reg16(Riscv64_memory*, byte* virtual_addr, reg16 value);
reg16 get_memory_reg16(Riscv64_memory*, byte* virtual_addr);
void  set_memory_reg32(Riscv64_memory*, byte* virtual_addr, reg32 value);
reg32 get_memory_reg32(Riscv64_memory*, byte* virtual_addr);
void  set_memory_reg64(Riscv64_memory*, byte* virtual_addr, reg64 value);
reg64 get_memory_reg64(Riscv64_memory*, byte* virtual_addr);


/*********************************************/
/*                                           */
/* functions for register file               */
/*                                           */
/*********************************************/
/* integer */
void set_register_pc(Riscv64_register*, reg64 value); // set pc
reg64 get_register_pc(Riscv64_register*); // get a 64-bit value from pc

void set_register_general(Riscv64_register*, int index, reg64 value); // set general register x[index]
reg64 get_register_general(Riscv64_register*, int index); // get a 64-bit value from x[index]
void register_pc_self_increase(Riscv64_register*); // pc self-increase the the next instruction

/* floating point */
void set_register_fcsr(Riscv64_register*, reg64 value); // set fcsr (64-bit needed)
reg64 get_register_fcsr(Riscv64_register*); // get a 64-bit value from fcsr

void set_register_fp(Riscv64_register*, int index, reg64 value);// set floating-point register f[index] = value (64-bit needed)
reg64 get_register_fp(Riscv64_register*, int index); // get a 64-bit value from floating-pointer register f[index]

#endif
