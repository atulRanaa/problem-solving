        #include <bits/stdc++.h>
             
        using namespace std;
         
        typedef long long ll;
        const int SZ = 1e6+5; 
        const int N = 3e5; 
        #define MOD 1000000007
        #define pii pair<int,int>
        #define pll pair<ll ,ll >
        #define pis pair<int,string>
        #define F first
        #define S second
        #define LCM(a,b) ((a*b)/__gcd(a,b))
        #define inf 1e15

    ll BIT[SZ][2];
    ll H[SZ], I[SZ];
pll get(int ind){ 
    pll x = {0,0};
    while(ind > 0){
        x.F = BIT[ind][0];
        x.S = BIT[ind][1];  ind -= ind & (-ind);
    }
    return x;
}
 
void updateBIT(int n, int ind, ll val){ 
    while(ind <= n){
        BIT[ind][0] += val;
        BIT[ind][1] +=   1;   ind += ind & (-ind);
    }
}
int main(){
    memset(BIT, 0, sizeof BIT);
    
    int n;  scanf("%d",&n);
    ll ans[n+1];

    for(int i=1;i<=n;i++) scanf("%lld %lld",H+i,I+i);
    for(int i=n;i>=1;i--){
        pll A = get(H[i]-1);                // HUMOURi  > HUMOURj
        pll B = get(N) -  A;                // HUMOURi <= HUMOURj
        

        ans[i] = (A.F - A.S*I[i]) + (-B.F + B.S*I[i]);

        updateBIT(N, H[i], I[i]);
    }

    for(int i=1;i<=n;i++) printf("%lld\n", ans[i]);
    return 0;
}