#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const ll N = 5e5 + 5;
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
inline bool MO(const Query &x, const Query &y) {
	    return x.r < y.r;
}
inline bool MO_hilbertorder(const Query &a, const Query &b) {
	return a.ord < b.ord;
}


ll ar[N];
ll prefixSum[N];
ll happyCount[N];

Query Q[N];

ll BIT[N];
int answer[N];

ll query(int idx){
    ll sum = 0;
    for(;idx > 0; idx -= idx & (-idx))  sum += BIT[idx];
    return sum;
}

void updateBIT(int idx, ll val, int n){
    for(;idx <= n;idx += idx & (-idx))  BIT[idx] += val;
}

void solve(int n, int q){


    int last_visit[N];
    for(int i = 0; i < N; i++) last_visit[i] = -1, BIT[i] = 0LL;

    // answer for each query
    int query_counter = 0;
    for(int i = 1; i <= n; i++) {
        // If last visit is not -1 update -1 at the
        // idx equal to last_visit[arr[i]]
        if (last_visit[ar[i]] != -1)
            updateBIT(last_visit[ar[i]], - (ar[i] * happyCount[ ar[i] ]), n);

        // Setting last_visit[arr[i]] as i and updating
        // the bit array accordingly
        last_visit[ar[i]] = i;
        updateBIT(i, (ar[i] * happyCount[ ar[i] ]), n);

        while (query_counter < q && Q[query_counter].r == i) {

        	ll exp = query(Q[query_counter].r) - query(Q[query_counter].l - 1);
        	ll act = prefixSum[Q[query_counter].r] - prefixSum[Q[query_counter].l - 1];

            answer[ Q[query_counter].idx ] = ((exp == act) ? 1 : 0);
            query_counter++;
        }
    }

}

int main() {


	int n, m, q;
	scanf("%d %d", &n, &m);

	ar[0] = 0LL, happyCount[0] = INT_MAX;
	for(int i = 1; i <= n; i++) scanf("%lld", &ar[i]);
	for(int i = 1; i <= m; i++) scanf("%lld", &happyCount[i]);


	prefixSum[0] = 0LL;
	for(int i = 1; i <= n; i++) prefixSum[i] = (prefixSum[i-1] + ar[i]);

	// sum[l : r] = x * hx + y + hy + ...

	scanf("%d", &q);
	int l, r;
	for(int i = 0; i < q; i++) {
		scanf("%d %d", &l, &r);

		Q[i].l = l, Q[i].r = r;
		Q[i].idx = i;
		// Q[i].calcOrder();
	}

	sort(Q, Q + q, MO);
	// sort(Q, Q + q, MO_hilbertorder);
	solve(n, q);
	for(int i = 0; i < q; i++) printf("%d\n", answer[i]);

	return 0;
}