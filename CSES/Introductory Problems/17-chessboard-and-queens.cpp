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

vector<bool> rows(8);
vector<bool> cols(8);
vector<bool> diag1(16);
vector<bool> diag2(16);

ll ans = 0;
ll N = 8;
void solve(int row, vector<string> &board) {
    if(row == N) {
        ans++;
        return;
    }
    for(int col = 0; col < N; col++) {
        // valid
        bool flag = !cols[col] && !diag1[row + col] && !diag2[row - col + N - 1];
        if(board[row][col] == '.' && flag) {
            cols[col] = diag1[row + col] = diag2[row - col + N - 1] = true;
            solve(row + 1, board);

            cols[col] = diag1[row + col] = diag2[row - col + N - 1] = false;
        }
    }
}

int main() {
    fast;

    vector<string> board(8);
    for(int i = 0; i < 8; i++) {
        cin >> board[i];
    }

    solve(0, board);
    cout << ans << "\n";

    return 0;
}