OBJECTS = memory_system.o riscv_instruction.o execute.o debug.o memory.o cache.o
COMPILEFLAGS = -std=c99 -lm -fno-stack-protector -fpermissive
CC = g++
simulator : $(OBJECTS)
	$(CC) -o simulator $(OBJECTS) $(COMPILEFLAGS)

memory_system.o : memory_system.c memory_system.h
	$(CC) -c memory_system.c $(COMPILEFLAGS)
riscv_instruction.o : riscv_instruction.c riscv_instruction.h
	$(CC) -c riscv_instruction.c $(COMPILEFLAGS)
execute.o : execute.c execute.h
	$(CC) -c execute.c $(COMPILEFLAGS)
debug.o : debug.c debug.h
	$(CC) -c debug.c $(COMPILEFLAGS)
cache.o: cache.h def.h
memory.o: memory.h

clean :
	    rm simulator $(OBJECTS)
