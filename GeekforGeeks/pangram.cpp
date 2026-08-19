#include<iostream>
#include<stdio.h>

using namespace std;


int main() {
   char ch;
   int ar[26]={0},index=0,flag=1,i;
    while((ch=getchar())!='\n')
          { if(ch>='A' && ch<='Z') 
              index=ch -'A';
            else if(ch>='a' && ch<='z') 
              index=ch-'a';
            else continue;
            
            ar[index]=1;        
          }
    for(i=0;i<26;i++)
         { if(ar[i]==0)
             flag=0;
         }
    
    if(flag==0) 
        printf("not pangram");
    else 
        printf("pangram");
    return 0;
}

