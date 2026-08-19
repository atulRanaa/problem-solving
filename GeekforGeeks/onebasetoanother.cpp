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
       sum=(sum+((int)s[j]-48)*pow(base,i));
      }
      else if(s[j]>=65){
      sum+=(((int)(s[j]-55))*pow(base,i));
      }
   }
 return sum;
}
void decimaltoAny(string s,int base){
  int num=0;
  for(int i=0;i<s.size();i++) { num=(num*10+s[i]-48);}
  vector<char> number;
  char ch;
  int temp;
  while(num!= 0){
      temp=num%base;
      if(temp > 9){
        ch=(char)(temp+55);
        number.push_back(ch);
      }
      else if(temp <= 9){ ch=(char)(temp+48);
           number.push_back(ch);
      }
    num=num/base;
  }
  reverse(number.begin(),number.end());
  for(int i=0;i<number.size();i++) cout<<number[i];
}

void decimaltoAny(long int num,int base){
  vector<char> number;
  char ch;
  long int temp;
  while(num!= 0){
      temp=num%base;
      if(temp > 9){
        ch=(char)(temp+55);
        number.push_back(ch);
      }
      else if(temp <= 9){ ch=(char)(temp+48);
           number.push_back(ch);
      }
      num=num/base;
  }
  reverse(number.begin(),number.end());
for(int i=0;i<number.size();i++) cout<<number[i];
}

int main(){
  string s;
  int a,b,t;
    cin>>t;
       while(t--){
            s.clear();
         long int num;
           cin>>s;
           cin>>a;
           cin>>b;
          // cout<<s;
           if(s[0]=='0'){cout<<0;}
           if(a==10){decimaltoAny(s,b);}
           else{
               num=anytoDecimal(s,a);
               //cout<<num;
               decimaltoAny(num,b);
           }
          cout<<endl;
       }
return 0;  
}
