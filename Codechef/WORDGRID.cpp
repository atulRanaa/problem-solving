// codechef      incomplete
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
        typedef vector<vector<ll>> matrix;
        typedef vector<ll> vi;

int main() {
    test{
        int n,k; cin >> n >> k;

        int ar[n];
        for(int i=0;i<n;i++) cin >> ar[i];
        sort(ar, ar+n);

        for(int i=0;i<n-1;i++){

            if(ar[i] > k){
                int val = ar[i]-k;
                ar[i] -= val;
                ar[i+1]-= val;
            }
        }
        ll sum=0;
        for(int i=0;i<n;i++) sum+=ar[i];
        cout << sum <<"\n";

    }
    return 0;
}