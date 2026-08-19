#include<bits/stdc++.h>
using namespace std;

typedef long long ll;
const ll MAX = 1e5+5;
const ll MOD = 1e9+7;
int main(){
	ll multi[MAX];
	ll array[MAX];
	multi[0]=1;
	for(int i=1;i <MAX; i++) multi[i] = (multi[i-1]*(1 + i))%MOD;
	
	int t,n;	cin>>t;
	ll ans = 0LL;
	while(t--){
		cin>>n;
		for(int i=0;i<n;i++) cin>>array[i];
		sort(array,array+n,greater<ll>());
		for(int i=0;i<n;i++){
			ans += (array[i]*multi[i])%MOD;
			ans %= MOD; 
		}
		cout<<ans<<"\n";
	}
	return 0;
}