#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const ll N = 1e6 + 5;
const ll MOD = 1e9 + 7;
#define all(x) x.begin(),x.end()
#define pii pair<int,int>
#define pis pair<ll,string>
#define F first
#define S second
#define LCM(a,b) ((a*b)/__gcd(a,b))
#define inf 1e15
#define test ll cse;cin>>cse;for(ll _i=1;_i<=cse;_i++)
#define PI 3.14159265
#define fast ios_base::sync_with_stdio(false);cin.tie(NULL);cout.tie(NULL);
const double EPS = 1E-9;
typedef vector< vector<double> > matrix;
typedef vector< vector<int> > array_2D;
typedef pair<int, pair<int, int> > ppi;
typedef vector<int> vi;


inline ll hilbertorder(int x, int y, int pow, int rotate) {
	if (pow == 0) {
		return 0;
	}
	int hpow = 1 << (pow-1);
	int seg = (x < hpow) ? ( (y < hpow) ? 0 : 3 ) : ( (y < hpow) ? 1 : 2 );
	seg = (seg + rotate) & 3;
	const int rotateDelta[4] = {3, 0, 0, 1};
	int nx = x & (x ^ hpow), ny = y & (y ^ hpow);
	int nrot = (rotate + rotateDelta[seg]) & 3;
	ll subSquareSize = ll(1) << (2*pow - 2);
	ll ans = seg * subSquareSize;
	ll add = hilbertorder(nx, ny, pow-1, nrot);
	ans += (seg == 1 || seg == 2) ? add : (subSquareSize - add - 1);
	return ans;
}


struct Query {
	int l, r, idx;
	ll ord;

	inline void calcOrder() {
		ord = hilbertorder(l, r, 21, 0);
	}
};
inline bool operator<(const Query &a, const Query &b) {
	return a.ord < b.ord;
}


ll ar[N];
Query Q[N];
int main() {
	fast;


	ll n, m, k; cin >> n >> m >> k;
	ar[0] = 0;
	for(int i = 1; i <= n; i++) cin >> ar[i];
	for(int i = 1; i <= n; i++) ar[i] ^= ar[i-1];

	for(int i = 0; i < m; i++) {
		cin >> Q[i].l >> Q[i].r;
		Q[i].idx = i;
		Q[i].calcOrder();
	}

	sort(Q, Q + m);


	ll ans = 0LL;
	vector< ll > answer(N);
	vector< ll > cnt(2*N, 0);


	// iterate over queries
	int l = 0, r = 1;
	ans = (ar[1] == k);

	cnt[ar[0]]++;
	cnt[ar[1]]++;


	for(int i = 0; i < m; i++) {

		Q[i].l--;
		// XOR of l-r subArray = pref[r] ^ pref[l-1];
		while (l > Q[i].l) {
			l--;
			ans += cnt[ar[l] ^ k];
			cnt[ar[l]]++;
		}
		while (r < Q[i].r) {
			r++;
			ans += cnt[ar[r] ^ k];
			cnt[ar[r]]++;
		}
		while (l < Q[i].l) {
			cnt[ar[l]]--;
			ans -= cnt[ar[l] ^ k];
			l++;
		}
		while (r > Q[i].r) {
			cnt[ar[r]]--;
			ans -= cnt[ar[r] ^ k];
			r--;
		}
		answer[ Q[i].idx ] = ans;
	}

	for(int i = 0; i < m; i++) cout << answer[i] << "\n";
	return 0;
}