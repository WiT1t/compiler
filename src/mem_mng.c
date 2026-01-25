#include "../inc/reg.h"
#include "../inc/utils.h"
#include "../inc/mem_mng.h"
#include "stdio.h"
#include <string.h>
#include "stdlib.h"

ConstantEntry constant_table[MAX_CONSTANTS];
int constant_count = 0;
long long const_free_addr_ptr = CONST_VAL_START;
long long const_one_addr = -1;

SymbolEntry symbols_table[MAX_SYMBOLS];
int symbol_count = 0;
long long global_memory_ptr = GLOBAL_ADDR_START;

long long free_temp_addresses[MAX_TEMP_MEMORY];
int free_temp_stack_pointer = 0;
long long temporary_memory_ptr = TEMP_ADDR_START;

ArrVar arr_addresses[MAX_ARR_MEMORY];
int arr_var_count = 0;
long long arr_mem_pointer = ARR_ADDR_START;

void initTMP() {
    for(int i=0; i<MAX_TEMP_MEMORY; i++) {
        free_temp_addresses[i] = TEMP_ADDR_START + i;
    }
    free_temp_stack_pointer = MAX_TEMP_MEMORY;
}

long long get_temporary_address() {
    long long temp_addr;

    if(free_temp_stack_pointer > 0) {
        free_temp_stack_pointer--;
        temp_addr = free_temp_addresses[free_temp_stack_pointer];
    } else {
        fprintf(stderr, "Błąd: Brak pamięci tymczasowej.\n");
        exit(1);
    }

    return temp_addr;
}

void free_temporary_address(long long addr) {
    if(TEMP_ADDR_START > addr || addr >= TEMP_ADDR_START + MAX_TEMP_MEMORY) {
        fprintf(stderr, "Błąd: Nieprawidłowy adres tymczasowy %lld.\n", addr);
        exit(1);
    }
    free_temp_addresses[free_temp_stack_pointer] = addr;
    free_temp_stack_pointer++;
}


long long reserve_const_address(long long num_val) {
    for (int i = 0; i < constant_count; i++) {
        if (constant_table[i].value == num_val) {
            return constant_table[i].address; // Stała już jest, zwróć istniejący adres.
        }
    }

    if (constant_count >= MAX_CONSTANTS) {
        // Obsługa błędu przepełnienia tablicy stałych
        fprintf(stderr, "Błąd: Przepełnienie tablicy stałych.\n");
        exit(1);
    }

    long long new_address = const_free_addr_ptr;
    
    constant_table[constant_count].value = num_val;
    constant_table[constant_count].address = new_address;
    constant_count++;
    
    const_free_addr_ptr++;

    generate_num_val(num_val, new_address);
    return new_address;
}

// Funkcja pomocnicza do wyszukiwania symbolu
SymbolEntry* find_symbol_entry(const char *name) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbols_table[i].name, name) == 0 && symbols_table[i].type != T_DEAD) {
            return &symbols_table[i];
        }
    }
    return NULL;
}

long long add_symbol(char* id_name) {
    if (symbol_count >= MAX_SYMBOLS) {
        fprintf(stderr, "Błąd kompilacji: Przepełnienie tablicy symboli.\n");
        exit(1);
    }
    
    long long new_address = global_memory_ptr;
    global_memory_ptr++;

    SymbolEntry *new_entry = &symbols_table[symbol_count];
    strncpy(new_entry->name, id_name, 31);
    new_entry->name[31] = '\0';
    new_entry->address = new_address;
    new_entry->type = T_STANDARD;
    
    symbol_count++;

    return new_address;
}

long long add_arr(char* id_name, long long start_idx, long long end_idx) {
    if(arr_var_count  >= MAX_ARR_MEMORY) {
        fprintf(stderr, "Błąd kompilacji: Przepełnienie pamięci tablic.\n");
        exit(1);
    }
    if(end_idx < start_idx) {
        fprintf(stderr, "Błąd kompilacji: Nieprawidłowy zakres w tablicy.\n");
        exit(1);
    }

    long long new_address = arr_mem_pointer;
    arr_mem_pointer += end_idx - start_idx + 1;

    ArrVar *arr_var = &arr_addresses[arr_var_count];
    strncpy(arr_var->name, id_name, 31);
    arr_var->name[31] = '\0';
    arr_var->addr = new_address;
    arr_var->start_idx = start_idx;
    arr_var->end_idx = end_idx;

    arr_var_count++;

    return new_address;
}

int find_arr_idx(char* id_name) {
    for(int i = 0; i <= arr_var_count; i++) {
        if(strcmp(arr_addresses[i].name, id_name) == 0) {
            return i;
        }
    }

    return -1;
}

long long find_arr_member(char* id_name, long long idx) {
    for(int i = 0; i <= arr_var_count; i++) {
        if(strcmp(arr_addresses[i].name, id_name) == 0) {
            if(idx > arr_addresses[i].end_idx || idx < arr_addresses[i].start_idx) {
                fprintf(stderr, "Błąd kompilacji: Nieprawidłowy indeks tablicy: %s, %lld.\n", id_name, idx);
                exit(1);
            }
            long long rel_idx = idx - arr_addresses[i].start_idx;

            return arr_addresses[i].addr + rel_idx;
        }
    }

    return -1;
}

long long handle_arr_dynamic(char* arr_name, char* var_name) {
    int arr_idx = find_arr_idx(arr_name);
    if(arr_idx == -1) {
        fprintf(stderr, "Błąd kompilacji: Zmienna nie istnieje: %s.\n", arr_name);
        exit(1);
    }
    SymbolEntry* var = find_symbol_entry(var_name);
    if(var == NULL) {
        fprintf(stderr, "Błąd kompilacji: Zmienna nie istnieje: %s.\n", var_name);
        exit(1);
    }
    long long v_addr = var->address;
    long long s_idx_addr = reserve_const_address(arr_addresses[arr_idx].start_idx);
    long long ptr_addr = get_temporary_address();
    long long b_addr = reserve_const_address(arr_addresses[arr_idx].addr);
    // --- OBLICZANIE ADRESU ---
    int x = get_free_register();
    generate_instruction(INSTR_LOAD, s_idx_addr);
    generate_instruction(INSTR_SWP, x);
    generate_instruction(INSTR_LOAD, v_addr);
    generate_instruction(INSTR_SUB, x); // i - start_idx
    generate_instruction(INSTR_SWP, x);
    generate_instruction(INSTR_LOAD, b_addr);
    generate_instruction(INSTR_ADD, x);     // + base_addr
    
    generate_instruction(INSTR_STORE, ptr_addr);

    release_register(x);

    return ptr_addr;
}