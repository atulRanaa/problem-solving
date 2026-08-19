#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const ll N = 1e5 + 5;
const ll MOD = 1000000007;
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
typedef vector<int> vi;

map<int, int> printNGE(int ar[], int n) {
  int f[n + 5], g[n + 5];

  stack < int > stk;

  for (int i = 1; i <= n ; i++) {
    while (! stk.empty() && ar[i] > ar[ stk.top() ]) {
      f[ stk.top()] = i; stk.pop();
    }
    stk.push(i);
  }
  while (! stk.empty()) { f[ stk.top()] = n + 1; stk.pop();}


  for (int i = n; i >= 1 ; i--) {
    while (! stk.empty() && ar[i] >= ar[ stk.top() ]) {
      g[ stk.top()] = i; stk.pop();
    }
    stk.push(i);
  }
  while (! stk.empty()) { g[ stk.top()] = 0; stk.pop();}
  // for(int i=1;i<=n;i++) cout << g[i] << " "; cout<<"\n"; // ((g[i]!=-1)? ar[g[i]]: g[i])
  // for(int i=1;i<=n;i++) cout << f[i] << " "; cout<<"\n"; // ((f[i]!=-1)? ar[f[i]]: f[i])

  int tot = 0;
  map<int, int > countOcc;
  for (int i = 1; i <= n; i++) {
    int no = (f[i] - i) * (i - g[i]);
    tot += no;

    countOcc[ ar[i] ] += no;
  }
  cout << tot << "\n";

  return countOcc;
}

map<int, int> bruteForce(int ar[], int n) {
  map<int, int> countOcc;
  int tot = 0;
  for (int i = 1; i <= n; i++) for (int j = i + 1; j <= n + 1; j++) {
      int e = *max_element(ar + i, ar + j);
      countOcc[e] += 1;
      tot += 1;
    }
  cout << tot << "\n";

  return countOcc;
}

int main() {
  int arr[] = {0, 39, 99, 70, 24, 99, 49, 13, 86, 43, 88, 74, 45, 92, 72, 71, 90, 32, 19, 76, 84, 46, 63, 15, 87,
   1, 39, 58, 17, 65, 99, 43, 83, 29, 64, 67, 100, 14, 17, 100, 81, 26, 45, 40, 95, 94, 86, 2, 89, 57, 52, 91, 45};
  int n = sizeof(arr) / sizeof(arr[0]) - 1;

  cout << n << "\n";
  map<int, int> a = printNGE(arr, n);
  map<int, int> b = bruteForce(arr, n);
  for (auto e : a) { cout << e.F << " Calculated: " << e.S << " Actual: " << b[ e.F ] << "\n";}
  cout << ((n * (n + 1)) / 2) << "\n";
  return 0;
}

