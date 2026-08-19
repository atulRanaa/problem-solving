#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const ll N = 1e6 + 5;
const ll MOD = 1e9 + 7;
#define all(x) x.begin(),x.end()
#define pii pair<int,int>
#define pis pair<ll,string>
#define F first
#define S second
#define LCM(a,b) ((a*b)/__gcd(a,b))
#define inf 1e15
#define test ll cse;cin>>cse;for(ll _i=1;_i<=cse;_i++)
#define PI 3.14159265
#define fast ios_base::sync_with_stdio(false);cin.tie(NULL);cout.tie(NULL);
const double EPS = 1E-9;
typedef vector< vector<double> > matrix;
typedef vector<int> vi;




ll isPrime(ll n) {
    ll cnt = 0;
    if(n % 2 == 0) cout << 2 << "\n";
    if(n % 3 == 0) cout << 3 << "\n";

    for (ll i = 5; i * i <= n; i = i + 6) {
        if(n % i == 0) cout << i << "\n";
        if(n % (i+2) == 0) cout << i + 2 << "\n";
    }
    return n;
}

ll powerOfPrimeDivisors(ll n) {
    ll cnt = 0;
    ll tmp = n;
    while(n % 2 == 0) { n /= 2; cnt++; cout << 2 << " ";}
    while(n % 3 == 0) { n /= 3; cnt++; cout << 3 << " ";}

    for (ll i = 5; i * i <= tmp; i = i + 6) {
        while(n % i == 0) { n /= i; cnt++; cout << i << " ";}
        while(n % (i+2) == 0) { n /= (i+2); cnt++; cout << i+2 << " ";}

        // if(i > n) break;
    }
    if(n > 1) cnt++, cout << n << ":";
    return cnt;
}

const int MAXN = 1005;

ll ar[MAXN][MAXN][5];
int main() {
    fast;

    ll x = 212072634227239451;

    // cout << 107LL * 107 * 1361 * 1361 * 10000019 << "\n";
    // cout << powerOfPrimeDivisors(x) << "\n";
    // cout << powerOfPrimeDivisors(107) << "\n";
    // cout << powerOfPrimeDivisors(1361) << "\n";
    // cout << powerOfPrimeDivisors(10000019) << "\n";
    int n, m;

    cin >> n >> m;
    for(int i = 0; i < n; i++) for(int j = 0; j < m; j++) {
        cin >> ar[i][j][0];

        ar[i][j][1] = ar[i][j][2] = ar[i][j][3] = ar[i][j][4] = 0;
        ll tmp = ar[i][j][0];

        while(tmp % 107 == 0) tmp /= 107, ar[i][j][1]++;
        while(tmp % 1361 == 0) tmp /= 1361, ar[i][j][2]++;
        while(tmp % 10000019 == 0) tmp /= 10000019, ar[i][j][3]++;
    }

    if(ar[n-1][m-1][0] % x == 0) {
        cout << 0 << "\n";
        return 0;
    }


    ar[0][0][4] = 1;
    for(int i = 0; i < n; i++) for(int j = 0; j < m; j++) {

    }
    return 0;
}