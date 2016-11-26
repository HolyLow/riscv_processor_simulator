#ifndef __DEBUG_H__
#define __DEBUG_H__
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "memory_system.h"

//#define DEBUG

// void DEBUG(char* p1, ...);
void DEBUG_MESSAGE(char* message);
void DEBUG_INST(char* inst_name, char* type, Riscv64_decoder*, Riscv64_register*);
void MY_DEBUG_MODE(Riscv64_register*, Riscv64_memory*);
void DEBUG_SHOW_REG_GENERAL(Riscv64_register*);

#endif
