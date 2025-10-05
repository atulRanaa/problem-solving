#include<bits/stdc++.h>

using namespace std;


#define all(x) x.begin(),x.end()
#define fastIO ios::sync_with_stdio(false);cin.tie(0);cout.tie(0)
#define test int testcases;cin>>testcases;for(int tc=1;tc<=testcases;tc++)
#define lcm(a,b) ((a*b)/__gcd(a,b))
typedef long long ll;

void print(string x)  { cout << '\"' << x << '\"'; }
void print(char x)  { cout << '\'' << x << '\''; }
void print(long long x)  { cout << x; }
void print(double x)  { cout << x; }
void print(bool x)  { cout << x; }
void print(int x)  { cout << x; }
void println(string x)  { cout << '\"' << x << '\"' << "\n"; }
void println(char x)  { cout << '\'' << x << '\'' << "\n"; }
void println(long long x)  { cout << x << "\n"; }
void println(double x)  { cout << x << "\n"; }
void println(bool x)  { cout << x << "\n"; }
void println(int x)  { cout << x << "\n"; }
 
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
	
   int n;
   cin >> n;

   vector<int> ar(n);
   for(int i = 0; i < n; i++) {
      cin >> ar[i];
   }

   set<int> s(ar.begin(), ar.end());

   if(s.count(0)) {
      cout << min(2, (int)s.size()) << "\n";
   } else {
      int g = abs(ar[0] - ar[1]);
      for(int i = 1; i < n; i++) {
         g = gcd(g, abs(ar[i] - ar[i-1]));
      }

      // cout << g << "\n";

      if(g > 1)
         cout << 1 << "\n";
      else
         cout << min(2, (int)s.size()) << "\n";
   }
   


}