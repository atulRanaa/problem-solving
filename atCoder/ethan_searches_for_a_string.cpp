#include <cstdio>
#include <cmath>
#include <iostream>
#include <set>
#include <algorithm>
#include <vector>
#include <map>
#include <cassert>
#include <string>
#include <cstring>
#include <queue>

using namespace std;

#define rep(i,a,b) for(int i = a; i < b; i++)
#define S(x) scanf("%d",&x)
#define S2(x,y) scanf("%d%d",&x,&y)
#define P(x) printf("%d\n",x)
#define all(v) v.begin(),v.end()
#define FF first
#define SS second
#define pb push_back
#define mp make_pair

typedef long long int LL;
typedef pair<int, int > pii;
typedef vector<int > vi;

int main() {
  int tc;
  S(tc);
  rep(t,1,tc+1) {
    string s;
    cin >> s;
    printf("Case #%d: ",t);
    int n = s.size();
    bool ok = true;
    rep(i,1,n) {
      if(s[0] == s[i] && s.substr(0, n - i) != s.substr(i, n - i)) {
        ok = false;
        cout << s.substr(0, i) + s << "\n";
        break;
      }
    }
    if(ok) printf("Impossible\n");
  }
  return 0;
}
