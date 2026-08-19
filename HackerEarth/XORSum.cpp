// hackerearth
        #include <bits/stdc++.h>
             
        using namespace std;
         
        typedef long long ll; 
        const int N = 1e5+5; 
        #define MOD 1000000007
        #define pii pair<int,int>
        #define pis pair<int,string>
        #define F first
        #define S second
        #define LCM(a,b) ((a*b)/__gcd(a,b))
        #define inf 1e15
        int i,j,k;

int BIT[N][42];
ll ar[N];
ll inv2, inv6;

ll modexp(ll x,ll n){
    ll res=1LL;
    while(n > 0){
        if(n&1) res = (res*x) % MOD;
        x = (x*x) % MOD;    n >>= 1; 
    }
    return res;
}
ll nCr(ll n, ll r){
    
    if(r > n) return 0LL;
    if(r== n) return 1LL;
    ll ans;
    ll a=1LL;
    ll b=(r==2)?(inv2):(inv6);

        for(ll i=n-r+1;i<=n;i++) a=(a*i) % MOD;
        ans = (a*b) % MOD;
    return ans;
}
ll power2[65];
void pow2(){
    power2[0]=1;
    for(i=1;i<65;i++) power2[i]=(power2[i-1]+power2[i-1])%MOD;
}
int main(){
    memset(BIT, 0, sizeof BIT);
    memset(ar, 0, sizeof ar);
    pow2();
    inv2 = modexp(2, MOD-2);
    inv6 = modexp(6, MOD-2);

    int n; scanf("%d",&n);
    for(i=1;i<=n;i++) scanf("%lld",ar+i);
    
    for(i=1;i<=n;i++) for(j=0;j<42;j++)
        BIT[i][j] = ((ar[i]>>j)&1) + BIT[i-1][j];

    int q,l,r; scanf("%d",&q);
    while(q-- > 0){
        scanf("%d %d",&l,&r);
        ll ans = 0;
        for(i=0;i<42;i++){
            int k = BIT[r][i]-BIT[l-1][i];
            ans +=  (nCr(k,3)*power2[i])%MOD;
            ans %= MOD;
            ans +=  ((k*nCr(r-l+1-k,2))*power2[i])%MOD;
            ans %= MOD;
        }
        printf("%lld", ans);
    }
    for(i=1;i<=n;i++){ for(j=0;j<42;j++) cout<<BIT[i][j]; cout<<"\n";}
    return 0;
}