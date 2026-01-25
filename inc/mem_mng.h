#ifndef MEM_MNG_H
#define MEM_MNG_H

//ZAREZERWOWANE MIESJCA W PAMIĘCI
#define RETURN_ADDR 0
#define RESULT_ADDR 1
#define BOOL_ADDR 2

//CONST
// stałę będą zapisywane w addresach 100 - 199
#define MAX_CONSTANTS 100
#define CONST_VAL_START 100

typedef struct {
    long long value;
    long long address;
} ConstantEntry;

extern ConstantEntry constant_table[MAX_CONSTANTS];
extern long long const_free_addr_ptr;
extern int constant_count;

extern long long const_one_addr;

//GLOB
//pamięć globalna w adresach 200-299
#define MAX_SYMBOLS 100
#define GLOBAL_ADDR_START 200

typedef enum {
    T_ITERATOR, T_STANDARD, T_DEAD
} VarType;

typedef struct {
    char name[32];
    long long address;
    char scope[32];
    int is_initialized;
    VarType type;
} SymbolEntry;

extern char curr_scope[32];
extern SymbolEntry symbols_table[MAX_SYMBOLS];
extern long long global_memory_ptr;
extern int symbol_count;

//TMP
// pamięć tymczasowa w adresach 300 - 399 // forlimits && tab[i] calls
#define MAX_TEMP_MEMORY 100
#define TEMP_ADDR_START 300
extern long long temporary_memory_ptr;
// Stos do przechowywania wolnych adresów, które można recyklingować
extern long long free_temp_addresses[MAX_TEMP_MEMORY];
extern int free_temp_stack_pointer;

//PARAMETRY FORMALNE
#define MAX_PARAM_MEMORY 100
#define PARAM_MEMORY_START 400

typedef enum {
    P_I, P_O, P_T
} ParamType;

typedef struct {
    ParamType type;
    long long addr;
    int is_initiated;
} FormalParam;

extern FormalParam param_table[MAX_PARAM_MEMORY];
extern long long param_memory_ptr;
extern int param_count;

//TABLICE
#define MAX_ARR_MEMORY 1000000
#define ARR_ADDR_START 500

typedef struct {
    char name[32];
    long long addr;
    long long start_idx;
    long long end_idx;
} ArrVar;

extern ArrVar arr_addresses[MAX_ARR_MEMORY];
extern long long arr_mem_pointer;
extern int arr_var_count;

void initTMP();
long long get_temporary_address();
void free_temporary_address(long long addr);
long long reserve_const_address(long long num_val);
SymbolEntry* find_symbol_entry(const char *name);
long long add_symbol(char* id_name);
long long add_arr(char* id_name, long long start_idx, long long end_idx);
int find_arr_idx(char* id_name);
long long find_arr_member(char* id_name, long long idx);
long long handle_arr_dynamic(char* id_name, char* var_name);

#endif