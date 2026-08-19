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

int distinctChar(string &s) {
    int ar[26] = {0};

    int cnt = 0;
    for(char &c: s) if(c != ' ') {
        if(!ar[c-'A']) cnt++;
        ar[c-'A']++;
    }

    return cnt;
}
int main() {
    fast;

    int n;
    test {
        cin >> n;

        vector< string > names(n);
        for(int i = 0; i < n; i++) {
            getline(cin >> std::ws, names[i]);
        }

        // for(int i = 0; i < n; i++) cout << names[i] << "\n";
        vector< pair<int, string> > ar;
        for(int i = 0; i < n; i++) ar.push_back( { distinctChar(names[i]) , names[i]} );


        sort(ar.begin(), ar.end(), comp);
        cout << "Case #" << _i << ": " << ar[0].second << "\n";
    }
    return 0;
}