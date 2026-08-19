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

bool within(int i, int j, int n, int m) {
    return i >= 0 && j >= 0 && i < n && j < m;
}

void dfs(int i, int j, ll &n, ll prev, vector< vector<bool> > &seen, vector< vector<ll> > &arr) {

    if(!within(i, j, n, n))
        return;
    if(seen[i][j])
        return;

    seen[i][j] = true;
    arr[i][j] = prev + 1;

    dfs(i - 2, j - 1, n, prev + 1, seen, arr);
    dfs(i - 1, j - 2, n, prev + 1, seen, arr);
    
    dfs(i + 2, j - 1, n, prev + 1, seen, arr);
    dfs(i - 1, j + 2, n, prev + 1, seen, arr);
    
    dfs(i + 2, j + 1, n, prev + 1, seen, arr);
    dfs(i + 1, j + 2, n, prev + 1, seen, arr);
    
    dfs(i - 2, j + 1, n, prev + 1, seen, arr);
    dfs(i + 1, j - 2, n, prev + 1, seen, arr);
}

int main() {
    fast;

    ll n; cin >> n;

    vector< vector<ll> > arr(n, vector<ll> (n, inf));
    vector< vector<bool> > seen(n, vector<bool> (n, false));

    arr[0][0] = 0;
    // dfs(0, 0, n, -1LL, seen, arr);

    queue< pair<int, int> > Q;
    Q.push({0,0});

    int steps = 0;
    while(!Q.empty()) {


        int sz = Q.size();
        for(int k = 0; k < sz; k++) {
            auto [i, j] = Q.front();
            Q.pop();

            if(!within(i, j, n, n))
                continue;
            if(seen[i][j])
                continue;
            
            seen[i][j] = true;
            arr[i][j] = steps;
            
            Q.push({i + 1, j + 2});
            Q.push({i + 2, j + 1});

            Q.push({i - 1, j - 2});
            Q.push({i - 2, j - 1});

            Q.push({i - 1, j + 2});
            Q.push({i + 2, j - 1});

            Q.push({i + 1, j - 2});
            Q.push({i - 2, j + 1});
        }

        steps++;
    }

    for(const auto &row: arr) {
        for(const auto &e: row)
            cout << e << " ";
        cout << "\n";
    }
    return 0;
}