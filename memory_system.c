#include "memory_system.h"

#define CACHE_LEVEL 3
// somthing for debug
extern bool debug_flag;
extern unsigned long int pause_addr;

Memory m;
Cache l[CACHE_LEVEL + 1];
int sim_time, sim_hit;

/*********************************************/
/*                                           */
/* initialization and gc                     */
/*                                           */
/*********************************************/
/* XJM modified init functions below, and    */
/* now they don't have to return a pointer   */
/*********************************************/
// Riscv64_decoder* init_decoder(Riscv64_decoder* riscv_decoder)
void init_decoder(Riscv64_decoder** riscv_decoder)
{
	*riscv_decoder = (Riscv64_decoder*) malloc (sizeof(Riscv64_decoder));
	memset(*riscv_decoder, 0, sizeof(riscv_decoder));
}

// Riscv64_register* init_register(Riscv64_register* riscv_register)
void init_register(Riscv64_register** riscv_register, Riscv64_memory* riscv_memory)
{
	// set all registers 0
	*riscv_register = (Riscv64_register*) malloc (sizeof(Riscv64_register));
	memset(*riscv_register, 0, sizeof(Riscv64_register));
	(*riscv_register)->sp = get_virtual_addr(riscv_memory, (reg64)riscv_memory->stack_bottom); // set sp
}

// Riscv64_memory* init_memory(Riscv64_memory* riscv_memory)
void init_memory(Riscv64_memory** riscv_memory)
{
	*riscv_memory = (Riscv64_memory*) malloc (sizeof(Riscv64_memory));
	(*riscv_memory)->mem_size = MEM_SIZE;
	(*riscv_memory)->memory = (byte*) malloc (sizeof(byte) * (*riscv_memory)->mem_size);
	(*riscv_memory)->stack_bottom = get_actual_addr((*riscv_memory), (byte*)STACK_BOTTOM);
}

void init_cache_simulator()
{
	CacheConfig cache_config[CACHE_LEVEL + 1];
	StorageStats storage_stats;
    StorageLatency latency_m, latency_c[CACHE_LEVEL + 1];

	storage_stats.access_counter = 0;
    storage_stats.miss_num = 0;
    storage_stats.access_time = 0;
    storage_stats.replace_num = 0;
    storage_stats.fetch_num = 0;
    storage_stats.prefetch_num = 0;

    latency_m.bus_latency = 6;
    latency_m.hit_latency = 100;

    int i;
    for(i = 1; i <= CACHE_LEVEL; i++)
    {
        latency_c[i].bus_latency = 3;
        latency_c[i].hit_latency = 10;
    }
	latency_c[1].bus_latency = 2;
	latency_c[2].bus_latency = 2;
	latency_c[3].bus_latency = 4;

	latency_c[1].hit_latency = 4;
	latency_c[2].hit_latency = 5;
	latency_c[3].hit_latency = 11;

    for(i = 1; i <= CACHE_LEVEL; i++)
    {
        cache_config[i].size = 32;
        cache_config[i].associativity = 8;
        cache_config[i].write_through = 0;
        cache_config[i].write_allocate = 0;
        cache_config[i].blocksize = 64;
    }
	for(i = 1; i <= CACHE_LEVEL; i++)
    {
        cache_config[i].write_through = 0;
        cache_config[i].write_allocate = 0;
    }
	cache_config[1].size = 32;
	cache_config[2].size = 256;
	cache_config[3].size = 8 * 1024;

	cache_config[1].associativity = 8;
	cache_config[2].associativity = 8;
	cache_config[3].associativity = 8;

	cache_config[1].blocksize = 64;
	cache_config[2].blocksize = 64;
	cache_config[3].blocksize = 64;


	int levelNum = CACHE_LEVEL;
	for(i = 1; i < levelNum; i++)
        l[i].SetLower(&l[i + 1]);
    l[levelNum].SetLower(&m);

    // set storageStatus and Configulation of memory and cache
    m.SetStats(storage_stats);
    m.BuildMemory();
    for(i = 1; i <= levelNum; i++)
    {
        l[i].SetStats(storage_stats);
        l[i].SetConfig(cache_config[i]);
        l[i].BuildCache();
    }

    // set latency of memory and cache
    m.SetLatency(latency_m);

    for(i = 1; i <= levelNum; i++)
    {
        l[i].SetLatency(latency_c[i]);
    }

}

