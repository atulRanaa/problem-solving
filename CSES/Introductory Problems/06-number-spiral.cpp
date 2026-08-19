#include <vector>
#include <string>
#include <iostream>

using namespace std;
typedef long long ll;
const ll N = 2e5 + 5;
const ll MOD = 1e9 + 7;
#define all(x) x.begin(),x.end()
#define LCM(a,b) ((a*b)/__gcd(a,b))
#define inf 1e15
#define test ll cse;cin>>cse;for(ll _i=1;_i<=cse;_i++)
#define PI 3.14159265
#define fast ios_base::sync_with_stdio(false);cin.tie(NULL);cout.tie(NULL);
const double EPS = 1E-9;
typedef vector< vector<double> > matrix;
typedef vector<int> vi;


int ar[N];
int main() {
    fast;

    test{
        ll r, c; cin >> r >> c;

        ll mx = max(r, c);
        ll num = mx * mx - (mx - 1)*(mx-1);
        if(mx&1) {
            cout << (mx-1)*(mx-1) + (r >= c ? c : num - r + 1) << "\n";
        } else {
            cout << (mx-1)*(mx-1) + (r >= c ? num - c + 1 : r) << "\n";
        }
    }
    return 0;
}