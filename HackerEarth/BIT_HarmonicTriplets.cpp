    #include <cstring>
    #include <stdio.h>
    #include <string>
    #include <iostream>
    using namespace std;

    const int N = 1e6+5;
    #define ll long long
    #define MOD 1000000007
    #define pii pair<int,int>
    #define pis pair<int,string>
    #define F first
    #define S second
    #define LCM(a,b) ((a*b)/__gcd(a,b))
    #define inf 1e15
    ll BIT[N];
    ll ar[N];
ll get(ll ind){ 
    ll x = 0;
    while(ind > 0){
        x = max( x, BIT[ind]);  ind -= ind & (-ind);
    }
    return x;
}
 
void updateBIT(int n, ll ind, ll val){ 
    while(ind <= n){
        BIT[ind] = max(BIT[ind], val);  ind += ind & (-ind);
    }
}
    int main(){
    
        int t; scanf("%d",&t);
        while(t-- > 0){

            memset(BIT, 0 ,sizeof BIT);
            ll ans[N];

            int n,q; scanf("%d %d",&n,&q);
            for(int i=1;i<=n;i++) scanf("%lld", ar+i);
            
            for(int i=1;i<=n;i++){
                ans[i] = ar[i]*get(ar[i]);
                updateBIT(100000,ar[i],ar[i]);
            }
            memset(BIT, 0 ,sizeof BIT);
            for(int i=n;i>=1;i--){
                ans[i] *= get(ar[i]);
                updateBIT(100000,ar[i],ar[i]);
            }
            int j;
            while(q-- > 0){
                scanf("%d",&j); printf("%lld\n",  ans[j]);
            }
        }
        return 0;
    }
