%option nodefault
%option noyywrap
%option noinput
%option nounput

%{
//#define _POSIX_C_SOURCE 200809L

#include "stdio.h"
#include "stdlib.h"
#include "string.h"   
#include "unistd.h"
#include "cp.tab.h"
int yylex();

//extern char *strdup(const char *s);
%}

%%
^"#".*              {}
[0-9]+              {yylval.val = strtoll(yytext, NULL, 10); return NUM;}
"PROGRAM"           {return PROGRAM;}
"IS"                {return IS;}
"IN"                {return IN;}
"ENDIF"             {return ENDIF;}
"ENDWHILE"          {return ENDWHILE;}
"ENDFOR"            {return ENDFOR;}
"END"               {return END;}
"PROCEDURE"         {return PROCEDURE;}
"IF"                {return IF;}
"ELSE"              {return ELSE;}
"THEN"              {return THEN;}
"WHILE"             {return WHILE;}
"REPEAT"            {return REPEAT;}
"UNTIL"             {return UNTIL;}
"FOR"               {return FOR;}
"FROM"              {return FROM;}
"DOWNTO"            {return DOWNTO;}
"TO"                {return TO;}
"DO"                {return DO;}
"READ"              {return READ;}
"WRITE"             {return WRITE;}
"T"                 {return T;}   
"I"                 {return I;}   
"O"                 {return O;}
"+"                 {return PLUS;}
"-"                 {return MINUS;}
"*"                 {return STAR;}
"/"                 {return SLASH;}
"%"                 {return MOD;}
":="                {return ASSIGN;}
"!="                {return NE;}
"<="                {return LE;}
">="                {return GE;}
"="                 {return EQ;}
"<"                 {return LT;}
">"                 {return GT;}
"("|")"|"["|"]"     {return *yytext;}
","|";"|":"         {return *yytext;}
[_a-z]+             {yylval.text = strdup(yytext); return PIDENTIFIER;}     
[\t\n]+             {}
.                   {}
%%