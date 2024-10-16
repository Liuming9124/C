%token Realnumber Plus Minus Times Div Lpa Rpa
%{
    #include<stdio.h>
    int yylex();
    int yyerror();
    int len=0;
%}
%%
statement :
    | statement expression
        {
            printf("\nAnser%d => %d\n\n===========\n",++len,$2);
        }
    | statement expression '\n'
        {
            printf("\nAnser%d => %d\n\n===========\n",++len,$2);
        }
    ;
expression :
    | expression lable primary
        {
            if      ($2=='t'){
                $$=$1*$3;
                printf("%d multiply %d equals %d\n",$1,$3,$$);
            }
            else if ($2=='d'){
                $$=$1/$3;
                printf("%d divide %d equals %d\n",$1,$3,$$);
            }
            else if ($2=='p'){
                $$=$1+$3;
                printf("%d add %d equals %d\n",$1,$3,$1+$3);
            }
            else if ($2=='m'){
                $$=$1-$3;
                printf("%d cut %d equals %d\n",$1,$3,$1-$3);
            }
        }
    | primary
    | Number
primary:
    | Lpa expression Rpa lable1 expression
        {
            if      ($4=='t'){
                $$=$2*$5;
                printf("%d multiply %d equals %d\n",$2,$5,$$);
            }
            else if ($4=='d'){
                $$=$2/$5;
                printf("%d divide %d equals %d\n",$2,$5,$$);
            }
        }   
    | Lpa expression Rpa
        {
            $$=$2;
        }
    | primary1
    | primary2
primary1:
    | primary1 lable1 Number
        {
            if      ($2=='t'){
                $$=$1*$3;
                printf("%d multiply %d equals %d\n",$1,$3,$$);
            }
            else if ($2=='d'){
                $$=$1/$3;
                printf("%d divide %d equals %d\n",$1,$3,$$);
            }
        }
    | Number
primary2:
    | primary2 lable2 Number
        {
            if      ($2=='p'){
                $$=$1+$3;
                printf("%d add %d equals %d\n",$1,$3,$1+$3);
            }
            else if ($2=='m'){
                $$=$1-$3;
                printf("%d cut %d equals %d\n",$1,$3,$1-$3);
            }
        }
    | Number
Number:
    | Minus Realnumber  
        {
            $$=-1*$2;
        }
    | Realnumber
        {
            $$=$1;
        }
lable:
    | lable1
    | lable2
lable1:
    | Times     { $$='t'; }
    | Div       { $$='d'; }
lable2:
    | Plus      { $$='p'; }
    | Minus     { $$='m'; }
%%
int yyerror(char *msg)
{
    printf("Error:%s \n", msg);
}
int main()
{
    yyparse();
    return 0;
}