void cachesimulator_output()
{
	// StorageStats s;
    // l[1].GetStats(s);
    // printf("Total L1 access cycle: %d\n", s.access_time);
    // m.GetStats(s);
    // printf("Total Memory access cycle: %d\n", s.access_time);
	int i;
	for(i = 1; i <= CACHE_LEVEL; i++)
	{
		printf("-----------CACHE LEVEL %d----------------\n", i);
        l[i].OutputStorage();
        printf("\n");
	}
    // printf("Total number of instructions: %d\n", inst_num);
    // printf("Total number of missed instructions: %d\n", miss_num);
    // printf("Miss rate: %f\n", (float)miss_num/(float)inst_num );
}

void delete_memory_system(Riscv64_decoder* riscv_decoder, Riscv64_register* riscv_register, Riscv64_memory* riscv_memory)
{
	free(riscv_decoder);
	free(riscv_register);
	free(riscv_memory->memory);
	free(riscv_memory);
}


/*********************************************/
/*                                           */
/* functions for memory                      */
/*                                           */
/*********************************************/

byte* get_actual_addr(Riscv64_memory* riscv_memory, byte* virtual_addr)
{
	return riscv_memory->memory + (unsigned long int)virtual_addr;
}

byte* get_virtual_addr(Riscv64_memory* riscv_memory, byte* actual_addr)
{
	return (byte*)((unsigned long int)actual_addr - (unsigned long int)riscv_memory->memory);
}

bool out_of_memory_virtual(Riscv64_memory* riscv_memory, byte* virtual_addr)
{
	return (unsigned long int)virtual_addr > riscv_memory->mem_size ? TRUE : FALSE;
}

bool out_of_memory_actual(Riscv64_memory* riscv_memory, byte* actual_addr)
{
	byte* memory_addr_0 = riscv_memory->memory;
	if((unsigned long int)actual_addr < (unsigned long int)memory_addr_0 )
		return FALSE;
	if((unsigned long int)actual_addr - (unsigned long int)actual_addr > riscv_memory->mem_size)
		return FALSE;
	return TRUE;
}

void check_valid_memory_virtual(Riscv64_memory* riscv_memory, byte* virtual_addr)
{
	if(out_of_memory_virtual(riscv_memory, virtual_addr))
	{
		printf("Out of memory!\n");
		exit(0);
	}
	return;
}



