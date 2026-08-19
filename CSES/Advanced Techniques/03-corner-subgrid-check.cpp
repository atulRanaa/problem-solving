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
#include <utility>
#include <type_traits>
#include <sstream>
#include <iomanip>
#include <cstring>
#include <bitset>
#include <cstdint>

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

void solve(vector<ll> &arr, vector<ll> &ele, ll &x) {
    int n = (int) arr.size();
    
    for(int i = 0; i < (1 << n); i++) {

        ll sum = 0;
        bool f = true;
        for(int j = 0; j < n; j++) {
            if(i & (1 << j)) {
                sum += arr[j];
            }

            if(sum > x) {
                f = false;
                break;
            }
        }

        if(f) ele.push_back(sum);
    }
}

int main() {
    fast;
 
    int n, k;
    cin >> n >> k;
 
    vector<vector<vector<uint16_t>>> cols(26, vector<vector<uint16_t>>(n));
    for (int i = 0; i < n; i++) {
        string s; cin >> s;
        for (int j = 0; j < n; j++)
            cols[s[j]-'A'][i].push_back(j);
    }
 
    // seen[p*n + q] = last character c that marked column pair (p, q)
    vector<uint8_t> seen(n * n, -1);
    int max_pairs = (long long)n * (n-1) / 2;   // pigeonhole threshold
 
    for (int c = 0; c < k; c++) {
        bool found = false;
        int pairs_added = 0;
 
        for (int i = 0; i < n && !found; i++) {
            auto& ci = cols[c][i];
            int m = ci.size();
            if (m < 2) continue;
 
            for (int p = 0; p < m && !found; p++) {
                for (int q = p+1; q < m; q++) {
                    int id = ci[p] * n + ci[q];
 
                    if (seen[id] == c) {
                        found = true;
                        break;
                    }
 
                    seen[id] = c;
                    if (++pairs_added > max_pairs) {
                        found = true;
                        break;
                    }
                }
            }
        }
 
        cout << (found ? "YES" : "NO") << "\n";
    }
    return 0;
}