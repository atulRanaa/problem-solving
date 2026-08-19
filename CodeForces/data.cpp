#include<iostream>
#include<fstream>
#include<iomanip>
using namespace std;
long long unsigned int fib(int t1,int t2,int n)
{
 
  long long unsigned int f[n+1];
  int i;
  f[0] = 0;
  f[1] = t1;
  f[2] = t2;
  for (i=3;i<=n;i++){ f[i] = f[i-2] + f[i-1]*f[i-1];}
  return f[n];
}
int main(){
 // long double num;
  int t1,t2,n;
  cout<<"enter the numbers:\n";
  cin>>t1>>t2>>n;
  ofstream fout("integer.txt",ios::out|ios::app);
   for(int i=3;i<=n;i++)
   {  
     // num=fib(t1,t2,i);
      fout<< setprecision(0);
      fout<< fixed <<fib(t1,t2,i)<<", ";
   }
   //num=fib(t1,t2,n);
   fout.close();
   return 0;
}
