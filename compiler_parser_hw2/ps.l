%{
    #include "y.tab.h"
%}
%option noyywrap
%%
[0-9]+  { yylval = atoi(yytext);
            return(Realnumber); }
"("     { return(Lpa); }
")"     { return(Rpa); }
"*"     { return(Times); }
"/"     { return(Div); }
"+"     { return(Plus); }
"-"     { return(Minus); }
\n      { return '\n' ;}
.       { return yytext[0];}
%%