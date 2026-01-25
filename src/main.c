// main.c

#include <stdio.h>
#include <stdlib.h>
#include "../inc/mem_mng.h"
#include "../inc/utils.h"

// Deklaracje zewnętrzne
extern int yyparse();
extern FILE *yyin; 
extern void print_compiled_code(); // Funkcja do wypisania kodu 

int main(int argc, char **argv) {
    // Sprawdzenie argumentów
    if (argc != 2) {
        fprintf(stderr, "Użycie: %s <plik_wejsciowy>\n", argv[0]);
        return 1;
    }

    // Otwarcie pliku wejściowego
    FILE *input_file = fopen(argv[1], "r");
    if (!input_file) {
        perror("Błąd otwarcia pliku wejściowego");
        return 1;
    }
    yyin = input_file; // Ustawienie pliku wejściowego dla parsera/skanera

    // INICJALIZACJA
    initTMP(); 
   
    fprintf(stderr, "Rozpoczęcie parsowania i generowania kodu...\n");
    yyparse(); 
    
    // HALT został dodany w regule 'main' w parser.y, ale jeśli tam go nie ma,
    // należy go dodać tutaj: generate_instruction(INSTR_HALT, 0);

    // WYPISANIE WYNIKU
    print_compiled_code(); 
    
    // ... Sprzątanie i wyjście
    return 0;
}