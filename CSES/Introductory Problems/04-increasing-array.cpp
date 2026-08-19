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


    ll n; cin >> n;
    vector<ll> arr(n);

    for(int i = 0; i < n; i++) {
        cin >> arr[i];
    }

    ll ans = 0;
    for(int i = 1; i < n; i++) {
        if(arr[i] < arr[i-1]) {
            ans += arr[i-1] - arr[i];
            arr[i] = arr[i-1];
        }
    }
    cout << ans << "\n";
    return 0;
}