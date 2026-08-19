    
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


int main(){
    fast;

    test{
        int n; cin>>n;

        ll ar[n];
        for(int i=0;i<n;i++) cin>>ar[i];

        priority_queue<ll, vector<ll>, greater<ll> > pq;
        
        for(int i=0;i<n;i++){
            pq.push(ar[i]);
        }
        ll prev=0;
        ll sum=0;
        while( ! pq.empty()){
            ll a=0,b=0;
            a = pq.top();
            pq.pop();
            if( ! pq.empty()){
                b = pq.top();
                pq.pop();
            }
            else{
                break;
            }
            sum += (a+b);
            pq.push(a+b);
        }
        cout << sum <<"\n";
    }
}
