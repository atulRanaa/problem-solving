        #include <bits/stdc++.h>
             
        using namespace std;
         
        typedef long long ll; 
        const int N = 1e5+5;
        #define all(x) x.begin(),x.end()
        #define MOD 1000000007
        #define pii pair<int,int>
        #define pis pair<int,string>
        #define F first
        #define S second
        #define LCM(a,b) ((a*b)/__gcd(a,b))
        #define inf 1e15
        #define test ll cse;cin>>cse;for(ll I=1;I<=cse;I++)
        #define fast ios_base::sync_with_stdio(0);cin.tie(NULL);cout.tie(NULL);
        #define PI 3.14159265
        #define MAXN 1300005
        #define pb      push_back
        #define mp      make_pair


ll ar[N];
int n;
ll fun(ll lvl){
        ll cost=0;
        for(int i=0;i<n;i++) cost+=min(abs(lvl-ar[i]),ar[i]);
        return cost;
}
int main(){
        fast;


        cin >> n;
        for(int i=0;i<n;i++) cin>>ar[i];

        ll lo = -1, hi = 1000000000;
        ll ans=0;
        while (hi-lo > 1){
            ll mid = (hi + lo)>>1;
            ll f_mid = fun(mid) , f_mid1 = fun(mid+1);
            if (f_mid <= f_mid1){
                 hi = mid;
                 ans=f_mid;
            }
            else{
                 lo = mid;
            }

        }
        cout<<ans<<"\n";
        return 0;
} 