void  set_memory_reg8(Riscv64_memory* riscv_memory, byte* virtual_addr, reg8 value)
{
	check_valid_memory_virtual(riscv_memory, virtual_addr);
	byte* actual_addr = get_actual_addr(riscv_memory, virtual_addr);
	*(reg8*)actual_addr = value;
	char* tmp_content, *tmp_block;
	l[1].HandleRequest((uint64_t)actual_addr, sizeof(reg8), 0, tmp_content, sim_hit, sim_time, tmp_block);
}
reg8 get_memory_reg8(Riscv64_memory* riscv_memory, byte* virtual_addr)
{
	check_valid_memory_virtual(riscv_memory, virtual_addr);
	byte* actual_addr = get_actual_addr(riscv_memory, virtual_addr);

	char* tmp_content, *tmp_block;
	l[1].HandleRequest((uint64_t)actual_addr, sizeof(reg8), 1, tmp_content, sim_hit, sim_time, tmp_block);
	return *(reg8*)actual_addr;
}
void  set_memory_reg16(Riscv64_memory* riscv_memory, byte* virtual_addr, reg16 value)
{
	check_valid_memory_virtual(riscv_memory, virtual_addr);
	byte* actual_addr = get_actual_addr(riscv_memory, virtual_addr);
	*(reg16*)actual_addr = value;
	char* tmp_content, *tmp_block;
	l[1].HandleRequest((uint64_t)actual_addr, sizeof(reg16), 0, tmp_content, sim_hit, sim_time, tmp_block);
}
reg16 get_memory_reg16(Riscv64_memory* riscv_memory, byte* virtual_addr)
{
	check_valid_memory_virtual(riscv_memory, virtual_addr);
	byte* actual_addr = get_actual_addr(riscv_memory, virtual_addr);
	char* tmp_content, *tmp_block;
	l[1].HandleRequest((uint64_t)actual_addr, sizeof(reg16), 1, tmp_content, sim_hit, sim_time, tmp_block);
	return *(reg16*)actual_addr;
}
void  set_memory_reg32(Riscv64_memory* riscv_memory, byte* virtual_addr, reg32 value)
{
	check_valid_memory_virtual(riscv_memory, virtual_addr);
	byte* actual_addr = get_actual_addr(riscv_memory, virtual_addr);
	*(reg32*)actual_addr = value;
	char* tmp_content, *tmp_block;
	l[1].HandleRequest((uint64_t)actual_addr, sizeof(reg32), 0, tmp_content, sim_hit, sim_time, tmp_block);
}
reg32 get_memory_reg32(Riscv64_memory* riscv_memory, byte* virtual_addr)
{
	check_valid_memory_virtual(riscv_memory, virtual_addr);
	byte* actual_addr = get_actual_addr(riscv_memory, virtual_addr);
	char* tmp_content, *tmp_block;
	l[1].HandleRequest((uint64_t)actual_addr, sizeof(reg32), 1, tmp_content, sim_hit, sim_time, tmp_block);
	return *(reg32*)actual_addr;
}
void  set_memory_reg64(Riscv64_memory* riscv_memory, byte* virtual_addr, reg64 value)
{
	check_valid_memory_virtual(riscv_memory, virtual_addr);
	byte* actual_addr = get_actual_addr(riscv_memory, virtual_addr);
	*(reg64*)actual_addr = value;
	char* tmp_content, *tmp_block;
	l[1].HandleRequest((uint64_t)actual_addr, sizeof(reg64), 0, tmp_content, sim_hit, sim_time, tmp_block);
}
reg64 get_memory_reg64(Riscv64_memory* riscv_memory, byte* virtual_addr)
{
	check_valid_memory_virtual(riscv_memory, virtual_addr);
	byte* actual_addr = get_actual_addr(riscv_memory, virtual_addr);
	char* tmp_content, *tmp_block;
	l[1].HandleRequest((uint64_t)actual_addr, sizeof(reg64), 1, tmp_content, sim_hit, sim_time, tmp_block);
	return *(reg64*)actual_addr;
}


/*********************************************/
/*                                           */
/* functions for register file               */
/*                                           */
/*********************************************/

// integer
void set_register_pc(Riscv64_register* riscv_register, reg64 value)
{
	riscv_register->pc = value;
}

reg64 get_register_pc(Riscv64_register* riscv_register)
{
	return riscv_register->pc;
}


void set_register_general(Riscv64_register* riscv_register, int index, reg64 value)
{
	// can't set x[0], because x[0] always equals to zero
	if(index == 0)
	{
		printf("Error: can't set x[0] to be 0.\n");
	}
	else if(index < 0 || index > 31)
	{
		printf("Error: register index out of range.\n");
	}

	riscv_register->x[index] = value;
}

reg64 get_register_general(Riscv64_register* riscv_register , int index)
{
	return riscv_register->x[index];
}


void register_pc_self_increase(Riscv64_register* riscv_register)
{
	riscv_register->pc += sizeof(instruction);
}

// floating-point
void set_register_fcsr(Riscv64_register* riscv_register, reg64 value)
{
	riscv_register->fcsr = value;
}

reg64 get_register_fcsr(Riscv64_register* riscv_register)
{
	return riscv_register->fcsr;
}

void set_register_fp(Riscv64_register* riscv_register, int index, reg64 value)
{
	if(index < 0 || index > 31)
	{
		printf("Error: register index out of range.\n");
	}
	riscv_register->f[index] = value;
}

reg64 get_register_fp(Riscv64_register* riscv_register, int index)
{
	return riscv_register->f[index];
}
