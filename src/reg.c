#include "../inc/reg.h"
#include "stdlib.h"

char registers_in_use[8] = {0, 0, 0, 0, 0, 0, 0, 0};

int get_free_register() {
    for (int i = 1; i <= 7; i++) {
        if (registers_in_use[i] == 0) {
            registers_in_use[i] = 1;
            return i; // Zwraca numer rejestru (1 do 7)
        }
    }
    // Obsługa błędu - brak wolnych rejestrów
    return -1; 
}

void release_register(int reg_num) {
    if (reg_num >= 1 && reg_num <= 7) {
        registers_in_use[reg_num] = 0;
    }
}