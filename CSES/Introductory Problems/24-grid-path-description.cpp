#include <cstdint>
#include <vector>
#include <string>
#include <cstring>
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

constexpr int dx8[8] = {-1,-1,-1, 0, 1, 1, 1, 0};
constexpr int dy8[8] = {-1, 0, 1, 1, 1, 0,-1,-1};
constexpr int dx4[4] = {-1, 0, 1, 0};
constexpr int dy4[4] = { 0,-1, 0, 1};
constexpr char dirs_c[4] = {'U','L','D','R'};

bool within(int i, int j, int n, int m) {
    return i >= 0 && j >= 0 && i < n && j < m;
}

const int n = 7;

bool visited[n][n];
int ans = 0;

string input;

void solve(int i, int j, int itr, vector<string> &grid) {
    if(i == n-1 && j == 0) {
        if(itr == n*n - 1)
            ans++;
        return;
    }

    // created components
    uint8_t mask = 0;
    for (int k = 0; k < 8; k++) {
        int x = i + dx8[k], y = j + dy8[k];
        if (within(x, y, n, n) && !visited[x][y])
            mask |= (1u << k);
    }
    uint8_t rotated = (mask >> 1) | ((mask & 1) << 7);
    if (__builtin_popcount((uint8_t)(mask ^ rotated)) > 2)
        return;


    for(int d = 0; d < 4; d++) {
        if(input[itr] != '?' && input[itr] != dirs_c[d])
            continue;
        int next_i = i + dx4[d];
        int next_j = j + dy4[d];

        if(!within(next_i, next_j, n, n) || visited[next_i][next_j])
            continue;

        visited[next_i][next_j] = true;
        solve(next_i, next_j, itr + 1, grid);
        visited[next_i][next_j] = false;
    }
}

int main() {
    fast;

    memset(visited, false, sizeof(visited));
    cin >> input;

    vector<string> grid(n);

    visited[0][0] = true;
    solve(0, 0, 0, grid);

    cout << ans << "\n";
    return 0;
}