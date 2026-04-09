%{
#include <stdio.h>
#include "mycode.tab.h"
%}

%%
"left"|"l"      { return LEFT; }
"right"|"r"     { return RIGHT; }
"up"|"u"        { return UP; }
"down"|"d"      { return DOWN; }
"forward"|"f"   { return FORWARD; }
"back"|"b"      { return BACK; }
"query"|"q"     { return QUERY; }
[0-9]+          { yylval = atoi(yytext); return NUMBER; }
";"             { return OVER;}
[ \t\n]         ;
%%

int yywrap(void) {
    return 1;
}