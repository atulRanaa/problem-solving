#include<bits/stdc++.h>

using namespace std;
#define ll long long
#define mod 1000000007
#define max_sz 200000
#define S second
#define F first

int prime[200005];
vector<int> P;


int numofFact(int num){
    int res = 1;
    unordered_map<int,int> mapp;
    while(num != 1){
        mapp[ prime[num]]++;
        num /= prime[num];
    }
    for(auto ele: mapp) res *= (ele.S+1);

    return res;
}
ll modExpone(int x,int n){
    ll ans = 1, X =x,N= n;
    while(N > 0){
        if(N&1) ans = (ans*X)%mod;
        X = (X*X)%mod;
        N >>= 1;
    }
    return ans;
}

void sieve(){
	memset(prime,0,sizeof(prime));
    prime[0] = prime[1] = 1;
    
    for(int i = 2; i <= max_sz; i++){
        if(prime[i]==0){
            for(int j=i; j <= max_sz;j+=i) prime[j] = i;
        }
    }
}
ll prodofDivor[200005];
int main(){

    ios::sync_with_stdio(0), cin.tie(0);
	sieve();
	int n = 200000,q;  scanf("%d",&q);
    
    for(int i=1;i<=n;i++){
    	int div = numofFact(i);
    	prodofDivor[i] = modExpone(i,div/2);
    	if(div & 1) prodofDivor[i] = (prodofDivor[i] * (int)sqrt(i))%mod;
    	//printf("%d %lld Actual: %lld %lld\n",div, prodofDivor[i],count, temp);
    }
    
    while(q--){
    	scanf("%d",&n); printf("%lld\n",prodofDivor[n]);
    }
    return 0;
}
