#include<iostream>
#include<string.h>
using namespace std;

int main(){
   string s="1234";
  int num=0;
  for(int i=0;i<s.size();i++) { cout<<s[i]<<" "<<(int)(s[i]-48)<<endl;}
  cout<<num;
return 0;
}
