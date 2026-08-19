// spoj
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
        #define test ll cse;scanf("%lld",&cse);for(ll i=1;i<=cse;i++)
// https://www.sciencedirect.com/science/article/pii/S0095895600919956
int main(){
        ll n,ans;
        test{
                ans = 0;
                scanf("%lld",&n);
                ans += (n*n);
                ans -= (n&1)?(1):(4);
                ans *= n;
                ans /= 24;
                printf("%lld\n",ans);
        }

        return 0;
}