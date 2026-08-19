#include<iostream>
#include<math.h>
#include<algorithm>
#include<vector>
#include<string.h>

using namespace std;
long int anytoDecimal(string s,int base){
   long int sum=0;
   int j=0;
   for(int i=s.size()-1,j=0 ; i>=0 ; i--,j++){
      if(s[j]<58){
       sum=(sum+((int)(s[j]-48))*pow(base,i));
      }
      else if(s[j]>=65){
      sum+=(((int)(s[j]-55))*pow(base,i));
      }
   }
 return sum;
}
void decimaltoAny(int num,int base){
  vector<char> number;
  char ch;
  int temp;
  while(num != 0){
    temp=num%base;
      if(temp > 9){
        ch=(char)(temp+55);
        number.push_back(ch);
      }
      else if(temp <= 9){ ch=(char)(temp+48); cout<<ch<<" ";
           number.push_back(ch);
      }
   num=num/base;
  }
  reverse(number.begin(),number.end());
for(int i=0;i<number.size();i++) cout<<number[i];
}
int main(){
  string s="45A";
  int num;
  num=anytoDecimal(s,12);
  cout<<num<<endl;
  decimaltoAny(num,4);
}
