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
#define inf 1e15
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

template <typename T, T MOD_VAL = 0>
struct Matrix {
private:
    int n, m;
    vector<vector<T>> a;

public:
    // Constructors
    Matrix() : n(0), m(0) {}
    Matrix(int n, int m, T val = 0) : n(n), m(m), a(n, vector<T>(m, val)) {}
    Matrix(vector<vector<T>> v) : n(v.size()), m(v[0].size()), a(v) {}

    // Access
    vector<T>&       operator[](int i)       { return a[i]; }
    const vector<T>& operator[](int i) const { return a[i]; }

    // Identity matrix (n×n)
    static Matrix identity(int n) {
        Matrix I(n, n, 0);
        for (int i = 0; i < n; i++) I[i][i] = 1;
        return I;
    }

    // Zero matrix
    static Matrix zero(int n, int m) { return Matrix(n, m, 0); }

    Matrix operator+(const Matrix& B) const {
        assert(m == B.n);
        Matrix C(n, B.m, 0);
        for (int i = 0; i < n; i++)
            for (int k = 0; k < m; k++) {
                if (a[i][k] == 0) continue;   // skip zero rows (optimization)
                for (int j = 0; j < B.m; j++) {
                    C[i][j] += a[i][k] * B[k][j];
                    if constexpr (MOD_VAL != 0)
                        C[i][j] %= MOD_VAL;
                }
            }
        return C;
    }

    Matrix operator+(const Matrix& B) const {
        assert(n == B.n && m == B.m);
        Matrix C(n, m);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++) {
                C[i][j] = a[i][j] + B[i][j];
                if constexpr (MOD_VAL != 0)
                    C[i][j] %= MOD_VAL;
            }
        return C;
    }

    Matrix operator*(T scalar) const {
        Matrix C(n, m);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++) {
                C[i][j] = a[i][j] * scalar;
                if constexpr (MOD_VAL != 0)
                    C[i][j] %= MOD_VAL;
            }
        return C;
    }

    bool operator==(const Matrix& B) const { return a == B.a; }

    vector<T> apply(const vector<T>& v) const {
        assert(m == (int)v.size());
        vector<T> res(n, 0);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < m; j++) {
                res[i] += a[i][j] * v[j];
                if constexpr (MOD_VAL != 0)
                    res[i] %= MOD_VAL;
            }
        return res;
    }

    Matrix pow(ll k) const {
        assert(n == m);   // must be square
        Matrix result = identity(n);
        Matrix base   = *this;
        while (k > 0) {
            if (k & 1) result = result * base;
            base = base * base;
            k >>= 1;
        }
        return result;
    }

    void print() const {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++)
                cerr << a[i][j] << " \n"[j == m-1];
        }
    }
};

using Mat = Matrix<ll, MOD>;

int main() {
    fast;
    ll n; cin >> n;

    if(n == 0) {
        cout << 0 << "\n";
        return 0;
    }

    Mat fab({{1,1}, {1, 0}});
    auto result = fab.pow(n - 1);
    cout << result[0][0] << "\n";
    return 0;
}