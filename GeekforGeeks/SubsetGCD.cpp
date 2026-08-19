#include <bits/stdc++.h>

using namespace std;
#define ll long long
#define mod 1000000007
#define max_sz 200000
#define S second
#define F first



int main()  {
    int q;  scanf("%d",&q);
    
    ll lim = (1<<q)-1;
    ll ar[ lim +5];

    for(int i=0;i<lim;i++) scanf("%lld",&ar[i]);
    
    unordered_set<ll> gd(ar,ar+lim);
    for(auto ele: gd)  printf("%lld ",ele); cout<<"\n";
    return 0;
}
