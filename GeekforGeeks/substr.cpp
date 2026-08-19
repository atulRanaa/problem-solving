#include<bits/stdc++.h>
using namespace std;
int main(){
string str="abababcabcabcdabcd",temp;
 int i=2,j=2;
int len=strlen(str);
   while(i<len){
     while(j<i)
      {  temp=str.substr(i,i+j);
         if(str.find(temp,0,i)==0) break;
         else printf("match");
         j++;
       }  
    i++;
   }


return 0;
}
