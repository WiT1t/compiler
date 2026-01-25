#include "../inc/utils.h"
#include "../inc/reg.h"
#include "../inc/cond.h"
#include "../inc/mem_mng.h"
#include "stdlib.h"

long long handle_eq(long long val1_addr, long long val2_addr) {
    // A: val1_addr, B: val2_addr. Sprawdzamy, czy A == B.
    
    int reg_X = get_free_register(); // Dla B

    if (reg_X == -1) return 0;
    
// Sprawdź A > B (tj. A - B > 0) ---
    // Oblicz A - B.
    load_value(val2_addr);
    generate_instruction(INSTR_SWP, reg_X); // rX <- B
    load_value(val1_addr); // ra <- A
    generate_instruction(INSTR_SUB, reg_X);      // ra <- A - B
    
    // Jeśli ra > 0 (JPOS), to A > B, czyli A != B. SKOCZ DO FAŁSZU.
    int jpos_to_false_1 = code_ptr;
    generate_instruction(INSTR_JPOS, 0);

// Sprawdź B > A (tj. B - A > 0) ---
    // Oblicz B - A.
    load_value(val1_addr);
    generate_instruction(INSTR_SWP, reg_X);
    load_value(val2_addr); // ra <- B
    generate_instruction(INSTR_SUB, reg_X);      // ra <- B - A
    
    // Jeśli ra > 0 (JPOS), to B > A, czyli A != B. SKOCZ DO FAŁSZU.
    int jpos_to_false_2 = code_ptr;
    generate_instruction(INSTR_JPOS, 0);
    
// PRAWDA (Jeśli oba warunki są <= 0, to A = B)
    generate_instruction(INSTR_RST, 0); 
    generate_instruction(INSTR_INC, 0); // ra <- 1
    int jump_to_end = code_ptr;
    generate_instruction(INSTR_JUMP, 0); 
    
// L_FALSE
    int label_false = code_ptr;
    code_buffer[jpos_to_false_1].arg = label_false;
    code_buffer[jpos_to_false_2].arg = label_false;
    generate_instruction(INSTR_RST, 0); // ra <- 0
    
// L_END
    int label_end = code_ptr;
    code_buffer[jump_to_end].arg = label_end;
    
    generate_instruction(INSTR_STORE, BOOL_ADDR); 
    release_register(reg_X);
    return BOOL_ADDR;
}


long long handle_neq(long long val1_addr, long long val2_addr) {
    // A: val1_addr, B: val2_addr. Sprawdzamy, czy A != B.
    
    int reg_X = get_free_register(); // Dla B

    if (reg_X == -1) return 0;
    
// Sprawdź A > B (tj. A - B > 0) ---
    // Oblicz A - B.
    load_value(val2_addr);
    generate_instruction(INSTR_SWP, reg_X); // rX <- B
    load_value(val1_addr); // ra <- A
    generate_instruction(INSTR_SUB, reg_X);      // ra <- A - B
    
    // Jeśli ra > 0 (JPOS), to A > B. SKOCZ DO PRAWDY.
    int jpos_to_true_1 = code_ptr;
    generate_instruction(INSTR_JPOS, 0); 

// Sprawdź B > A (tj. B - A > 0) ---
    // Oblicz B - A.
    load_value(val1_addr);
    generate_instruction(INSTR_SWP, reg_X);
    load_value(val2_addr); // ra <- B
    generate_instruction(INSTR_SUB, reg_X);      // ra <- B - A
    
    // Jeśli ra > 0 (JPOS), to B > A. SKOCZ DO PRAWDY.
    int jpos_to_true_2 = code_ptr;
    generate_instruction(INSTR_JPOS, 0);
    
// FAŁSZ (Jeśli oba warunki są <= 0, to A = B)
    generate_instruction(INSTR_RST, 0); // ra <- 0
    int jump_to_end = code_ptr;
    generate_instruction(INSTR_JUMP, 0); 
    
// L_TRUE
    int label_true = code_ptr;
    code_buffer[jpos_to_true_1].arg = label_true;
    code_buffer[jpos_to_true_2].arg = label_true;
    generate_instruction(INSTR_RST, 0);
    generate_instruction(INSTR_INC, 0); // ra <- 1
    
//L_END
    int label_end = code_ptr;
    code_buffer[jump_to_end].arg = label_end;
    
    generate_instruction(INSTR_STORE, BOOL_ADDR); 
    release_register(reg_X);
    return BOOL_ADDR;
}

long long handle_g(long long val1_addr, long long val2_addr) {
    // A: val1_addr, B: val2_addr. Sprawdzamy, czy A > B.
    // A > B jest PRAWDZIWE tylko, jeśli A - B > 0.
    
    int reg_X = get_free_register();

    if (reg_X == -1) return 0;
    
// Oblicz A - B (A do ra, B do rX)
    load_value(val2_addr);
    generate_instruction(INSTR_SWP, reg_X); // rX <- B
    load_value(val1_addr); // ra <- A
    generate_instruction(INSTR_SUB, reg_X);      // ra <- A - B

// JZERO L_FALSE
    // Jeśli ra == 0 (JPOS), to A <= B. Skocz do FAŁSZU.
    int jpos_to_false = code_ptr;
    generate_instruction(INSTR_JZERO, 0); 

// L_TRUE
    generate_instruction(INSTR_RST, 0);
    generate_instruction(INSTR_INC, 0);
    int jump_to_end = code_ptr;
    generate_instruction(INSTR_JUMP, 0); 
    
// L_FALSE
    int label_false = code_ptr;
    code_buffer[jpos_to_false].arg = label_false;
    generate_instruction(INSTR_RST, 0); // ra <- 0 (FAŁSZ)
    
// L_END
    int label_end = code_ptr;
    code_buffer[jump_to_end].arg = label_end;
    
    generate_instruction(INSTR_STORE, BOOL_ADDR); 
    release_register(reg_X);
    return BOOL_ADDR;
}

