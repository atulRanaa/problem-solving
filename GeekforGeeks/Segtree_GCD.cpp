#include <bits/stdc++.h>

#define ll long
#define mod 1000000007
#define rep(i, begin, end) for (__typeof(end) i = (begin) - ((begin) > (end)); i != (end) - ((begin) > (end)); i += 1 - 2 * ((begin) > (end)))
#define MAX 1000010
#define scan(a) scanf("%ld", &a)

using namespace std;

ll n, q, x, y, a[MAX], phi[MAX];
ll f[MAX], bit[MAX];
char c;

void sieve(){
	for (ll i = 1; i < MAX; ++i) phi[i] = i;

	for (ll i = 2; i < MAX; ++i){
		if(phi[i] == i)
		for (ll j = i; j < MAX; j+=i)
		{
			phi[j]/= i;
			phi[j]*= (i-1);
		}
	}
	for (ll i = 1; i < MAX; ++i)
		for (ll j = i; j < MAX; j+=i){
			f[j] += i*phi[j/i];
			if(f[j] >= mod) f[j]-= mod;
		}
}

void update(ll x, ll val){
	while(x<=n){
		bit[x]+= val;
		if(bit[x]>=mod) bit[x]-= mod;
		x += (x&(-x));
	}
}

ll query(ll x){
	ll ans = 0ll;
	while(x){
		ans+= bit[x];
		if(ans>=mod)ans-= mod;
		x -= (x&(-x));
	}
	if(ans >= mod) ans-=mod;
	return ans;
}

int main(){

	sieve();
	scan(n);
	rep(i,1,n+1){
		scan(a[i]);
		update(i, f[a[i]]);
	}
	cin>>q;
	ll ans = 0ll;
	while(q--){
		scanf(" %c", &c);scan(x);scan(y);
		if(c == 'C'){
			ans = (query(y) - query(x-1));
			while(ans<0)
			ans+= mod;
			printf("%ld\n", (ans%mod));
		}
		else{
			update(x, f[y]-f[a[x]]);
			a[x] = y;
		}
	}
	return 0;
}
