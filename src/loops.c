#include "../inc/utils.h"
#include "../inc/reg.h"
#include "../inc/cond.h"
#include "../inc/mem_mng.h"
#include "stdlib.h"

void handle_RU(int loop_start_line) {
    generate_instruction(INSTR_JZERO, loop_start_line);
}

void handle_while(int cond_line, int patch_line) {
    generate_instruction(INSTR_JUMP, cond_line);
    code_buffer[patch_line].arg = code_ptr;
}

int handle_up_for(long long it_addr, long long limit_addr) {
    long long res_addr = handle_le(it_addr, limit_addr);
    generate_instruction(INSTR_LOAD, res_addr);

    int jump_to_end_line = code_ptr;
    generate_instruction(INSTR_JZERO, 0);

    return jump_to_end_line;
}

int handle_endfor_up(int patch_line, long long it_addr) {
    generate_instruction(INSTR_LOAD, it_addr);
    generate_instruction(INSTR_INC, 0);
    generate_instruction(INSTR_STORE, it_addr);

    int jump_to_cond = code_ptr;
    generate_instruction(INSTR_JUMP, 0);

    code_buffer[patch_line].arg = code_ptr;

    symbols_table[it_addr - GLOBAL_ADDR_START].type = T_DEAD;

    return jump_to_cond;
}

int handle_down_for(long long it_addr, long long limit_addr) {
    long long res_addr = handle_ge(it_addr, limit_addr);
    generate_instruction(INSTR_LOAD, res_addr);

    int jump_to_end_line = code_ptr;
    generate_instruction(INSTR_JZERO, 0);

    return jump_to_end_line;
}

int handle_endfor_down(int patch_line, long long it_addr) {
    generate_instruction(INSTR_LOAD, it_addr);
    generate_instruction(INSTR_DEC, 0);
    generate_instruction(INSTR_STORE, it_addr);

    int jump_to_cond = code_ptr;
    generate_instruction(INSTR_JUMP, 0);

    code_buffer[patch_line].arg = code_ptr;

    symbols_table[it_addr - GLOBAL_ADDR_START].type = T_DEAD;

    return jump_to_cond;
}

long long handle_iterator(const char* name, long long start_val_addr) {

    SymbolEntry* it = find_symbol_entry(name);
    if(it != NULL) { fprintf(stderr, "Błąd: Ta zmienna nie może być iteratorem.\n "); exit(1);}
    if (symbol_count >= MAX_SYMBOLS) {
        fprintf(stderr, "Błąd kompilacji: Przepełnienie tablicy symboli.\n");
        exit(1);
    }

    long long it_addr = global_memory_ptr;
    global_memory_ptr++;

    SymbolEntry *new_entry = &symbols_table[symbol_count];
    strncpy(new_entry->name, name, 31);
    new_entry->name[31] = '\0';
    new_entry->address = it_addr;
    new_entry->type = T_ITERATOR;
    
    symbol_count++;

    load_value(start_val_addr);
    generate_instruction(INSTR_STORE, it_addr);

    return it_addr;
}
