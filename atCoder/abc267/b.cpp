#include<bits/stdc++.h>

using namespace std;


#define all(x) x.begin(),x.end()
#define fastIO ios::sync_with_stdio(false);cin.tie(0);cout.tie(0)
#define test int testcases;cin>>testcases;for(int tc=1;tc<=testcases;tc++)
typedef long long ll;

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
	

	string s;
	cin >> s;
	s = "#" + s;
	if(s[1] == '1') {
		cout << "No\n";
		return 0;
	}


	vector<char> col[7];
	col[0] = {s[7]};
	col[1] = {s[4]};
	col[2] = {s[2], s[8]};
	col[3] = {s[1], s[5]};
	col[4] = {s[3], s[9]};
	col[5] = {s[6]};
	col[6] = {s[10]};


	vector<int> standingpin;
	vector<int> knockeddown;
	for(int i = 0; i < 7; i++) {

		bool f = false;
		for(char &e: col[i]) {
			if(e == '1') 
				f = true;
		}

		if(f) 
			standingpin.push_back(i);
		else
			knockeddown.push_back(i);
	}

	if(standingpin.size() <= 1) {
		cout << "No\n";
		return 0;
	} else {
		int l = standingpin[0];
		int r = standingpin[standingpin.size() - 1];

		for(int &e: knockeddown) if(l < e && e < r) {
			cout << "Yes\n";
			return 0;
		}

		cout << "No\n";
	}


	return 0;
}