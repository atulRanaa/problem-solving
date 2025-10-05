#include<bits/stdc++.h>

using namespace std;


#define all(x) x.begin(),x.end()
#define fastIO ios::sync_with_stdio(false);cin.tie(0);cout.tie(0)
#define test int testcases;cin>>testcases;for(int tc=1;tc<=testcases;tc++)
typedef long long ll;

void print(string x)  { cout << '\"' << x << '\"'; }
void print(char x)  { cout << '\'' << x << '\''; }
void print(long long x)  { cout << x; }
void print(double x)  { cout << x; }
void print(bool x)  { cout << x; }
void print(int x)  { cout << x; }
 
template <class T, class V> void print(const pair<T, V> &x);
template <class T, class V> void print(const map<T, V>& mp);
template <class T, class... V> void print(T t, V... v);
template <class T> void print(const multiset<T>& pq);
template <class T> void print(const vector<T>& v);
template <class T> void print(const set<T>& pq);
 
template <class T, class V> void print(const pair<T, V>& x) {
   cout << "{"; print(x.first); cout << ", "; print(x.second); cout << "}";
}
template <class T, class V> void print(const map<T, V>& mp) {
   for (auto it = mp.begin(); it != mp.end(); ++it) { print(*it); cout << " "; }
}
template <class T> void print(const multiset<T>& pq) {
   vector<T> values(pq.begin(), pq.end()); print(values);
}
template <class T> void print(const vector<T>& v) {
   for (int i = 0; i < (int) v.size(); ++i) { print(v[i]); cout << (i + 1 < (int) v.size() ? " " : ""); }
}
template <class T> void print(const set<T>& pq) {
   vector<T> values(pq.begin(), pq.end()); print(values);
}
template <class T, class... V> void print(T t, V... v) {
   print(t); if(sizeof...(v)) cout << " | "; print(v...);
}

ll dp[2001][2001];
ll minf = -1e15;
int n, m;

ll solve(int i, int j, vector<ll> &ar) {
	if(j > m) return 0;
	if(i > n) {
		return minf;
	}

	if(dp[i][j] != LLONG_MIN)
		return dp[i][j];

	ll x = j * ar[i] + solve(i+1, j+1, ar);
	ll y = solve(i+1, j, ar);

	dp[i][j] = max(x, y);

	return dp[i][j];
}

int main() {
	fastIO;
	

	cin >> n >> m;

	vector<ll> ar(n+5, 0);
	for(int i = 1; i <= n; i++)
		cin >> ar[i];

	for(int i = 0; i < 2001; i++) for(int j = 0; j < 2001; j++) dp[i][j] = LLONG_MIN;

	cout << solve(1, 1, ar) << "\n";
	return 0;
}