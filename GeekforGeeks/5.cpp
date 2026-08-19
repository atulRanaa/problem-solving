        #include <bits/stdc++.h>
             
        using namespace std;
         
        typedef long long ll; 
        const ll N = 1e5+5;
        const ll MOD = 1000000007;
        #define all(x) x.begin(),x.end()
        #define pii pair<ll,ll>
        #define pis pair<ll,string>
        #define F first
        #define S second
        #define LCM(a,b) ((a*b)/__gcd(a,b))
        #define inf 1e15
        #define test ll cse;cin>>cse;for(ll _i=1;_i<=cse;_i++)
        #define PI 3.14159265
        #define fast ios_base::sync_with_stdio(false);cin.tie(NULL);
        const double EPS = 1E-9;
        typedef vector<vector<double>> matrix;
        typedef vector<int> vi;

int prime[N];

struct Query{ 
    int l, r, idx;
};
int bit[N];
Query Q[N];

bool cmp(Query x, Query y){
    return  (x.r<y.r) || (x.r==y.r && x.l<y.l);
} 
void update(int idx, int val, int n){ 
    for (; idx <= n; idx += idx&-idx)   bit[idx] += val; 
} 
int query(int idx,int n){ 
    int sum = 0; 
    for (; idx>0; idx-=idx&-idx) sum += bit[idx]; 
    return sum; 
} 
void answeringQueries(ll arr[],int n,int q){ 
    
    memset(bit, 0, sizeof(bit)); 
    unordered_map<ll,int> last_visit;

    int ans[q]; 
    int j = 0;
    sort(Q,Q+q, cmp);
    for (int i=1; i<=n; i++){ 
        if (last_visit[arr[i]]) 
            update (last_visit[arr[i]], -1, n); 
        
        last_visit[arr[i]] = i; 
        update(i, 1, n); 
  
        while (j < q && Q[j].r == i){
            int L =  Q[j].l;
            int R =  Q[j].r;
            ans[Q[j].idx] = query(R, n)-query(L-1, n); 
            j++; 
        } 
    }
    for (int i=0; i<q; i++) 
        cout << ans[i] << "\n"; 
} 
ll Exp(int x,int n){
    ll ans = 1, X =x,N= n;
    while(N > 0){
        if(N&1) ans = (ans*X);
        X = (X*X);
        N >>= 1;
    }
    return ans;
}
ll solve(ll num){
    if(num==0) return 1;


    unordered_map<int,int> mapp;
    while(num != 1){
        mapp[ prime[num]]++;
        num /= prime[num];
    }
    // number of factors
    //int res = 1;for(auto ele: mapp) res *= (ele.S+1);

    // sum of divisors/factors
    ll ans = 1;
    for(auto ele: mapp){
        ll ai = ele.F;
        ll pi = ele.S;

        ans *= (Exp(ai,pi+1)-1)/(ai-1);
    }
    return ans;
}

void sieve(){
	memset(prime,0,sizeof(prime));
    prime[0] = prime[1] = 1;
    
    for(int i = 2; i <= N; i++){
        if(prime[i]==0){
            for(int j=i; j <= N;j+=i) prime[j] = i;
        }
    }
}
int main(){

    fast;
	sieve();
	
    int n,q; cin>>n>>q;

    ll ar[N], sod[N];
    for(int i=1;i<=n;i++) cin>>ar[i];
    
    for(int i=1;i<=n;i++)
        sod[i] = solve(ar[i])-ar[i];
    
    int l,r;
    for(int i=0;i<q;i++){
        cin>>l>>r; Q[i] = {l+1,r+1, i};
    }
    
    answeringQueries(sod, n, q);
    return 0;
}