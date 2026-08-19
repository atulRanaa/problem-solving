#include<iostream>
#include<math.h>
using namespace std;
int main(){
int flag=0;
cout<<"ENTER A NUMBER TO CHECK PRIME OR NOT";
int num;
cin>>num;

for(int i=2;i<=sqrt(num);i++){

if(num % i==0)
{
flag=1;
break;}
}


if(flag==1){cout<<"NUMBER IS NOT PRIME\n";}
else
cout<<"NUMBER IN PRIME\n";
return 0;
}
