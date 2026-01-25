#include "../inc/utils.h"
#include "../inc/reg.h"
#include "../inc/arithm.h"
#include "../inc/mem_mng.h"
#include "stdlib.h"

int first_mult = 1;
int mult_start = -1;
int first_div = 1;
int div_start = -1;

long long handle_plus(long long val1_addr, long long val2_addr) {
    int reg_x = get_free_register(); 
    if (reg_x == -1) {return 0; }
    
    load_value(val2_addr);
    generate_instruction(INSTR_SWP, reg_x); 

    load_value(val1_addr);
    generate_instruction(INSTR_ADD, reg_x);
    generate_instruction(INSTR_STORE, RESULT_ADDR);    

    release_register(reg_x); 
    return RESULT_ADDR;
}

long long handle_minus(long long val1_addr, long long val2_addr) {
    int reg_x = get_free_register(); 
    if (reg_x == -1) {return 0; }
    
    load_value(val2_addr);
    generate_instruction(INSTR_SWP, reg_x);

    load_value(val1_addr);
    generate_instruction(INSTR_SUB, reg_x);
    generate_instruction(INSTR_STORE, RESULT_ADDR);
    
    release_register(reg_x);
    return RESULT_ADDR;
}

//UWAGA WYNIK ZAWSZE W rejeSTRZE 4
//ADDRES 0 ZAREZERWOWANY DO CALL I RETURN
void generate_star_block() {
    int reg_x = 2; //na zmienna 1
    int reg_y = 3; //na zmienna 2
    int reg_z = 4; //na kroki do wyniku
    int reg_p = 5; // rejestr pomocniczy

    //przeskocz na koniec (można tu wejść tylko z CALL)
    int jump_to_end = code_ptr;
    generate_instruction(INSTR_JUMP, 0);

    //zapisz adres powrotu
    generate_instruction(INSTR_STORE, 0);

    //wyzeruj rejestr na wynik
    generate_instruction(INSTR_RST, reg_z);

    int label_start = code_ptr;
    int jzero_to_end_line = code_ptr;
    generate_instruction(INSTR_JZERO, 0);

    //y%2
    generate_instruction(INSTR_RST, 0);
    generate_instruction(INSTR_ADD, reg_y);
    generate_instruction(INSTR_SWP, reg_p); //w reg_p jest kopia reg_y

    generate_instruction(INSTR_SHR, reg_y);
    generate_instruction(INSTR_SHL, reg_y);
        
    generate_instruction(INSTR_RST, 0);
    generate_instruction(INSTR_ADD, reg_p);
    generate_instruction(INSTR_SUB, reg_y); //w rejestrze ra jest teraz reg_p%2

    //if y%2 == 1
    int jzero_to_next_iter = code_ptr;
    generate_instruction(INSTR_JZERO, 0);

    //reg_z = reg_z+ reg_x -> potem reg_x / 2
    generate_instruction(INSTR_SWP, reg_z); // ?    
    generate_instruction(INSTR_ADD, reg_x);     
    generate_instruction(INSTR_SWP, reg_z);

    int label_next_iteration = code_ptr;
    code_buffer[jzero_to_next_iter].arg = label_next_iteration;
        
    // Ponowne obliczenie y/2 (tym razem zapisujemy wynik)
    // ry <- ry / 2
    generate_instruction(INSTR_RST, 0);
    generate_instruction(INSTR_ADD, reg_p); 
    generate_instruction(INSTR_SWP, reg_y); 
    generate_instruction(INSTR_SHR, reg_y); // ra <- ry / 2
        
    // x = x * 2
    generate_instruction(INSTR_SHL, reg_x);
        
    //Skok do początku pętli
    generate_instruction(INSTR_JUMP, label_start); 
        
    //Koniec Pętli
    int label_end = code_ptr;
    code_buffer[jzero_to_end_line].arg = label_end;

    // b. ra <- adres powrotu (INSTR_RTRN używa ra jako adresu!)
    generate_instruction(INSTR_LOAD, RETURN_ADDR);
    
    // c. RTRN (k <- ra)
    generate_instruction(INSTR_RTRN, 0);

    code_buffer[jump_to_end].arg = code_ptr;
}

