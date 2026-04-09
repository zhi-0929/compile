%{
#include <stdio.h>

/* Define token values so we don't need y.tab.h */
#define HI   1
#define BYE  2

/* No parser — we'll print directly */
%}

%%

("hi"|"oi")"\n"        { printf("Got HI token\n"); }
("tchau"|"bye")"\n"    { printf("Got BYE token\n"); }
[-+.,><]               { printf("Other char: %c\n", yytext[0]); }
.                      { printf("Unknown char: %c\n", yytext[0]); }

%%

int yywrap(void) {
    return 1;          /* Tell Flex we’re at EOF */
}

int main(void) {
    printf("Enter text (Ctrl+Z to quit):\n");
    yylex();           /* Call the scanner directly */
    return 0;
}