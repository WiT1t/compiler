#include "../inc/utils.h"
#include "../inc/reg.h"
#include "../inc/cond.h"
#include "../inc/ifels.h"
#include "stdlib.h"

int handle_if(long long cond_result_addr) {
    
    //ra <- flaga oceny warunku
    generate_instruction(INSTR_LOAD, cond_result_addr);

    //skok do else / endif
    int jump_line = code_ptr;
    generate_instruction(INSTR_JZERO, 0);

    return jump_line;
}

int handle_else(int jump_line) {
    int jump_to_endif_line = code_ptr;
    generate_instruction(INSTR_JUMP, 0);
    
    int label_else = code_ptr;
    code_buffer[jump_line].arg = label_else;

    return jump_to_endif_line;
}

void handle_endif(int jump_line) {
    int label_endif = code_ptr;
    
    code_buffer[jump_line].arg = label_endif;
}