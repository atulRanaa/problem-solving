#include<iostream>
#include<math.h>
using namespace std;
#define ull unsigned long long
int main()
{
  int d,t;
  long long int len,i;
  long int n;
  cin>>t;
  while(t--){
  	 ull sum=0,p;
     cin>>n;
     for(i=0;i<n;i++){
           cin>>len>>d;
           sum=sum+(len*d);
     }
     if(sum==0) cout<<0<<endl;
     else{
            if(sum>9 && sum%9!=0) {
               p=sum/9;
               sum=(sum-p*9);
               cout<<sum<<endl;
             }
            else if(sum%9==0) cout<<9<<endl;
            else cout<<sum<<endl;
         }
 }
return 0;
}
