#include <bits/stdc++.h>
using namespace std;

typedef long long int ll;
ll ar[100010];

int main()
{
    int t;
    cin>>t;
    while(t--){
         ll n,i,j,min=0,max=0;
         cin>>n;
         for(i=0;i<n;i++) cin>>ar[i];
         sort(ar,ar+n);
         
         for(i=0,j=n-1;i<j;i++,j--)  max += abs(ar[i]-ar[j]);
         for(i=0,j=1;j<n;i+=2,j+=2)  min += abs(ar[i]-ar[j]);
         cout<<min<<" "<<max<<endl;
    }
    return 0;
}
