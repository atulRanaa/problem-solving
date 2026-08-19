        #include <bits/stdc++.h>
             
        using namespace std;
         
        typedef long long ll; 
        const ll N = 3e5+5;
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

int continuous1(vector<int> &dp,int n,int k){
    int l=1,r=1,zero=0;
    int ans=0;
    while(r <= n){
        if(r <=n && zero <= k){
            if(dp[r]==0) zero++;
            r++;
        }
        if(l <=n && zero > k){
            if(dp[l]==0) zero--;
            l++;
        }
        ans = max(ans, r-l);

    }
    return ans;
}

int main(){

    test{
        int n,k; cin>>n>>k;
        vector<int> v(n+1);
        for(int i=1;i<=n;i++) cin>>v[i];

        cout<< continuous1(v, n,k) <<"\n";
    }
    return 0;
}