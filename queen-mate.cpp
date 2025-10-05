#include<bits/stdc++.h>
// #include<ext/pb_ds/assoc_container.hpp>
// #include<ext/pb_ds/tree_policy.hpp>

using namespace std;
// using namespace __gnu_pbds;


#define all(x) x.begin(),x.end()
#define fastIO ios::sync_with_stdio(false);cin.tie(0);cout.tie(0)
#define test int testcases;cin>>testcases;for(int tc=1;tc<=testcases;tc++)

typedef long long ll;
// typedef tree<
//         pair<int, int>,
//         null_type,
//         less<pair<int, int>>,
//         rb_tree_tag,
//         tree_order_statistics_node_update>
//         ordered_set;

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
template <class T> void print(const set<T>& pq) {
   vector<T> values(pq.begin(), pq.end()); print(values);
}
template <class T, class... V> void print(T t, V... v) {
   print(t); if(sizeof...(v)) cout << " | "; print(v...);
}

int main() {
	fastIO;
	for(int x = 1; x <= 8; x++) for(int y = 1; y <= 8; y++) {
        // int x, y;
        // cin >> x >> y;
        
        int ar[9][9];
        memset(ar, 0, sizeof ar);
        
        ar[x][y] = 1;
        if(x == 1 && y == 1) {
            // ar[3][2] = 2;
            ar[2][3] = 2;
        } else if (x == 1 && y == 8) {
            ar[3][7] = 2;
            // ar[2][6] = 2;
        } else if (x == 8 && y == 1) {
            ar[6][2] = 2;
            // ar[7][3] = 2;
        } else if (x == 8 && y == 8) {
            // ar[6][7] = 2;
            ar[7][6] = 2;
        } else {
            
            if(y == 1) {
                ar[x-1][y+2] = 2;
                ar[x+1][y+2] = 2;
            } else if(y == 8) {
                ar[x-1][y-2] = 2;
                ar[x+1][y-2] = 2;
            } else if(x == 1) {
                ar[x+2][y-1] = 2;
                ar[x+2][y+1] = 2;
            } else if(x == 8) {
                ar[x-2][y-1] = 2;
                ar[x-2][y+1] = 2;
            } else {
                
                if(x-2 >= 1 && x+2 <= 8 && y-1>=1 && y+1 <=8) {
                    ar[x-2][y-1] = 2;
                    ar[x+2][y+1] = 2;
                } else if(y-2 >= 1 && y+2 <= 8 && x-1>=1 && x+1 <=8) {
                    ar[x-1][y-2] = 2;
                    ar[x+1][y+2] = 2;
                } else {

                    if(x == 2 && y == 2) {
                        ar[x-1][y+2] = 2;
                        ar[x+3][y-1] = 2;
                    } else if(x == 2 && y == 7) {
                        ar[x-1][y-2] = 2;
                        ar[x+3][y+1] = 2;
                    } else if(x == 7 && y == 2) {
                        ar[x+1][y+2] = 2;
                        ar[x-3][y-1] = 2;
                    } else if(x == 7 && y == 7) {
                        ar[x+1][y-2] = 2;
                        ar[x-3][y+1] = 2;
                    }
                }
                
            }
        }
        
        
        
        for(int i = 1; i <= 8; i++) {
            for(int j = 1; j <= 8; j++) cout << ar[i][j] << " ";
            cout << "\n";
        }

        cout << "\n";
        
	}
}