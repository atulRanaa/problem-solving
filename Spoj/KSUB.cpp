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
 
template <class T, class V> void println(const pair<T, V>& x) {
   cout << "{"; print(x.first); cout << ", "; print(x.second); cout << "}"; cout << "\n";
}
template <class T, class V> void println(const map<T, V>& mp) {
   for (auto it = mp.begin(); it != mp.end(); ++it) { print(*it); cout << " "; } cout << "\n";
}
template <class T> void println(const multiset<T>& pq) {
   vector<T> values(pq.begin(), pq.end()); print(values); cout << "\n";
}
template <class T> void println(const vector<T>& v) {
   for (int i = 0; i < (int) v.size(); ++i) { print(v[i]); cout << (i + 1 < (int) v.size() ? " " : ""); } cout << "\n";
}
template <class T> void println(const vector< vector<T> >& v) {
   for (int i = 0; i < (int) v.size(); ++i) { print(v[i]); cout << "\n"; } cout << "\n";
}
template <class T> void println(const set<T>& pq) {
   vector<T> values(pq.begin(), pq.end()); print(values);
}
template <class T, class... V> void println(T t, V... v) {
   print(t); if(sizeof...(v)) cout << " | "; print(v...); cout << "\n";
}

int maxn = 2e5 + 5;

struct union_find {
    std::vector <int> parent, rank;

    // Constructor to initialse 'parent' and 'rank' vector.
    union_find(int n) {
        parent = std::vector <int> (n);
        rank = std::vector <int> (n, 0);        // initialse rank vector with 0.
        for(int i = 0; i < n; i++)
            parent[i] = i, rank[i] = 1;
    }

    // Find with Path Compression Heuristic.
    int find(int x){
       while(parent[x] != x){
           parent[x] = parent[parent[x]];
           x = parent[x];
       }
       return x;
   }

    // Union by checking rank to keep the depth of the tree as shallow as possible.
    void combine(int x, int y) {
        int p = find(x), q = find(y);
         if(p == q) return;
         if(rank[q] > rank[p]) swap(p,q);
       
        parent[q]=p; 
        rank[p]+=rank[q];
        rank[q]=-1;
    }
};

vector<int> primes;
void seive() {
   bool isprime[maxn];
   memset(isprime, true, sizeof isprime);

   primes.push_back(2);
   for(int i = 3; i <= maxn; i += 2) {
     if(isprime[i]) {
         primes.push_back(i);
         for(int j = 2*i; j < maxn; j += i)
             isprime[j] = false;
     }
   }

}

vector<int> factors(int n) {
   vector <int> divisors;
   for(int j = 0; j < primes.size(); j++) {
      if(primes[j] > n)
          break;
      if(n % primes[j] == 0) {
          divisors.push_back(primes[j]);
          while(n % primes[j] == 0) {
              n /= primes[j];
          }
      }
   }
   if(n > 2)
      divisors.push_back(n);

   return divisors;
}


int parition_subarray(vector<int> &ar) {
   union_find ds(maxn);

   for(int &e: ar) {
      vector<int> divisors = factors(e);

      // cout << "Divisors of " << e << ":"; println(divisors);

      for(int i = 1; i < divisors.size(); i++)
         ds.combine(divisors[i], divisors[i-1]);
      ds.combine(divisors[0], e);
   }


   set <int> unique;
   for(auto &e: ar) {
        unique.insert(ds.find(e));
   }

   // cout << "Subarrays:";
   // for(auto e: unique) cout << e << " "; cout << "\n";
   
   vector<int> sz;
   for(auto &node: unique)
      sz.push_back(ds.rank[node]);

   sort(sz.begin(), sz.end());

   // cout << "Subarrays Size:"; println(sz);

   int countPairs = 0;
   for(int i = sz.size()-2; i >= 0; i--) {
      int tmp = min(sz[i], sz[i+1]);

      sz[i+1] -= tmp;
      sz[i] -= tmp;
      swap(sz[i], sz[i+1]);

      countPairs += tmp;
   }

   return countPairs;
}

int main() {
	fastIO;

   seive();
   // println(primes);
	
   test {
      int n, k;
      cin >> n >> k;

      vector<int> ar(n);
      for(int i = 0; i< n; i++)
         cin >> ar[i];


      int g = ar[0];
      for(int &e: ar) g = std::gcd(g, e);

      vector<int> partition;

      int count = 0;
      for(int i = 0; i < n; i++) {
         ar[i] /= g;

         if(ar[i] > 1)
            partition.push_back(ar[i]);
         else 
            count++;
      }


      int pairs = parition_subarray(partition);

      if(count + pairs >= k)
         cout << "Yes\n";
      else
         cout << "No\n";
   }

}