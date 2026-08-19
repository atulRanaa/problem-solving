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

vector< int > mergeKsortedArray(array_2D &arr, int k) {
    vector< int > ANS;
    priority_queue< ppi, vector<ppi>, greater<ppi> > Q;

    for(int i = 0; i < k; i++) Q.push( {arr[i][0], {0, i}} );

    while(!Q.empty()) {
        ppi min_value = Q.top();
        Q.pop();

        int val = min_value.first;
        int itr = min_value.second.first;
        int i = min_value.second.second;

        ANS.push_back(val);
        if(itr + 1 < arr[i].size()) {
            Q.push( { arr[i][itr + 1], { itr + 1, i}} );
        }
    }
    return ANS;
}


int main() {
    fast;

    array_2D data({
        {1, 4, 10},
        {2, 9, 10},
        {5, 7, 18, 100},
        {4, 7}
    });
    vector< int > answer = mergeKsortedArray(data, 4);
    for(int i = 0; i < answer.size(); i++) cout << answer[i] << " "; cout << "\n";
    return 0;
}