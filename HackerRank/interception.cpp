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
    int n;
    S(n);
    rep(i,0,n+1) {
      int x;
      S(x);
    }
    printf("Case #%d: ",t);
    if(n & 1) printf("1\n0\n");
    else printf("0\n");
  }
  return 0;
}
