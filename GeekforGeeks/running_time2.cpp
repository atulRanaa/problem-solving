#include<iostream>
#include<math.h>
using namespace std;
int main(){
int flag=0,num;
cout<<"ENTER A NUMBER WHETHER IT IS PRIME OR NOT:";
cin>>num;
int sqr=sqrt(num),i;

for(i=2;i<=sqr;i++){

if(num%i==0){
flag=1;break;}
}

if(flag==1)cout<<"NUMBER IS MOT PRIME\n";
else
cout<<"NUMBER IS PRIME\n";

return 0;
}


