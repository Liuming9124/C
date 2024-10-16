%{
    #include <stdio.h>
%}
%option noyywrap
%%
[-]{0,1}[0-9]+;\n  {
            printf("Int: %s",yytext);
        }
[-]{0,1}[0-9]+\.[0-9]+;\n {
            printf("Float: %s",yytext);
        }
[a-zA-Z]+[0-9a-zA-z_]*;\n {
            printf("Identifier: %s",yytext);
        }
[^\n]+\n {
             printf("Error: %s",  yytext);
        }
[^\n]+  {
            printf("Error: %s\n",yytext);
        }
\n      {
            printf("Error: \n",yytext);
        }
%%
int main()
{
    yylex();
    return 0;
}
