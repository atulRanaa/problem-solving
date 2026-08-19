
#include <bits/stdc++.h>

using namespace std;

typedef long long ll;
const int N = 1e5 + 5;
#define all(x) x.begin(),x.end()
#define MOD 1000000007
#define pii pair<int,int>
#define pis pair<int,string>
#define F first
#define S second
#define LCM(a,b) ((a*b)/__gcd(a,b))
#define inf 1e15
#define test ll cse;cin>>cse;for(ll i=1;i<=cse;i++)
#define fast ios_base::sync_with_stdio(false);cin.tie(NULL);
#define PI 3.14159265
#define MAXN 1000001


ll phi[MAXN + 1], prime[MAXN / 10];

bitset < MAXN + 1 > mark;
void sieve() {
    int size = 0;
    for (int i = 2; i <= MAXN; i++ ) {
        if (!mark[i]) {
            phi[i] = i - 1;
            prime[size++] = i;
        }
        for (int j = 0; j < size && prime[j]*i <= MAXN; j++ ) {
            mark[prime[j]*i] = 1;
            if (i % prime[j] == 0) {
                phi[i * prime[j]] = phi[i] * prime[j];
                break;
            }
            else phi[i * prime[j]] = phi[i] * (prime[j] - 1 );
        }
    }
}
int main() {

    fast;
    sieve();

    int n, k; cin >> n >> k;
    pair<int, int> ar[n];

    for (int i = 0; i < n; i++) cin >> ar[i].F, ar[i].S = i;
    sort(ar, ar + n);

    int ans[n];
    memset(ans, 0 , sizeof(ans));

    int team = 1;
    for (int i = n - 1; i >= 0; i--) {
        int ind = ar[i].S;
        int l = min(0, i - ind);
        int r = max(n - 1, i + ind);

        if (ans[i] == 0) {
            for (int j = l; j <= r; j++) if ()
                }
    }
    return 0;
}