#include "debug.h"

// somthing for debug
bool debug_flag = FALSE;
unsigned long int pause_addr = -1;


// void DEBUG(char* p1, ...)
// {
// 	va_list va;
//     va_start(va, p1); // init va, pointing to the first argument
//     int tmp,smax=-1;
//     int i;
//     for(i = 0;i<n;i++)
//         {
//                 tmp=va_arg(va,int); // get the next argument, the type is int
//                 if(tmp>smax) smax=tmp;
//         }
//         va_end(va);
// }
void DEBUG_MESSAGE(char* message)
{
	printf("%s, I showed here!\n", message);
}
void DEBUG_INST(char* inst_name, char* type, Riscv64_decoder* riscv_decoder, Riscv64_register* riscv_register)
{
	int d_rd = riscv_decoder->rd;
	int d_rs1 = riscv_decoder->rs1;
	int d_rs2 = riscv_decoder->rs2;
	int d_imm = riscv_decoder->immediate;
	int d_shamt = riscv_decoder->shamt64;
	reg64 d_reg_rd = riscv_register->x[d_rd];
	reg64 d_reg_rs1 = riscv_register->x[d_rs1];
	reg64 d_reg_rs2 = riscv_register->x[d_rs2];

	printf("%s:", inst_name);
	if(!strcmp(type, "di"))
	{
		printf("reg[rd] = %lu, imm = %d", d_reg_rd, d_imm);
	}
	else if(!strcmp(type, "d1i"))
	{
		printf("reg[rd] = %lu, reg[rs1] = %lu, imm = %d", d_reg_rd, d_reg_rs1, d_imm);
	}
	else if(!strcmp(type, "12i"))
	{
		printf("reg[rs1] = %lu, reg[rs2] = %lu, imm = %d", d_reg_rs1, d_reg_rs2, d_imm);
	}
	else if(!strcmp(type, "d12"))
	{
		printf("reg[rd] = %lu, reg[rs1] = %lu, reg[rs2] = %lu", d_reg_rd, d_reg_rs1, d_reg_rs2);
	}
	else if(!strcmp(type, "d1s"))
	{
		printf("reg[rd] = %lu, reg[rs1] = %lu, shamt = %d", d_reg_rd, d_reg_rs1, d_shamt);
	}
	printf("\n");
}

void MY_DEBUG_MODE(Riscv64_register* riscv_register, Riscv64_memory* riscv_memory)
{
	char command[30];

	while(1)
	{
		printf("Please enter command (enter 'help' for help):\n");
		scanf("%s", command);
		if(strcmp(command, "help") == 0)
		{
			printf("exit:  exit the program immediately.\n");
			printf("rtn:   continue to run the program till end.\n");
			printf("r:     run to the next break point\n");
			printf("n:     run next instruction.\n");
			printf("reg:   print out all general register.\n");
		}
		// exit program directly
		else if(strcmp(command, "exit") == 0)
		{
			exit(0);
		}
		// run to next pause_addr
		else if(strcmp(command, "r") == 0)
		{
			debug_flag = FALSE;
			break;
		}
		// run till end
		else if(strcmp(command, "rtn") == 0)
		{
			debug_flag = FALSE;
			pause_addr = -1;
			break;
		}
		// next instruction
		else if(strcmp(command, "n") == 0)
		{
			break;
		}
		else if(strcmp(command, "reg") == 0)
		{
			DEBUG_SHOW_REG_GENERAL(riscv_register);
		}
		else
		{
			printf("invalid command!\n");
		}
	}
}

void DEBUG_SHOW_REG_GENERAL(Riscv64_register* riscv_register)
{
	printf("\n");
	printf("   name           value\n");
	printf("x[0]  zero       %x\n", riscv_register->x[0]);
	printf("x[1]  ra         %x\n", riscv_register->x[1]);
	printf("x[2]  sp         %x\n", riscv_register->x[2]);
	printf("x[3]  gp         %x\n", riscv_register->x[3]);
	printf("x[4]  tp         %x\n", riscv_register->x[4]);
	printf("x[5]  t0         %x\n", riscv_register->x[5]);
	printf("x[6]  t1         %x\n", riscv_register->x[6]);
	printf("x[7]  t2         %x\n", riscv_register->x[7]);
	printf("x[8]  s0/fp      %x\n", riscv_register->x[8]);
	printf("x[9]  s1         %x\n", riscv_register->x[9]);
	printf("x[10] a0         %x\n", riscv_register->x[10]);
	printf("x[11] a1         %x\n", riscv_register->x[11]);
	printf("x[12] a2         %x\n", riscv_register->x[12]);
	printf("x[13] a3         %x\n", riscv_register->x[13]);
	printf("x[14] a4         %x\n", riscv_register->x[14]);
	printf("x[15] a5         %x\n", riscv_register->x[15]);
	printf("x[16] a6         %x\n", riscv_register->x[16]);
	printf("x[17] a7         %x\n", riscv_register->x[17]);
	printf("x[18] s2         %x\n", riscv_register->x[18]);
	printf("x[19] s3         %x\n", riscv_register->x[19]);
	printf("x[20] s4         %x\n", riscv_register->x[20]);
	printf("x[21] s5         %x\n", riscv_register->x[21]);
	printf("x[22] s6         %x\n", riscv_register->x[22]);
	printf("x[23] s7         %x\n", riscv_register->x[23]);
	printf("x[24] s8         %x\n", riscv_register->x[24]);
	printf("x[25] s9         %x\n", riscv_register->x[25]);
	printf("x[26] s10        %x\n", riscv_register->x[26]);
	printf("x[27] s11        %x\n", riscv_register->x[27]);
	printf("x[28] t3         %x\n", riscv_register->x[28]);
	printf("x[29] t4         %x\n", riscv_register->x[29]);
	printf("x[30] t5         %x\n", riscv_register->x[30]);
	printf("x[31] t6         %x\n", riscv_register->x[31]);
	printf("\n");
}
