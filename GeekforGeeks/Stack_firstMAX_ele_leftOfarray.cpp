#include <stdio.h>
#include <iostream>
#include <cstring>
#include <cmath>
#include <algorithm>
#include <map>
#include <vector>
#include <set>
#include <string>
#include <stack>
#define gc getchar_unlocked
#define F first
#define S second
using namespace std;
typedef long long ll;
typedef vector<int> vi;
typedef pair<int,int> pii;

void scanI(int &x)
{
    register int c = gc();
    x = 0;
    int neg = 0;
    for(;((c<48 || c>57) && c != '-');c = gc());
    if(c=='-') {neg=1;c=gc();}
    for(;c>47 && c<58;c = gc()) {x = (x<<1) + (x<<3) + c - 48;}
    if(neg) x=-x;
}
void scanLL(long long &x)
{
    register int c = gc();
    x = 0;
    int neg = 0;
    for(;((c<48 || c>57) && c != '-');c = gc());
    if(c=='-') {neg=1;c=gc();}
    for(;c>47 && c<58;c = gc()) {x = (x<<1) + (x<<3) + c - 48;}
    if(neg) x=-x;
}
int main(){
	
		int n; scanI(n);
		ll ar[n],f[n],g[n];
		for(int i=0;i<n;i++) scanLL(ar[i]);

		stack<ll> stk;

		for(int i = 0;i < n ;i++){
			while(! stk.empty() && ar[i] > ar[ stk.top() ]){
				f[ stk.top()] = i; stk.pop();
			}
			stk.push(i);
		}
		while(! stk.empty()){ f[ stk.top()] = -1; stk.pop();}
		

		for(int i = 0;i < n ;i++){
			while(! stk.empty() && ar[i] < ar[ stk.top() ]){
				g[ stk.top()] = i; stk.pop();
			}
			stk.push(i);
		}
		while(! stk.empty()){ g[ stk.top()] = -1; stk.pop();}
        
        
        //for(int i=0;i<n;i++) printf("%lld ", (f[i]!=-1)? ar[f[i]]: f[i]); cout<<"\n";
        //for(int i=0;i<n;i++) printf("%lld ", (g[i]!=-1)? ar[g[i]]: g[i]); cout<<"\n";
		for(int i=0;i<n;i++){
			if(f[i] != -1 && g[f[i]] != -1) printf("%lld ",ar[ g[f[i]] ]);
			else printf("%d ",-1);
		}

	return 0;
}