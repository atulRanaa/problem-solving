#include <climits>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <numeric>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <stack>

using namespace std;
typedef long long ll;
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

template <typename T>
void inspect(const char* name, const T& container) {
    std::cerr << "[INSPECT] " << name << " = [ ";
    bool first = true;
    for (const auto& item : container) {
        if (!first) std::cerr << ", ";
        std::cerr << item;
        first = false;
    }
    std::cerr << " ]\n";
}
#define inspect(x) inspect(#x, x)

ll binpow(ll a, ll b, ll m) {
    a %= m;
    ll res = 1;
    while (b > 0) {
        if (b & 1)
            res = res * a % m;
        a = a * a % m;
        b >>= 1;
    }
    return res;
}

int main() {
    fast;

    test {
        ll n, a, b;
        cin >> n >> a >> b;

        ll non_tie = a + b; 
        if(a + b > n || a == n || b == n || (non_tie >= 1 && (non_tie == a || non_tie == b))) {
            cout << "NO\n";
            continue;
        }

        cout << "YES\n";
        vector< pair<int, int> > moves;
        for(int i = 1; i <= (a + b); i++) {
            ll x = i + a;
            if(x > (a + b))
                x -= (a + b);
            moves.push_back({i, x});
        }
        for(int i = a + b + 1; i <= n; i++) {
            moves.push_back({i, i});
        }

        for(const auto &e: moves) {
            cout << e.first << " ";
        }
        cout << "\n";
        for(const auto &e: moves) {
            cout << e.second << " ";
        }
        cout << "\n";
    }

    return 0;
}