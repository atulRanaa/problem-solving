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


int main() {
	fastIO;
	

	int n, m;
	cin >> n >> m;

	vector<ll> ar(n+5, 0);
	for(int i = 1; i <= n; i++)
		cin >> ar[i];


	vector<ll> A(n+5, 0), B(n+5, 0);
	for(int i = 1; i <= n; i++) {
		A[i] = A[i-1] + i*ar[i];
		B[i] = B[i-1] + ar[i];
	}
	ll ans = LLONG_MIN;

	for(int i = 1; i <= n; i++) {
		int l = i;
		int r = i + m - 1;
		if(r > n) break;

		ll tmp = A[r] - A[l-1] - (i-1) * (B[r] - B[l-1]);
		if(tmp > ans)
			ans = tmp;
	}
	// print(A), cout << "\n";
	// print(B), cout << "\n";
	cout << ans << "\n";
	return 0;
}