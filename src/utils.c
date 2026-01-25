#include "../inc/utils.h"
#include "../inc/mem_mng.h"
#include "limits.h"

Instruction code_buffer[MAX_CODE_SIZE];
int code_ptr = 0;

void generate_instruction(OpCode op, long long arg) {
    if (code_ptr < MAX_CODE_SIZE) {
        code_buffer[code_ptr].op = op;
        code_buffer[code_ptr].arg = arg;
        code_ptr++;
    } else {
        fprintf(stderr, "Błąd: Przekroczono maksymalną liczbę instrukcji.\n");
        exit(1);
    }
}

long long handle_num(long long num_val) {
    long long addr = reserve_const_address(num_val);

    return addr;
}

long long handle_id(char* id_name) {
    SymbolEntry *sym = find_symbol_entry(id_name);
    if(sym == NULL) {
        fprintf(stderr, "Błąd: zmienna nie istnieje: %s\n", id_name);
        exit(1);
    }
    long long addr = sym->address;

    return addr;
}

void handle_read(long long addr) {
    if(addr >= GLOBAL_ADDR_START && addr < GLOBAL_ADDR_START + MAX_SYMBOLS) {
        if(symbols_table[addr - GLOBAL_ADDR_START].type == T_ITERATOR) {
            fprintf(stderr, "Błąd: próba modyfikacji iteratora.\n");
            exit(1);
        }
    }

    generate_instruction(INSTR_READ, 0);
    store_value(addr);
}

void handle_write(long long value_addr) {
    load_value(value_addr);
    
    generate_instruction(INSTR_WRITE, 0);
}

void handle_assign(long long addr, long long expr_result_addr) {

    if(addr >= GLOBAL_ADDR_START && addr < GLOBAL_ADDR_START + MAX_SYMBOLS) {
        if(symbols_table[addr - GLOBAL_ADDR_START].type == T_ITERATOR) {
            fprintf(stderr, "Błąd: próba modyfikacji iteratora.\n");
            exit(1);
        } 
    }
    generate_instruction(INSTR_LOAD, expr_result_addr);
    store_value(addr);
}

void print_instruction(Instruction inst) {
    switch (inst.op)
    {
        case INSTR_READ:
            printf("READ\n");
            break;
        case INSTR_WRITE:
            printf("WRITE\n");
            break;
        case INSTR_LOAD:
            printf("LOAD %lld\n", inst.arg);
            break;
        case INSTR_STORE:
            printf("STORE %lld\n", inst.arg);
            break;
        case INSTR_RLOAD:
            printf("RLOAD %c\n", ('a' +inst.arg));
            break;
        case INSTR_RSTORE:
            printf("RSTORE %c\n", ('a' +inst.arg));
            break;
        case INSTR_ADD:
            printf("ADD %c\n", ('a' +inst.arg));
            break;
        case INSTR_SUB:
            printf("SUB %c\n", ('a' +inst.arg));
            break;
        case INSTR_SWP:
            printf("SWP %c\n", ('a' +inst.arg));
            break;
        case INSTR_RST:
            printf("RST %c\n", ('a' +inst.arg));
            break;
        case INSTR_INC:
            printf("INC %c\n", ('a' +inst.arg));
            break;
        case INSTR_DEC:
            printf("DEC %c\n", ('a' +inst.arg));
            break;
        case INSTR_SHL:
            printf("SHL %c\n", ('a' +inst.arg));
            break;
        case INSTR_SHR:
            printf("SHR %c\n", ('a' +inst.arg));
            break;
        case INSTR_JUMP:
            printf("JUMP %lld\n", inst.arg);
            break;
        case INSTR_JPOS:
            printf("JPOS %lld\n", inst.arg);
            break;
        case INSTR_JZERO:
            printf("JZERO %lld\n", inst.arg);
            break;
        case INSTR_CALL:
            printf("CALL %lld\n", inst.arg);
            break;
        case INSTR_RTRN:
            printf("RTRN\n");
            break;
        case INSTR_HALT:
            printf("HALT\n");
            break;
        default:
            break;
    }
}

void print_compiled_code() {
    int line_count = code_ptr;

    for (int i=0; i < line_count; i++) {
        print_instruction(code_buffer[i]);
    }
}

void generate_num_val(long long num_val, long long addr) {
    
    generate_instruction(INSTR_RST, 0);
    
    const int bit_num = sizeof(unsigned long long) * CHAR_BIT - __builtin_clzll(num_val)-1;
    for(int i= bit_num; i >= 0; i--) {
        int bit = (num_val >> i) & 1;

        generate_instruction(INSTR_SHL, 0);
        if(bit == 1) {
            generate_instruction(INSTR_INC, 0);
        }
    }

    generate_instruction(INSTR_STORE, addr);
}

void load_value(long long addr) {
    if(addr >= TEMP_ADDR_START && addr < TEMP_ADDR_START + MAX_TEMP_MEMORY) {
        generate_instruction(INSTR_LOAD, addr);
        generate_instruction(INSTR_RLOAD, 0);
    } else {
        generate_instruction(INSTR_LOAD, addr);
    }
}

//używamy ostatniego rejestru
void store_value(long long addr) {
    if(addr >= TEMP_ADDR_START && addr < TEMP_ADDR_START + MAX_TEMP_MEMORY) {
        generate_instruction(INSTR_SWP, 7);
        generate_instruction(INSTR_LOAD, addr);
        generate_instruction(INSTR_SWP, 7);
        generate_instruction(INSTR_RSTORE, 7);
    } else {
        generate_instruction(INSTR_STORE, addr);
    }
}