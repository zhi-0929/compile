%{

  #include <stdio.h>
  #include <stdlib.h>
  int yylex(void);
  int yyerror(const char *s);

%}

%token HI BYE

%%

input:
        unit_list;

unit_list:
        unit
        | unit unit_list
        ;

unit:
        hi
        | bye
        ;

hi:
        HI     { printf("Hello World\n");   }
        ;
bye:
        BYE    { printf("Bye World\n");}
         ;
