#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>


// --- Instrukcje Maszyny Wirtualnej ---
typedef enum {
    INSTR_READ, INSTR_WRITE, INSTR_LOAD, INSTR_STORE, INSTR_RLOAD, INSTR_RSTORE,
    INSTR_ADD, INSTR_SUB, INSTR_SWP, INSTR_RST, INSTR_INC, INSTR_DEC,
    INSTR_SHL, INSTR_SHR, INSTR_JUMP, INSTR_JPOS, INSTR_JZERO,
    INSTR_CALL, INSTR_RTRN, INSTR_HALT
} OpCode;

typedef struct Instruction {
    OpCode op;
    long long arg;
} Instruction;

#define MAX_CODE_SIZE 1000
extern Instruction code_buffer[MAX_CODE_SIZE];
extern int code_ptr;

// --- Interfejs Funkcji Generujących Kod ---

void generate_instruction(OpCode op, long long arg);
void print_compiled_code();
void print_instruction(Instruction ins);
void handle_assign(long long addr, long long expr_result_addr);
void handle_read(long long addr);
void handle_write(long long value_addr);
void generate_num_val(long long num_val, long long addr);
long long handle_num(long long num_val);
long long handle_id(char* pidentifier);

void load_value(long long addr);
void store_value(long long addr);

#endif