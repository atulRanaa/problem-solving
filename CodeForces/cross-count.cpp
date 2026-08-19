#include<bits/stdc++.h>
#include<ext/pb_ds/assoc_container.hpp>
#include<ext/pb_ds/tree_policy.hpp>

using namespace std;
using namespace __gnu_pbds;


#define all(x) x.begin(),x.end()
#define fastIO ios::sync_with_stdio(false);cin.tie(0);cout.tie(0)
#define test int testcases;cin>>testcases;for(int tc=1;tc<=testcases;tc++)
#define lcm(a,b) ((a*b)/__gcd(a,b))
typedef long long ll;
typedef tree<
        pair<int, int>,
        null_type,
        less<pair<int, int>>,
        rb_tree_tag,
        tree_order_statistics_node_update>
        ordered_set;

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
template <class T> void print(const vector< vector<T> >& v) {
   for (int i = 0; i < (int) v.size(); ++i) { print(v[i]); cout << "\n"; }
}
template <class T> void print(const set<T>& pq) {
   vector<T> values(pq.begin(), pq.end()); print(values);
}
template <class T, class... V> void print(T t, V... v) {
   print(t); if(sizeof...(v)) cout << " | "; print(v...);
}



int main() {
	fastIO;
	test {
      int n, m;
      cin >> n >> m;


      vector<string> grid(n);
      for(int i = 0; i < n; i++)
         cin >> grid[i];

      vector< vector<int> > mn(n, vector<int>(m, 0));
      vector< vector<int> > mx(n, vector<int>(m, 0));

      vector<int> tmp(m, 0);

      for(int i = 0; i < n; i++) {

         vector<int> tmp2(m, 0);
         if(i == 0) {
            for(int j = 0; j < m; j++) tmp[j] = (grid[i][j]=='1');
            continue;
         }

         tmp2[0] = (grid[i][0]=='1');
         for(int j = 1; j < m; j++) {
            tmp2[j] = (grid[i][j]=='1')?(tmp[j-1] + 1):0;
            mn[i][j] = tmp2[j];
            mx[i][j] = tmp2[j];
         }

         tmp = tmp2;
      }

      for(int i = n-1; i >= 0; i--) {

         vector<int> tmp2(m, 0);

         if(i == n-1) {
            for(int j = 0; j < m; j++) tmp[j] = (grid[i][j]=='1');
            continue;
         }

         tmp2[m-1] = (grid[i][m-1]=='1');
         for(int j = 0; j < m-1; j++) {
            tmp2[j] = (grid[i][j]=='1')?(tmp[j+1] + 1):0;
            mn[i][j] = min(mn[i][j], tmp2[j]);
            mx[i][j] = min(mx[i][j], tmp2[j]);

         }

         tmp = tmp2;
      }

      for(int i = 0; i < n; i++) {

         vector<int> tmp2(m, 0);
         if(i == 0) {
            for(int j = 0; j < m; j++) tmp[j] = (grid[i][j]=='1');
            continue;
         }

         tmp2[m-1] = (grid[i][m-1]=='1');
         for(int j = 0; j < m-1; j++) {
            tmp2[j] = (grid[i][j]=='1')?(tmp[j+1] + 1):0;
            mn[i][j] = min(mn[i][j], tmp2[j]);
            mx[i][j] = min(mx[i][j], tmp2[j]);
         }

         tmp = tmp2;
      }

      for(int i = n-1; i >= 0; i--) {

         vector<int> tmp2(m, 0);

         if(i == n-1) {
            for(int j = 0; j < m; j++) tmp[j] = (grid[i][j]=='1');
            continue;
         }

         tmp2[0] = (grid[i][0]=='1');
         for(int j = 1; j < m; j++) {
            tmp2[j] = (grid[i][j]=='1')?(tmp[j-1] + 1):0;
            mn[i][j] = min(mn[i][j], tmp2[j]);
            mx[i][j] = min(mx[i][j], tmp2[j]);
         }

         tmp = tmp2;
      }
        

      print(mn);
      print(mx);


      long long ans = 0;
      for(int i = 0; i < n; i++) for(int j = 0; j < m; j++) {
         if(mn[i][j] >= 2)
            ans += mx[i][j];

         ans %= (1000000009);
      }
	}
}