long long handle_star(long long val1_addr, long long val2_addr) {
    if (first_mult) {
        first_mult = 0;
        mult_start = code_ptr + 1; // Zapisz adres, na który będziemy skakać
        generate_star_block();
    }
    
    // 1. Ustawienie argumentów w stałych rejestrach: r2 <- val1, r3 <- val2
    
    // r2 (X) <- [val1_addr]
    load_value(val1_addr);
    generate_instruction(INSTR_SWP, 2);

    // r3 (Y) <- [val2_addr]
    load_value(val2_addr);
    generate_instruction(INSTR_SWP, 3);
    
    // 2. Wywołanie procedury
    // CALL j: ra <- k+1 (adres powrotu), k <- j (mult_start_line)
    generate_instruction(INSTR_CALL, mult_start); 
    
    // 3. Po powrocie (dzięki RTRN), wynik jest w r4.
    generate_instruction(INSTR_SWP, 4);

    // Zapisz wynik z ra. 
    generate_instruction(INSTR_STORE, RESULT_ADDR);
    
    return RESULT_ADDR;
}

void generate_div_block() {
    // R: reg_R (r2) - Dzielna / Reszta
    // D: reg_D (r3) - Dzielnik
    // Q: reg_Q (r4) - Iloraz
    // I: reg_I (r5) - Licznik przesunięć/pętli
    int reg_R = 2; 
    int reg_D = 3; 
    int reg_Q = 4; 
    int reg_I = 5; 
    

    //przeskocz na koniec (można tu wejść tylko z CALL)
    int jump_to_end = code_ptr;
    generate_instruction(INSTR_JUMP, 0);

    // CALL ustawił ra na adres powrotu. Zapisujemy go w p[0].
    generate_instruction(INSTR_STORE, 0); 

    //  Q <- 0, I <- 0
    generate_instruction(INSTR_RST, reg_Q); // r4 <- 0
    generate_instruction(INSTR_RST, reg_I); // r5 <- 0

    // --- Kontrola Dzielnika (B) ---
    // ra <- reg_D (r3) (Dzielnik)
    generate_instruction(INSTR_SWP, reg_D); 
    int jpos_skip_zero_check = code_ptr;
    generate_instruction(INSTR_JPOS, 0); // JPOS L_START_ALG (jeśli B > 0, kontynuuj)

    // OBSŁUGA DZIELENIA PRZEZ ZERO (B = 0)
    // Wynik (r4) i Reszta (r2) muszą być 0.
    generate_instruction(INSTR_RST, reg_Q); // r4 <- 0
    generate_instruction(INSTR_RST, reg_R); // r2 <- 0
        
    // JUMP do powrotu
    int jump_to_return = code_ptr;
    generate_instruction(INSTR_JUMP, 0); // JUMP L_RETURN

    // L_START_ALG: Tu zaczyna się właściwy algorytm (FAZA 1)
    int label_start_alg = code_ptr;
    code_buffer[jpos_skip_zero_check].arg = label_start_alg;
    generate_instruction(INSTR_SWP, reg_D); //r_d <- r_a (oddajemy)

// --- Pętla Normalizacyjna (WHILE D <= R) ---
    // Przesuń D (r3) w lewo, dopóki jest mniejsze/równe R (r2)

    int label_norm_start = code_ptr;
    
    // 1 Sprawdzenie warunku: R - D >= 0 <-> not(D - R > 0)
    generate_instruction(INSTR_RST, 0); 
    generate_instruction(INSTR_ADD, reg_D); // ra <- r3 (D)
    generate_instruction(INSTR_SUB, reg_R); // ra = D - R. Wynik w ra, r3 niezmienione.
    
    // 2 Jeśli D - R > 0 wychodzimy z pętli.
    int jpos_norm_end = code_ptr;
    generate_instruction(INSTR_JPOS, 0); 
    
    // 3 Ciało pętli: D <- D * 2, I <- I + 1
    generate_instruction(INSTR_SHL, reg_D); // r3 <- r3 * 2
    generate_instruction(INSTR_INC, reg_I); // I <- I + 1
    
    generate_instruction(INSTR_JUMP, label_norm_start); // JUMP do początku normalizacji

// --- Koniec Pętli Normalizacyjnej (L_NORM_END) ---
    int label_norm_end = code_ptr;
    code_buffer[jpos_norm_end].arg = label_norm_end;
    
// Korekta: Po wyjściu D (r3) jest za duże. D <- D / 2.
    generate_instruction(INSTR_SHR, reg_D); // r3 <- r3 / 2
    
// --- Faza Główna Dzielenia (Pętla przez wszystkie bity ilorazu) ---

    // --- Pętla WHILE (WHILE I > 0) ---
    int label_div_start = code_ptr;

    // 1 Sprawdzenie warunku pętli: I == 0 ?
    generate_instruction(INSTR_SWP, reg_I); // ra <- r5 (I)
    int jzero_div_end = code_ptr;
    generate_instruction(INSTR_JZERO, 0); // JZERO L_DIV_END
    generate_instruction(INSTR_SWP, reg_I);

    // Rzutowanie Q w lewo (przygotowanie do następnego bitu)
    generate_instruction(INSTR_SHL, reg_Q); // r4 <- r4 * 2

    // --- 2 Sprawdzenie warunku: R >= D <-> not (D - R > 0) ---
    generate_instruction(INSTR_RST, 0);
    generate_instruction(INSTR_ADD, reg_D);
    generate_instruction(INSTR_SUB, reg_R); 
    
    // Jeśli R < D, skocz do L_NO_SUBTRACT
    int jpos_no_subtract = code_ptr;
    generate_instruction(INSTR_JPOS, 0); 

    // --- Ciało IF (R >= D): Możemy odjąć i ustawić bit ---

    // 3 Ustawienie bitu ilorazu: Q <- Q + 1
    generate_instruction(INSTR_INC, reg_Q);  

    // 4 Aktualizacja reszty: R <- R - D
    generate_instruction(INSTR_SWP, reg_R);
    generate_instruction(INSTR_SUB, reg_D);
    generate_instruction(INSTR_SWP, reg_R); //(r2 = R-D)

    // --- Koniec Sekcji IF (L_NO_SUBTRACT) ---
    int label_no_subtract = code_ptr;
    code_buffer[jpos_no_subtract].arg = label_no_subtract;

    // 5 Aktualizacja na następną iterację
    
    // Dzielnik w prawo: D <- D / 2
    generate_instruction(INSTR_SHR, reg_D); // r3 <- r3 / 2
    
    // Licznik w dół: I <- I - 1
    generate_instruction(INSTR_DEC, reg_I);
    
    generate_instruction(INSTR_JUMP, label_div_start); // JUMP do początku dzielenia

    // --- Koniec Głównej Pętli Dzielenia (L_DIV_END) ---
    int label_div_end = code_ptr;
    code_buffer[jzero_div_end].arg = label_div_end; // Backpatching JZERO Pętli
    code_buffer[jump_to_return].arg = label_div_end;
// --- Powrót ---
    
    //Wynik (Iloraz Q) jest w r4. Reszta R jest w r2.
    // Załaduj adres powrotu do ra
    generate_instruction(INSTR_LOAD, RETURN_ADDR);
    
    // RTRN (k <- ra)
    generate_instruction(INSTR_RTRN, 0);
    code_buffer[jump_to_end].arg = code_ptr;
}

