%{
#include <stdio.h>
int row = 1, col = 1;
%}

%%

[iI][nN][tT]                {printf("%s\t<KW,1>\n", yytext); col += yyleng;}
[vV][oO][iI][dD]            {printf("%s\t<KW,2>\n", yytext); col += yyleng;}
[rR][eE][tT][uU][rR][nN]    {printf("%s\t<KW,3>\n", yytext); col += yyleng;}
[cC][oO][nN][sS][tT]        {printf("%s\t<KW,4>\n", yytext); col += yyleng;}
[mM][aA][iI][nN]            {printf("%s\t<KW,5>\n", yytext); col += yyleng;}
[fF][lL][oO][aA][tT]        {printf("%s\t<KW,6>\n", yytext); col += yyleng;}
[iI][fF]                    {printf("%s\t<KW,7>\n", yytext); col += yyleng;}
[eE][lL][sS][eE]            {printf("%s\t<KW,8>\n", yytext); col += yyleng;}

"+"           {printf("%s\t<OP,9>\n", yytext); col += yyleng;}
"-"           {printf("%s\t<OP,10>\n", yytext); col += yyleng;}
"*"           {printf("%s\t<OP,11>\n", yytext); col += yyleng;}
"/"           {printf("%s\t<OP,12>\n", yytext); col += yyleng;}
"%"           {printf("%s\t<OP,13>\n", yytext); col += yyleng;}
"="           {printf("%s\t<OP,14>\n", yytext); col += yyleng;}
">"           {printf("%s\t<OP,15>\n", yytext); col += yyleng;}
"<"           {printf("%s\t<OP,16>\n", yytext); col += yyleng;}
"=="          {printf("%s\t<OP,17>\n", yytext); col += yyleng;}
"<="          {printf("%s\t<OP,18>\n", yytext); col += yyleng;}
">="          {printf("%s\t<OP,19>\n", yytext); col += yyleng;}
"!="          {printf("%s\t<OP,20>\n", yytext); col += yyleng;}
"&&"          {printf("%s\t<OP,21>\n", yytext); col += yyleng;}
"||"          {printf("%s\t<OP,22>\n", yytext); col += yyleng;}

"("           {printf("%s\t<SE,23>\n", yytext); col += yyleng;}
")"           {printf("%s\t<SE,24>\n", yytext); col += yyleng;}
"{"           {printf("%s\t<SE,25>\n", yytext); col += yyleng;}
"}"           {printf("%s\t<SE,26>\n", yytext); col += yyleng;}
";"           {printf("%s\t<SE,27>\n", yytext); col += yyleng;}
","           {printf("%s\t<SE,28>\n", yytext); col += yyleng;}

[a-zA-Z_][a-zA-Z0-9_]*   {printf("%s\t<IDN,%s>\n", yytext, yytext); col += yyleng;}
[0-9]+"."[0-9]+          {printf("%s\t<FLOAT,%s>\n", yytext, yytext); col += yyleng;}
[0-9]+                   {printf("%s\t<INT,%s>\n", yytext, yytext); col += yyleng;}

"\n"          {row++; col = 1;}
[ \t\r]+      {col += yyleng;}

.             {printf("%s\t<ERROR,%d,%d>\n", yytext, row, col); col += yyleng;}
%%

int yywrap(void) {
    return 1;
}

int main(void) {
    yylex();
    return 0;
}
