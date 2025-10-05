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

const int maxn = 2e5 + 5;

vector<int> adj[maxn];

int main() {
	fastIO;
	
	int n, m;

	cin >> n >> m;

	vector<ll> ar(n+5, 0);
	for(int i = 1; i <= n; i++)
		cin >> ar[i];

	int x, y;
	for(int i = 0; i < m; i++) {
		cin >> x >> y;
		adj[x].push_back(y);
		adj[y].push_back(x);
	}

	multiset< pair<ll, int> > cost;

	for(int i = 1; i <= n; i++) {

		ll sum = 0;
		for(int &e: adj[i]) {
			sum += ar[e];
		}

		cost.insert({sum, i});
	}

	ll ans = 0;
	vector<int> visited(n + 5, 0);

	while(cost.size() > 0) {
		auto [operationcost, node] = *cost.begin();
		visited[node] = 1;

		cost.erase(cost.begin());

		// cout << operationcost << " " << node << "\n";
		for(int &neibor: adj[node]) {
			ll sum = 0;
			for(int &e: adj[neibor]) {
				sum += ar[e];
			}

			if(!visited[neibor])
				cost.insert({sum - ar[node], neibor});
			auto it = cost.find({sum, neibor});
			if(it != cost.end())
				cost.erase(it);
		}

		ar[node] = 0;


		ans = max(ans, operationcost);
	}

	cout << ans << "\n";

	return 0;
}