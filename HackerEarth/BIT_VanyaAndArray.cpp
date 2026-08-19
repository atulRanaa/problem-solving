    #include <cstring>
    #include <stdio.h>
    #include <string>
    #include <iostream>
	#include <algorithm>
    using namespace std;

	const int N = 1e6+5;
	#define ll long long
	#define MOD 1000000007
	#define pii pair<int,int>
	#define pll pair<ll , ll>
	#define pis pair<int,string>
	#define F first
	#define S second
	#define LCM(a,b) ((a*b)/__gcd(a,b))
	#define inf 1e15
	
	ll BIT[N];
    ll ar[N];
    ll F[N];
    char s[N];
ll 	_scanf() {
	bool minus = false;
	ll result = 0;
	char ch;
	ch = getchar();
	while (true) {
		if (ch == '-') break;
		if (ch >= '0' && ch <= '9') break;
		ch = getchar();
	}
	if (ch == '-') minus = true; else result = ch-'0';
	while (true) {
		ch = getchar();
		if (ch < '0' || ch > '9') break;
		result = result*10 + (ch - '0');
	}
	if (minus)
		return -result;
	else
		return result;
}
ll get(int ind){ 
    ll sum = 0;
    for(;ind > 0; ind -= ind & (-ind))  sum += BIT[ind];
    return sum;
}
 
void updateBIT(int n, int ind,int val){ 
    for(;ind <= n;ind += ind & (-ind))  BIT[ind] += val;
    //for(int i=1;i<=n;i++) cout<<BIT[i]<<" "; cout<<"\n";
}
void build(int n){   
    for (int i=n; i>=1; i--){
    	F[i] = get(N-5) - get(ar[i]);
        updateBIT(N-5,ar[i],1);
    }
}

void convert(ll n)
{
    vector< pii > v(n);
	for(int i=1;i<=n;i++) v.push_back( {ar[i],i});
 
    sort(v.begin(),v.end());
 
    ll w = 0;
    ll ww = -1;
 
    for(int i=0;i<v.size();i++) {
        if(v[i].F > ww)
            w++,ww = v[i].F;
        ar[v[i].S] = w;
    }
}

	int main(){
	    
	    ll n,k; //scanf("%lld %lld",&n,&k);
	    n = _scanf();
	    k = _scanf();
	    for(int i=1;i<=n;i++) *(ar+i) = _scanf(); //scanf("%lld",ar+i);

	    //for(int i=1;i<=n;i++) cout<<ar[i]<<" "; cout<<"\n";
	    convert(n);
		//for(int i=1;i<=n;i++) cout<<ar[i]<<" "; cout<<"\n";
		build(n);
		//for(int i=1;i<=n;i++) cout<<F[i]<<" "; cout<<"\n";

		sort(F+1,F+n+1);
		ll count = 0LL;
		int i,j=1;
		for (i = n; i > j; i--) {
	        do {
	            if (F[i] + F[j] >= k) {
	                count += (i - j);
	                break;
	            }
	            j++;
	        } while (i > j);
	    }
	    printf("%lld", count);
	    return 0;
	}