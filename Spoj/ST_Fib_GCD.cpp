    #include <bits/stdc++.h>
    using namespace std;
     
    #define N 1000005
    #define ll long long
    #define MOD 1000000007
    ll ar[4*N]={0},ST[4*N]={0};
     
    void build(int index,int start,int end){
        if(start==end){
            ST[index] = ar[ start];
            return;
        }
        int mid = (start+end)>>1;
        build(2*index,start,mid);
        build(2*index+1,mid+1,end);
        
        ST[index] = __gcd(ST[2*index], ST[2*index+1]);
    }
     
    ll query(int index,int start,int end,int l,int r){
        if(r < start || end < l) return -1;
        if(l <= start &&  end <= r) return ST[index];
     
        int mid = (start+end)>>1;
     
        ll a = query(2*index,start,mid,l,r);
        ll b = query(2*index+1,mid+1,end,l,r);
        
        if(a == -1) return b;
        if(b == -1) return a;
        return __gcd(a, b);
    }
    unordered_map<ll, ll> Fib;
    ll fib(ll n){
        if(n == 0) return 0;
        if(n == 1 || n == 2) return (Fib[n] = 1);
        if (Fib[n]) return Fib[n];
        int k = (n & 1)? (n+1)>>1 : n>>1;
        Fib[n] = (n & 1)? (fib(k)*fib(k) + fib(k-1)*fib(k-1))%MOD
                        : ((2*fib(k-1) + fib(k))   *fib(k)  )%MOD;
        return Fib[n];
    }
    int main(){
        
        int n,q; scanf("%d %d",&n,&q);
        for(int i=1;i<=n;i++) scanf("%lld",ar+i);
        
        build(1,1,n);
        int x,y;
        while(q-- > 0){
            scanf("%d %d",&x,&y);
            ll G = query(1,1,n,x,y);
            if(Fib[G]) printf("%lld\n",Fib[G]);
            else{
                fib(G);
                printf("%lld\n",Fib[G]);
            }
        }
        return 0;
    }