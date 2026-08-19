// hackerearth
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
        #define test ll cse;scanf("%lld",&cse);for(ll _i=1;_i<=cse;_i++)
        #define PI 3.14159265
        typedef vector<vector<ll>> matrix;
        typedef vector<ll> vi;

// ll modexp(ll x,ll n){
//     ll res=1LL;
//     while(n > 0){
//         if(n&1) res = (res*x) % MOD;
//         x = (x*x) % MOD;    n >>= 1; 
//     }
//     return res;
// }
// ll nCr(ll n, ll r){
    
//     if(r > n) return 0LL;
//     if(r== n) return 1LL;
//     ll ans;
//     ll a=1LL;
//     ll b=(r==2)?(inv2):(modexp(r,MOD-2));

//         for(ll i=n-r+1;i<=n;i++) a=(a*i) % MOD;
//         ans = (a*b) % MOD;
//     return ans;
// }


inline matrix indentity(ll n){
    matrix I(n, vector<ll>(n,0LL));
    for(ll i=0;i<n;++i)
        I[i][i]=1LL;
    return I;
}
inline matrix zero(ll n){
    matrix Z(n, vector<ll>(n,0LL));
    return Z;
}
matrix matrix_mult(const matrix &A, const matrix &B){
    ll n = A.size();
    matrix C = zero(n);
    for(ll k=0;k<n; ++k) for(ll i=0;i<n;++i)
            for(ll j=0;j<n;++j)
                C[i][j]= (C[i][j]+A[i][k]*B[k][j])%MOD;
    return C;
}
matrix mat_exp(matrix a, ll exp){
    ll n = a.size();
    matrix R = indentity(n);
    for(;exp >0; exp>>=1LL, a=matrix_mult(a,a)){
        if(exp&1LL)
            R = matrix_mult(R,a);
    }
    return R;
}
// void print_matrix(const matrix &m){
//     for(auto &row : m){
//         for(ll val: row) printf("%lld ",val); cout<<"\n";
//     }
// }
// void print_vector(const vi &v){
//     for(ll x: v) printf("%lld ",x); cout<<"\n";
// }
int main(){
        ll x;
        
        //matrix M = {{1,1},{1,0}};     f(n) = f(n-1)+f(n-2);
        //vector<ll> x1 = {1,0};

        //f(n) = 2f(n-1)+3f(n-2)+n7^n
        matrix M = {{2,3,1,0},{1,0,0,0},{0,0,7,7},{0,0,0,7}}; 
        vector<ll> x1 = {1,1,2*7*7,7*7};

        test{   
                scanf("%lld",&x);
                ll ans=0;

                if(x==0) ans=1;
                else if(x==1) ans=1;
                else{
                    x--;
                    matrix mn = mat_exp(M,x);
                    for(int i=0;i<1;++i) for(int j=0;j<4;++j){
                        ans += mn[i][j]*x1[j];
                        ans %= MOD;
                    }
                }
                // print_matrix(M);
                // print_matrix(mn);
                // print_vector(xn1);

                printf("Case %lld: %lld\n",_i, ans);
        }
        return 0;
}