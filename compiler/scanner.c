#include <stdio.h>
#include <string.h>
void snum  ();  // number : int + float
void sid   ();  //ID

char str[1000];
int ci=0,cf=0,cid=0,cerr=0;
int main(void){
    FILE *fp = fopen("test.txt","r");
    if(fp==NULL){   //find fill exist or not
        perror("Error opening file");
        return(-1);
    }
    while( fgets (str, 60, fp)!=NULL ) {
        if (strlen(str)>=3){        //have formal length of string  ex: a;\n >=3
            if (str[0]>='0' && str[0]<='9' || str[0]=='-'){  //call number
                snum();
            }
            else if ((str[0]>='a'&&str[0]<='z')||(str[0]>='A'&&str[0]<='Z')){   //call id
                sid();
            }
            else{   //call error : error first char
                if (str[strlen(str)-1]=='\n')   printf ("Error: %s", str);
                else                            printf ("Error: %s\n", str);
                cerr++;
            }
        }
        else{
            if (strlen(str)==1){
                if (str[0]=='\n')   printf ("Error: %s",str);
                else                printf ("Error: %s\n",str);
                cerr++;
            }
            else if (strlen(str)==2){
                if (str[1]=='\n')   printf ("Error: %s",str);
                else                printf ("Error: %s\n",str);
                cerr++;
            }
        }
    }
    printf("----------token----------\nint  :%d\nfloat:%d\nid   :%d\nerror:%d\n",ci,cf,cid,cerr);
    fclose(fp);
}
void snum(){        //modify '-' assign
    int state = 0;
    int len = strlen(str);
    int start = 0;
    if (str[0]=='-')    start = 1;
    else                start = 0;
    for (int i=start; i<len; i++){
        if (i==len-2){      //last two char in string
            if (str[i+1]=='\n' && str[i]==';'){   //find ;\n -> means right ending   
                if (state==0){  //find int
                    printf ("Int: %s",str);
                    ci++;
                    return;
                }
                else if (state == 1){   //find float
                    printf ("Float: %s",str);
                    cf++;
                    return;
                }
            }
            else{
                if (str[len-1]=='\n')   printf ("Error: %s", str);
                else                    printf ("Error: %s\n", str);
                cerr++;
                return;
            }
        }
        else if (str[i]>='0' && str[i]<='9'){
            continue;
        }
        else if (str[i]=='.'){
            if (state==0){
                state=1;
            }
            else{
                if (str[len-1]=='\n')   printf ("Error: %s", str);
                else                    printf ("Error: %s\n", str);
                cerr++;
                return;
            }
        }
        else {
            if (str[len-1]=='\n')   printf ("Error: %s", str);
            else                    printf ("Error: %s\n", str);
            cerr++;
            return;
        }
    }
}
void sid (){
    int len = strlen(str);
    for (int i=1;i<len;i++){
        if (i==len-2){      //last two char in string
            if (str[i+1]=='\n' && str[i]==';'){   //find ;\n -> means right ending   
                printf ("ID: %s",str);
                cid++;
                return;
            }
            else{
                if (str[len-1]=='\n')   printf ("Error: %s", str);
                else                    printf ("Error: %s\n", str);
                cerr++;
                return;
            }
        }
        else if ( (str[i]>='a' && str[i]<='z') || (str[i]>='A' && str[i]<='Z') || (str[i]=='_') || (str[i]>='0' && str[i]<='9') );
        else{
            if (str[len-1]=='\n')   printf ("Error: %s", str);
            else                    printf ("Error: %s\n", str);
            cerr++;
            return;
        }
    }
}