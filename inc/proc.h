#ifndef PROC_H
#define PROC_H

#define MODE_DEFAULT 0
#define MODE_I 1
#define MODE_O 2
#define MODE_T 3

typedef struct {
    char name[32];
    int start_line;          
    int params_count;
    struct {
        int mode;            // MODE_I, MODE_O, MODE_T lub MODE_DEFAULT
        long long addr;      // Adres komórki przechowującej referencję
        char param_name[32]; // Nazwa parametru (do sprawdzania zakresu wewnątrz)
    } params[20];
} Procedure;

typedef struct {
    int count;
    struct {
        long long addr;
        int mode;            
        int is_array;
    } args[20];
} ListOfArgs;

void handle_proc_decl(int mode, char* name);
void handle_proc_call(char* proc_name, ListOfArgs* args);
void handle_proc_end(ListOfArgs* args);

#endif