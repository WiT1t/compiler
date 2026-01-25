%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./inc/arithm.h"
#include "./inc/loops.h"
#include "./inc/cond.h"
#include "./inc/utils.h"      // handle_*, generate_instruction
#include "./inc/mem_mng.h"
#include "./inc/ifels.h"       // handle_if, handle_else, handle_endif     // handle_identifier

extern int yylex();

// Funkcja błędu składni (zwykle implementowana w sekcji %{...%} lub osobnym pliku)
int yyerror(const char *s) {
    fprintf(stderr, "Błąd składni: %s\n", s);
    return 0;
}
%}

// --- Deklaracja Typów Semantycznych ---
%union {
    long long addr;        // Dla adresów w pamięci (wynik handle_*, const_addr)
    int jump_line;         // Dla numerów linii w buforze kodu (do łatania skoków)
    char *text;            // Dla identyfikatorów (PIDENTIFIER)
    long long val;         // Dla stałych liczbowych (NUM)
}

// --- Deklaracje Terminali (Tokenów) ---

// Słowa kluczowe
%token PROGRAM IS IN END
%token WHILE ENDWHILE FOR ENDFOR DO REPEAT DWONT TO PROCEDURE UNTIL FROM DOWNTO T I O E
%token IF THEN ELSE ENDIF
%token READ WRITE

// Operatory/Symbole
%token ASSIGN ';' 
// Operatory relacyjne (Condition)
%token EQ NE LT LE GT GE
// Operatory arytmetyczne (Expression)
%token PLUS MINUS STAR SLASH MOD

// Typy dla tokenów
%token <text> PIDENTIFIER 
%token <val> NUM         

// --- Deklaracje Symboli Nieterminalnych (Reguł) ---

// Symbole przechowujące Adres (addr)
%type <addr> expression value condition identifier num

// Symbole przechowujące Numer Linii Skoku (jump_line)
%type <jump_line> if_start_jump if_else_start

// Pozostałe symbole nieterminalne
%type program commands command main declarations

%right ASSIGN 
%left EQ NE LT LE GT GE 
%left PLUS MINUS
%left STAR SLASH MOD

%%
// ===================================
// Sekcja Reguł Gramatycznych
// ===================================

program: main
;

main: PROGRAM IS declarations IN commands END
    {
        // Po przetworzeniu wszystkich komend, dodajemy instrukcję kończącą program
        generate_instruction(INSTR_HALT, 0); 
    }
    |PROGRAM IS IN commands END
    {
        // Po przetworzeniu wszystkich komend, dodajemy instrukcję kończącą program
        generate_instruction(INSTR_HALT, 0); 
    }
;

commands: commands command
| command
;

command: identifier ASSIGN expression ';'
    {
        // $1: adres docelowej zmiennej (LHS)
        // $3: adres wyniku wyrażenia (RHS)
        handle_assign($1, $3); 
    }
| IF if_start_jump THEN commands ENDIF
    {
        // $2: numer linii instrukcji JZERO, którą należy załatać adresem ENDIF
        handle_endif($2);
    }
| IF if_else_start ELSE commands ENDIF
    {
        // $2: numer linii instrukcji JUMP z końca sekcji THEN
        handle_endif($2);
    }
| READ identifier ';'
    {
        // $2: adres zmiennej, do której ma być wczytana wartość
        handle_read($2);
    }
| WRITE expression ';'
    {
        // $2: adres wartości (stała lub wynik wyrażenia) do wypisania
        handle_write($2);
    }
| REPEAT {$<val>$ = code_ptr;} commands UNTIL condition ';'
    {
        // $2 linia do której skoczymy.
        handle_RU($<val>2);
    }
| WHILE {$<val>$ = code_ptr;} condition DO 
  {$<val>$ = code_ptr; generate_instruction(INSTR_JZERO, 0);} commands ENDWHILE
    {
        // $2 linia do skoku do sprawdzenia warunku
        // $5 linia do załatania skoku na koniec
        handle_while($<val>2, $<val>5);
    }
