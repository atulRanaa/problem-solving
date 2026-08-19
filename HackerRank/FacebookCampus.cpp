#include "bits/stdc++.h"
using namespace std;


typedef long long int ll;

int main(){
	int n; cin>>n;
	ll N,k, v;
	for(int sample=1; sample <= n; sample++){
		
		cin>>N>>k>>v;
		string ar[N+1];

		for(int i=0;i<N;i++) cin>>ar[i];

		ll temp = ((v-1)*k)%N;

		int check[N+1] = {0},count = k;
		while(count--){
			if(temp > N-1) temp=0;

			check[temp] = 1;
			temp++;
		}
		cout<<"Case #"<<sample<<": ";
		for(int i=0; i < N;i++) if(check[i]==1) cout<<ar[i]<<" ";
		cout<<"\n";

	}
}
