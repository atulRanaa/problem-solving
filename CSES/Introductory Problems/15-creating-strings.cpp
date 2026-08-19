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

vector<string> result;
void solve(int n, vector<int> &freq, string s) {
    if(n == (int)s.size()) {
        result.push_back(s);
        return;
    }

    for(int i = 0; i < 26; i++) {
        if(freq[i] > 0) {
            freq[i]--;
            solve(n, freq, s + (char)(i+'a'));
            freq[i]++;

            // solve(n, freq, s);
        }
    }
}

int main() {
    fast;

    result.clear();
    vector<int> freq(26, 0);

    string s; cin >> s;
    for(char ch: s)
        freq[ch-'a']++;

    solve((int)s.size(), freq, "");

    cout << result.size() << "\n";
    for(const auto &e: result)
        cout << e << "\n";
    return 0;
}