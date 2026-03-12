# Compiler

This is a compiler written in C for a language given by a gramar:

program  ⟶ procedures main

procedures  ⟶ procedures PROCEDURE proc_head IS declarations IN commands END | $\epsilon$

main ⟶ PROGRAM IS declarations IN commands END

proc_head ⟶ PIDENTIFIER ( args_decl )

proc_call ⟶ PIDENTIFIER ( args )

commands ⟶ commands command | command

command ⟶ identifier ASSIGN expression ; | <br>
          IF condition THEN commands ENDIF | <br>
          IF condition THEN commands ELSE commands ENDIF | <br>
          READ identifier ; | <br>
          WRITE identifier ; | <br>
          REPEAT commands UNTIL condition ; | <br>
          WHILE condition DO commands ENDWHILE | <br>
          FOR PIDENTIFIER FROM value TO value DO commands ENDFOR | <br>
          FOR PIDENTIFIER FROM value DOWNTO value DO commands ENDFOR | <br>
          proc_call ; <br>

expression ⟶ value | <br>
             expression + value | <br>
             expression - value | <br>
             expression * value |  <br>
             expression / value | <br>
             expression % value <br>
             
condition ⟶ value = value | <br>
            value != value | <br>
            value > value | <br>
            value < value | <br>
            value >= value | <br>
            value <= value <br>

args_decl ⟶ args_decl , type PIDENTIFIER | type PIDENTIFIER

args ⟶ args PIDENTIFIER | PIDENTIFIER

type ⟶ T | I | O | $\epsilon$

declarations ⟶ declarations , PIDENTIFIER | <br>
               declarations, PIDENTIFIER [ NUM : NUM ] | <br>
               PIDENTIFIER | <br>
               PIDENTIFIER [ NUM : NUM ] 

value ⟶ NUM | identifier

identifier ⟶ PIDENTIFIER | PIDENTIFIER [ PIDENTIFIER ] | PIDENTIFIER [ NUM ]


The code is compiled for a specific virtual machine.