| FOR PIDENTIFIER FROM value 
    {
        $<val>$ = handle_iterator($2, $4);
    } 
    TO value DO
    {
        $<val>$ = code_ptr;
    }
    {
        $<val>$ = handle_up_for($<val>5, $7);
    } 
    commands ENDFOR
    {
        int jump_line = handle_endfor_up($<val>10, $<val>5);
        code_buffer[jump_line].arg = $<val>9;
    }
| FOR PIDENTIFIER FROM value
    {
        $<val>$ = handle_iterator($2, $4);
    }
    DOWNTO value 
    DO
    {
        $<val>$ = code_ptr;
    }
    {
        $<val>$ = handle_down_for($<val>5, $7);
    }
    commands ENDFOR
    {
        int jump_line = handle_endfor_down($<val>10, $<val>5);
        code_buffer[jump_line].arg = $<val>9;
    }
;   

// --- Sekcje pomocnicze dla łatania skoków ---

// 1. Generuje kod dla warunku i JZERO. Zwraca numer linii JZERO.

if_start_jump: condition
    {
        // $1: adres flagi 0/1 zwrócony przez handle_eq/l/g...
        $$ = handle_if($1);  
    }
;

// 2. Obsługa bloku THEN i generowanie JUMP (dla IF-ELSE)
if_else_start: if_start_jump THEN commands
    {
        // $1: numer linii JZERO, który trafi do funkcji handle_else
        $$ = handle_else($1);
    }

// --- Wyrażenia Arytmetyczne ---

expression: value 
| expression PLUS value
    { $$ = handle_plus($1, $3); }
| expression MINUS value
    { $$ = handle_minus($1, $3); }
| expression STAR value
    { $$ = handle_star($1, $3); }
| expression SLASH value
    { $$ = handle_div($1, $3); }
| expression MOD value
    { $$ = handle_mod($1, $3); }
;

// --- Warunki ---

condition: value EQ value
    { $$ = handle_eq($1, $3); }
| value NE value
    { $$ = handle_neq($1, $3); }
| value GT value
    { $$ = handle_g($1, $3); }
| value LT value
    { $$ = handle_l($1, $3); }
| value GE value
    { $$ = handle_ge($1, $3); }
| value LE value
    { $$ = handle_le($1, $3); }
;

// --- Deklaracje ---
declarations: declarations ',' PIDENTIFIER {
                if (find_symbol_entry($3) != NULL) {
                    fprintf(stderr, "Błąd: Druga deklaracja zmiennej %s\n", $3);
                    exit(1);
                }
                add_symbol($3);
            } 
            |declarations ',' PIDENTIFIER '[' NUM ':' NUM ']' {
                if(find_arr_idx($3) != -1) {
                    fprintf(stderr, "Błąd: Druga deklaracja tablicy %s\n", $3);
                    exit(1);
                }
                add_arr($3, $5, $7);
            }
            | PIDENTIFIER {
                if (find_symbol_entry($1) != NULL) {
                    fprintf(stderr, "Błąd: Druga deklaracja zmiennej %s\n", $1);
                    exit(1);
                }
                add_symbol($1);
            }
            | PIDENTIFIER '[' NUM ':' NUM ']' {
                if(find_arr_idx($1) != -1) {
                    fprintf(stderr, "Błąd: Druga deklaracja tablicy %s\n", $1);
                    exit(1);
                }
                add_arr($1, $3, $5);
            }
            ;

// --- Wartości i Identyfikatory ---

value: num
| identifier
;

identifier: PIDENTIFIER { $$ = handle_id($1); }
    | PIDENTIFIER '[' PIDENTIFIER ']' { $$ = handle_arr_dynamic($1, $3); }
    | PIDENTIFIER '[' NUM ']' { $$ = find_arr_member($1, $3); }
;

num: NUM
    {
        // $1: Wartość liczbowa (long long). Zwracamy adres stałej.
        $$ = handle_num($1);
    }
;

%%