#include <climits>
#include <functional>
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
#include <utility>
#include <type_traits>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <cassert>

using namespace std;
typedef long long ll;
const ll MOD = 1e9 + 7;
#define all(x) x.begin(),x.end()
#define LCM(a,b) ((a*b)/__gcd(a,b))
#define inf 2e18
#define test ll cse;cin>>cse;for(ll _i=1;_i<=cse;_i++)
#define PI 3.14159265
#define fast ios_base::sync_with_stdio(false);cin.tie(NULL);cout.tie(NULL);
#define loop(i, n) for(int i = 0; i < n; i++)
const double EPS = 1E-9;

template <typename T, typename = void> struct is_iter : false_type {};
template <typename T> struct is_iter<T, void_t<decltype(begin(declval<T&>()))>> : true_type {};

template <typename T>
constexpr bool is_str_v = is_same_v<decay_t<T>, string> || is_same_v<decay_t<T>, char> || is_same_v<decay_t<T>, const char*>;

template <typename T> int width_of(const T& x) { ostringstream o; o << x; return (int)o.str().size(); }

template <typename A, typename B> void pr(const pair<A, B>& p, int d = 0);

template <typename T> void pr(const T& x, int d = 0) {
    using D = decay_t<T>;
    if constexpr (is_str_v<D>) cerr << x;
    else if constexpr (is_iter<T>::value) {
        using V = decay_t<decltype(*begin(x))>;
        if constexpr (is_iter<V>::value && !is_str_v<V>) {       // matrix: container of containers
            int w = 0;
            for (auto& row : x) for (auto& e : row) w = max(w, width_of(e));
            for (auto& row : x) {
                cerr << string(d * 2, ' ');
                for (auto& e : row) cerr << setw(w) << e << ' ';
                cerr << "\n";
            }
        } else {                                                  // flat container
            for (auto& e : x) { pr(e, d); cerr << ' '; }
        }
    } else cerr << x;
}

template <typename A, typename B> void pr(const pair<A, B>& p, int d) {
    pr(p.first, d); cerr << ":"; pr(p.second, d);
}

template <typename T, typename... R> void pr_all(const T& f, const R&... r) {
    pr(f); if constexpr (sizeof...(r) > 0) { cerr << "  "; pr_all(r...); }
}

#ifdef LOCAL
#define dbg(...) cerr << #__VA_ARGS__ << ":\n", pr_all(__VA_ARGS__), cerr << "\n"
#else
#define dbg(...)
#endif


ll binpow(ll a, ll b, ll m) {
    a %= m;
    if (a < 0) a += m;
    ll res = 1 % m;
    while (b > 0) {
        if (b & 1)
            res = res * a % m;
        a = a * a % m;
        b >>= 1;
    }
    return res;
}

ll binpow(ll a, ll b) {
    ll res = 1;
    while (b > 0) {
        if (b & 1)
            res = res * a;
        a = a * a;
        b >>= 1;
    }
    return res;
}

int factors(int n) {
    map<int, int> prime;

    while(n % 2 == 0) {
        prime[2]++;
        n /= 2;
    }
    for(ll i = 3; i * i <= n; i += 2) {
        while(n % i == 0) {
            prime[i]++;
            n /= i;
        }
    }

    if(n > 1)
        prime[n]++;

    int ans = 1;
    for(auto &[_, p]: prime) {
        ans *= (p + 1);
    }

    return ans;
}

map<ll, ll> prime_factors(ll n) {
    map<ll, ll> prime;

    while(n % 2 == 0) {
        prime[2]++;
        n /= 2;
    }
    for(ll i = 3; i * i <= n; i += 2) {
        while(n % i == 0) {
            prime[i]++;
            n /= i;
        }
    }

    if(n > 1)
        prime[n]++;

    return prime;
}

ll ap(ll n, ll mod) {
    n %= MOD;
    ll a = (n * (n + 1)) % MOD;
    ll b = binpow(2, MOD - 2, MOD);

    return (a * b) % MOD;
}

bool is_prime(ll n) {
    if(n > 2 && n % 2 == 0) return false;
    if(n > 3 && n % 3 == 0) return false;

    for (ll i = 5; i * i <= n; i = i + 6) {
        if(n % i == 0) return false;
        if(n % (i+2) == 0) return false;
    }
    return true;
}

ll mod_inv(ll a, ll mod) { return binpow(a, mod - 2, mod); }

struct GaussMod {
    int n, m;
    ll mod;
    vector<vector<int>> a;

    GaussMod(int n, int m, ll mod = MOD)
        : n(n), m(m), mod(mod), a(n, vector<int>(m + 1, 0)) {}
    GaussMod(vector<vector<int>> mat, ll mod = MOD)
        : n(mat.size()), m(mat[0].size() - 1), mod(mod), a(mat) {}

    int eliminate(vector<int>& pivot_col) {
        pivot_col.assign(n, -1);
        int row = 0;
        for (int col = 0; col < m && row < n; col++) {
            // Find any nonzero in column (mod arithmetic — no meaningful max)
            int best = -1;
            for (int i = row; i < n; i++)
                if (a[i][col] != 0) { best = i; break; }
            if (best == -1) continue;

            swap(a[row], a[best]);
            pivot_col[row] = col;

            ll pivot_inv = mod_inv(a[row][col], mod);
            // Row Echelon Form
            for (int i = row + 1; i < n; i++) {
                if (a[i][col] == 0) continue;
                
                ll factor = (1LL * a[i][col] * pivot_inv) % mod;
                
                for (int j = col; j <= m; j++) {
                    a[i][j] = (a[i][j] - factor * a[row][j]) % mod;
                    if (a[i][j] < 0) a[i][j] += mod;
                }
            }

            row++;
        }
        return row;
    }

    // Returns: 0=no solution, 1=unique, 2=infinite
    int solve(vector<ll>& x, vector<int>& free_vars) {
        vector<int> pivot_col;
        int rank = eliminate(pivot_col);

        // Check for inconsistency (0 = non-zero constant)
        for (int i = rank; i < n; i++)
            if (a[i][m] != 0) return 0;

        x.assign(m, 0);
        vector<bool> is_pivot(m, false);
        for (int i = 0; i < rank; i++)
            if (pivot_col[i] != -1) is_pivot[pivot_col[i]] = true;
        for (int j = 0; j < m; j++)
            if (!is_pivot[j]) free_vars.push_back(j);
            
        // Back-Substitution pass
        for (int i = rank - 1; i >= 0; i--) {
            int c = pivot_col[i];
            if (c == -1) continue;

            ll sum = a[i][m];
            for (int j = c + 1; j < m; j++) {
                sum = (sum - 1LL * a[i][j] * x[j]) % mod;
                if (sum < 0) sum += mod;
            }
            x[c] = (sum * mod_inv(a[i][c], mod)) % mod;
        }

        return free_vars.empty() ? 1 : 2;
    }
};

int main() {
    fast;
    ll n, m; cin >> n >> m;

    vector< vector<int> > equations(n, vector<int> (m + 1));

    for(int i = 0; i < n; i++) {
        loop(j, m + 1) {
            cin >> equations[i][j];
        }
    }
    GaussMod sol(equations);

    vector<ll> x;
    vector<int> free_vars;
    int ans = sol.solve(x, free_vars);
    
    // dbg(ans);
    // dbg(x);
    // dbg(free_vars);

    if(ans == 0) {
        cout << -1 << "\n";
    } else {
        for(auto e: x) {
            cout << e << " ";
        }
        cout << "\n";
    }
    return 0;
}