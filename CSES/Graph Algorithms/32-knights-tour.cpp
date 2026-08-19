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
template <typename T> using min_heap = priority_queue<T, vector<T>, greater<T>>;


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
    return i >= 1 && j >= 1 && i <= n && j <= m;
}

vector<int> dx = {-2,-1, 1, 2, 2, 1, -1, -2};
vector<int> dy = {1,2, 2, 1, -1, -2, -2, -1};


int choice(int i, int j, int n, int m, vector< vector<int> > &visited) {
    int count = 0;
    for(int d = 0; d < 8; d++) {
        int x = i + dx[d];
        int y = j + dy[d];

        if(within(x, y, n, m) && !visited[x][y])
            count++;
    }
    return count;
}

void knightour(int x, int y, int n, int m, vector< vector<int> > &board) {
    int idx = 1;

    queue< pair<int, int> > Q;
    Q.push({x, y});
    while(!Q.empty()) {
        auto [i, j] = Q.front();
        Q.pop();

        board[i][j] = idx++;

        int w = INT_MAX;
        pair<int, int> next;
        for(int d = 0; d < 8; d++) {
            x = i + dx[d];
            y = j + dy[d];

            if(within(x, y, n, m) && !board[x][y] && choice(x, y, n, m, board) < w) {
                w = choice(x, y, n, m, board);
                next = {x, y};
            }
        }

        if(w != INT_MAX) {
            Q.push(next);
        }
    }
}

bool isvalid(int n, int m, vector< vector<int> > &board) {
    for(int i = 1; i <= n; i++) {
        for(int j = 1; j <= m; j++) 
            if(board[i][j] == 0) return false;
    }

    return true;
}

vector< vector<int> > ans;
bool found;

void solve(int idx, int i, int j, int &n, int &m, vector< vector<int> > &board) {
    if(board[i][j]) return;
    board[i][j] = idx;

    if(idx == n * m) {
        ans = board;
        found = true;
        return;
    }
    if(found)
        return;

    vector< vector<int> > moves;
    for(int d = 0; d < 8; d++) {
        int x = i + dx[d];
        int y = j + dy[d];

        if(within(x, y, n, m) && !board[x][y]) {
            moves.push_back({choice(x, y, n, m, board), x, y});
        }
    }

    sort(all(moves));
    for(auto e: moves) {
        solve(idx + 1, e[1], e[2], n, m, board);
    }

    board[i][j] = 0;
}

int main() {
    fast;
    
    int x, y;
    cin >> y >> x;

    int n = 8, m = 8;
    vector< vector<int> > board(9, vector<int> (9, 0));

    solve(1, x, y, n, m, board);
    for(int i = 1; i <= n; i++) {
        for(int j = 1; j <= m; j++) cout << ans[i][j] << " ";
        cout << "\n";
    }
    return 0;
} 