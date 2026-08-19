#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const ll N = 3e5 + 5;
const ll MOD = 1e9 + 7;
#define all(x) x.begin(),x.end()
#define pii pair<ll,ll>
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
typedef vector< vector<ll> > array_2D;
typedef pair<ll, pair<ll, ll> > ppi;
typedef vector<ll> vi;

bool comp(const pair<int, string> &a, const pair<int, string> &b) {
    return  (a.first > b.first) ||
            (a.first == b.first && a.second < b.second);
}


int ar[N];
int main() {
    fast;

    int n, k;
    test {
        cin >> n >> k;

        for(int i = 0; i < n; i++) cin >> ar[i];

        int cnt = 0;

        for(int i = 0; i < n; i++) {

            bool f = true;
            int p = k, q = i;

            for(p = k, q = i; p >= 1 && q < n; p--, q++) {

                if(p != ar[q]) {
                    f = false;
                    break;
                }
            }

            if(f && p == 0) {
                cnt++;
            }
        }


        cout << "Case #" << _i << ": " << cnt << "\n";
    }
    return 0;
}