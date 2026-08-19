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


    string s; cin >> s;

    int ans = 1, cnt = 1;
    for(int i = 1; i < (int)s.size(); i++) {
        if(s[i] == s[i-1]) {
            cnt++;
        } else {
            ans = max(ans, cnt);
            cnt = 1;
        }
    }
    ans = max(ans, cnt);

    cout << ans << "\n";
    return 0;
}