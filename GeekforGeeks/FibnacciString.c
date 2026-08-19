#include <stdio.h>
#include <string.h>
#include <stdlib.h>

long long fib[80];
char fib_s[2000];
int ssize =0;
int valid[700];

void generate_fib(){
    fib[0] = 0;
    fib[1] = 1;
    fib_s[0]='0';
    fib_s[1]='1';
    ssize += 2;
    valid[0] = valid[1] = 1;
    
    for(int i=2;i<80;i++){
        fib[i] = fib[i-1] + fib[i-2];
        char str[30];
        
        itoa(fib[i],&str,10);
        
        int l = strlen(str),x;
        
        for(x=0;x<l;x++) fib_s[ssize++]=str[x];
        
        valid[strlen(fib_s) - 1] = 1;
    }
}
 
int main(){
    generate_fib();
    
    int test;
    scanf("%d",&test);
    while(test--){
        int i, n;
        char s[1000];
        int f = 1; int x, y, j;
        scanf("%s",s);
        if(s[0] == '0') i = 0;
        else if(s[0] == '2') i = 3;
        else if(s[0]=='3')i = 4;
        else if(s[0] == '5') i = 5;
        else if(s[0] == '8')i = 6;  
        else{
            if(s[1]=='1')i = 1;
            else i = 2;
        }
        j = 0;
        int len = strlen(s);
        for(j;j<len;i++, j++){
            if(s[j] != fib_s[i]){
                f = -1;
            }
        }
        if(f && valid[i-1]){
            printf("Valid\n");
        }
        else printf("Invalid\n");
    
    }
    
    return 0;
}