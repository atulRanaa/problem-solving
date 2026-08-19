#include <bits/stdc++.h>

using namespace std;
#define ll long long
#define mod 1000000007
#define max_sz 200000
#define S second
#define F first


bool comp(pair<ll,ll> a, pair<ll,ll> b){
    return (a.F < b.F) || (a.F == b.F && a.S > b.S);
}
int main()  {
    int n,q;  scanf("%d",&q);
    
    while(q--){
        scanf("%d",&n);
        
        ll s,e; pair<ll,ll> fish[n]; ll eating[n];
        for(int i=0;i<n;i++){
            scanf("%lld %lld",&s,&e); fish[i] = {e,s};
        }
        sort(fish, fish+n, comp);
        for(int i=0;i<n;i++) eating[i] = fish[i].F;
        
        int ans = 1;
        for(int i=0;i<n;i++){
            ans = max( ans, (int)distance(eating+i,lower_bound(eating,eating+n, fish[i].S-1)) );
        }
        printf("%d\n",ans);
    }
    return 0;
}
