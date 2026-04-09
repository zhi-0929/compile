%{
#include <stdio.h>
%}

%%

"int"|"main"|"return"   { printf("%s\t<KW>\n",  yytext); }
"="                     { printf("%s\t<OP>\n",  yytext); }
";"|"("|")"|"{"|"}"     { printf("%s\t<SE>\n",  yytext); }
[a-z]                   { printf("%s\t<IDN>\n", yytext); }
[0-9]+                  { printf("%s\t<INT>\n", yytext); }
[ \n]                   {}

%%

int yywrap(void) {
    return 1;
}

int main(void) {
    printf("Enter text (Ctrl+Z to quit):\n");
    yylex();
    return 0;
}
