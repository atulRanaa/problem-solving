#include <iostream>
#define ull unsigned long long int
using namespace std;

ull digit_sum(ull n){
	ull sum=0;
	  while(n){
	  	 sum+=(n%10);
	  	 n/=10;
	  }
  return sum;
}
int main()
{
    int t,d;
    long long int len;
    ull sum1=0;
    long int m;
    cin>>t;
           while(t--){
           	cin>>m;
           	sum1=0;
           	     while(m--){
           	     	cin>>len>>d;
           	     	if(d>0) sum1+=len*d;
           	     }
           	     while(sum1/10 != 0){
           	     	     	   sum1=digit_sum(sum1);
           	                        }
           	 cout<<sum1<<endl;
           }
    return 0;
}