long long handle_l(long long val1_addr, long long val2_addr) {
    // A: val1_addr, B: val2_addr. Sprawdzamy, czy A < B.
    // A < B jest PRAWDZIWE tylko, jeśli B - A > 0.
    
    int reg_X = get_free_register();

    if (reg_X == -1) return 0;
    
// Oblicz B - A (B do ra, A do rX)
    load_value(val1_addr);
    generate_instruction(INSTR_SWP, reg_X); // rX <- A
    load_value(val2_addr); // ra <- B
    generate_instruction(INSTR_SUB, reg_X);      // ra <- B - A

// JZERO L_FALSE
    // Jeśli ra == 0 (JPOS), to B <= A. Skocz do FAŁSZU.
    int jpos_to_false = code_ptr;
    generate_instruction(INSTR_JZERO, 0); 

// L_TRUE 
    generate_instruction(INSTR_RST, 0);
    generate_instruction(INSTR_INC, 0);
    int jump_to_end = code_ptr;
    generate_instruction(INSTR_JUMP, 0); 
    
// L_FALSE
    int label_false = code_ptr;
    code_buffer[jpos_to_false].arg = label_false;
    generate_instruction(INSTR_RST, 0); // ra <- 0 (FAŁSZ)
    
// L_END
    int label_end = code_ptr;
    code_buffer[jump_to_end].arg = label_end;
    
    generate_instruction(INSTR_STORE, BOOL_ADDR); 
    release_register(reg_X);
    return BOOL_ADDR;
}

long long handle_ge(long long val1_addr, long long val2_addr) {
    // A: val1_addr, B: val2_addr. Sprawdzamy, czy A >= B.
    // A >= B jest FAŁSZYWE tylko, jeśli A < B (tj. B - A > 0).
    
    int reg_X = get_free_register();

    if (reg_X == -1) return 0;
    
// Oblicz B - A (B do ra, A do rX)
    load_value(val1_addr);
    generate_instruction(INSTR_SWP, reg_X); // rX <- A
    load_value(val2_addr); // ra <- B
    generate_instruction(INSTR_SUB, reg_X);      // ra <- B - A

// JPOS L_FALSE
    // Jeśli ra > 0 (JPOS), to A < B. Skocz do FAŁSZU.
    int jpos_to_false = code_ptr;
    generate_instruction(INSTR_JPOS, 0); 

// L_TRUE
    // Ustaw ra = 1
    generate_instruction(INSTR_RST, 0);
    generate_instruction(INSTR_INC, 0);
    int jump_to_end = code_ptr;
    generate_instruction(INSTR_JUMP, 0); 
    
// L_FALSE
    int label_false = code_ptr;
    code_buffer[jpos_to_false].arg = label_false;
    generate_instruction(INSTR_RST, 0); // ra <- 0 (FAŁSZ)
    
// L_END
    int label_end = code_ptr;
    code_buffer[jump_to_end].arg = label_end;
    
    generate_instruction(INSTR_STORE, BOOL_ADDR); 
    release_register(reg_X);
    return BOOL_ADDR;
}

long long handle_le(long long val1_addr, long long val2_addr) {
    // A: val1_addr, B: val2_addr. Sprawdzamy, czy A <= B.
    // A <= B jest FAŁSZYWE tylko, jeśli A > B (tj. A - B > 0).
    
    int reg_X = get_free_register();

    if (reg_X == -1) return 0;
    
// Oblicz A - B (A do ra, B do rX)
    load_value(val2_addr);
    generate_instruction(INSTR_SWP, reg_X); // rX <- B
    load_value(val1_addr); // ra <- A
    generate_instruction(INSTR_SUB, reg_X);      // ra <- A - B

// JPOS L_FALSE
    // Jeśli ra > 0 (JPOS), to A > B. Skocz do FAŁSZU.
    int jpos_to_false = code_ptr;
    generate_instruction(INSTR_JPOS, 0); 

// L_TRUE 
    // ra <- 1
    generate_instruction(INSTR_RST, 0); 
    generate_instruction(INSTR_INC, 0); 
    int jump_to_end = code_ptr;
    generate_instruction(INSTR_JUMP, 0); 
    
// L_FALSE
    int label_false = code_ptr;
    code_buffer[jpos_to_false].arg = label_false;
    generate_instruction(INSTR_RST, 0); // ra <- 0 (FAŁSZ)
    
// L_END
    int label_end = code_ptr;
    code_buffer[jump_to_end].arg = label_end;
    
    generate_instruction(INSTR_STORE, BOOL_ADDR); 
    release_register(reg_X);
    return BOOL_ADDR;
}