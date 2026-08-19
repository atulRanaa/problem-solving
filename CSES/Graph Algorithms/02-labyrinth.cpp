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
#include <cassert>

using namespace std;
typedef long long ll;
const ll MOD = 1e9 + 7;
#define all(x) x.begin(),x.end()
#define LCM(a,b) ((a*b)/__gcd(a,b))
#define inf 1e18
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

bool within(int i, int j, int n, int m) {
    return i >= 0 && j >= 0 && i < n && j < m;
}

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

// left, up, right, down
vector<int> dx = {0, -1, 0, 1};
vector<int> dy = {-1, 0, 1, 0};
string dir = "LURD";

string ans;
void dfs(int i, int j, vector<string> &grid, vector< vector<bool> > &visited, string &path) {
    int n = grid.size();
    int m = grid[0].size();

    if(!within(i, j, n, m))
        return;

    if(grid[i][j] == '#' || visited[i][j])
        return;

    if(grid[i][j] == 'B') {
        if(ans.empty())
            ans = path;
        else 
            ans = min (ans, path);
        
        return;
    }

    visited[i][j] = true;
    for(int d = 0; d < 4; d++) {
        path.push_back(dir[d]);
        dfs(i + dx[d], j + dy[d], grid, visited, path);
        path.pop_back();
    }
}

void bfs(int start_i, int start_j, vector<string> &grid) {
    int n = grid.size();
    int m = grid[0].size();

    vector< vector< pair<int, int> > > visited (
        n,
        vector< pair<int, int> > (m, {-1, -1})
    );
    vector< vector< char > > direction (n, vector< char > (m));

    queue< pair<int, int> > Q;
    pair<int, int> dest = {-1, -1};

    Q.push({start_i, start_j});
    visited[start_i][start_j] = {start_i, start_j};


    while(!Q.empty()) {
        auto [i, j] = Q.front();
        Q.pop();

        // dbg(i, j);

        if(grid[i][j] == 'B') {
            dest = {i, j};
            break;
        }

        for(int d = 0; d < 4; d++) {
            int x = i + dx[d];
            int y = j + dy[d];

            if(within(x, y, n, m) && visited[x][y].first == -1 && grid[x][y] != '#') {
                visited[x][y] = {i, j};
                direction[x][y] = dir[d];

                Q.push({x,y});
            }
        }
    }

    if(dest.first == -1)
        return;

    string path = "";
    auto [i, j] = dest;
    while(make_pair(start_i, start_j) != make_pair(i,j)) {
        auto parent = visited[i][j];

        path += direction[i][j];
        i = parent.first;
        j = parent.second;
    }

    reverse(all(path));
    ans = path;
}

int main() {
    fast;
    ll n, m; cin >> n >> m;

    vector<string> grid(n);

    loop (i, n) {
        cin >> grid[i];
    }

    string s;
    for(int i = 0; i < n; i++) {
        for(int j = 0; j < m; j++) {
            if(grid[i][j] == 'A') {
                bfs(i, j, grid);
            }
        }
    }

    if(ans.empty()) {
        cout << "NO\n";
    } else {
        cout << "YES\n";
        cout << ans.size() << "\n";
        cout << ans << "\n";
    }
    return 0;
}