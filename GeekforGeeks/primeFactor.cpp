#include<iostream>
using namespace std;

void primeFactor(long int num){
 long int i;
  
   cout<<"Prime Factors:";
    for(i=2;i<=num/i;i++){
         while(num%i== 0){
            num/=i;
            cout<<i<<"*";
         }
    }
      if(num>1) cout<<num;
}

int main(){
 long int n;
 cout<<"Enter the number:";cin>>n;
 primeFactor(n);
 return 0;
}
