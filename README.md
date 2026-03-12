# Compiler

This is a compiler written in C for a language given by a gramar:

program  ⟶ procedures main

procedures  ⟶ procedures PROCEDURE proc_head IS declarations IN commands END | $\epsilon$

main ⟶ PROGRAM IS declarations IN commands END

proc_head ⟶ PIDENTIFIER ( args_decl )

proc_call ⟶ PIDENTIFIER ( args )

commands ⟶ commands command | command

command ⟶ identifier ASSIGN expression ; |
          IF condition THEN commands ENDIF |
          IF condition THEN commands ELSE commands ENDIF |
          READ identifier ; |
          WRITE identifier ; |
          REPEAT commands UNTIL condition ; |
          WHILE condition DO commands ENDWHILE |
          FOR PIDENTIFIER FROM value TO value DO commands ENDFOR |
          FOR PIDENTIFIER FROM value DOWNTO value DO commands ENDFOR |
          proc_call ;

expression ⟶ value |
             expression + value |
             expression - value |
             expression * value | 
             expression / value |
             expression % value
             
condition ⟶ value = value |
            value != value |
            value > value |
            value < value |
            value >= value |
            value <= value

args_decl ⟶ args_decl , type PIDENTIFIER | type PIDENTIFIER

args ⟶ args PIDENTIFIER | PIDENTIFIER


The code is compiled for a specific virtual machine.