long long handle_div(long long val1_addr, long long val2_addr) {
    if (first_div) {
        first_div = 0;
        div_start= code_ptr + 1;
        generate_div_block(); // Generowanie bloku procedury DIV/MOD
    }
    
    // --- 1. Ustawienie argumentów ---
    
    // r2 (R) <- [val1_addr] (Dzielna A)
    load_value(val1_addr);
    generate_instruction(INSTR_SWP, 2); 

    // r3 (D) <- [val2_addr] (Dzielnik B)
   load_value(val2_addr);
    generate_instruction(INSTR_SWP, 3);

    // --- 2. Wywołanie procedury ---
    generate_instruction(INSTR_CALL, div_start); 
    
    // --- 3. Zapisanie wyniku (Iloraz Q) ---
    generate_instruction(INSTR_SWP, 4);
    generate_instruction(INSTR_STORE, RESULT_ADDR); // Zapisujemy ra (Q)
    
    return RESULT_ADDR;
}

long long handle_mod(long long val1_addr, long long val2_addr) {
    if (first_div) {
        first_div = 0;
        div_start = code_ptr + 1;
        generate_div_block(); // Generowanie bloku procedury DIV/MOD
    }
    
    // --- 1. Ustawienie argumentów (Takie same jak dla DIV) ---
    
    // r2 (R) <- [val1_addr] (Dzielna A)
    load_value(val1_addr);
    generate_instruction(INSTR_SWP, 2); 

    // r3 (D) <- [val2_addr] (Dzielnik B)
    load_value(val2_addr);
    generate_instruction(INSTR_SWP, 3);

    // --- 2. Wywołanie procedury ---
    generate_instruction(INSTR_CALL, div_start); 
    
    // --- 3. Zapisanie wyniku (Reszta R) ---
    generate_instruction(INSTR_SWP, 2); // ra <- r2 (Reszta R)
     
    generate_instruction(INSTR_STORE, RESULT_ADDR); // Zapisujemy ra (R)
    
    return RESULT_ADDR;
}