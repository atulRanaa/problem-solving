#include<bits/stdc++.h>

using namespace std;

int main(){
	ios_base::sync_with_stdio(false);
    cin.tie(NULL); 
	

	int t; cin>>t;
	while(t--){

		int n; cin>>n;
		int ar[n];
		for(int i=0;i<n;i++) cin>>ar[i];
		sort(ar,ar+n);

		int m; cin>>m;
		int arr,maX = -1; unordered_map<int, bool> mapp;
		for(int i=0;i<n;i++)  { cin>>arr; 
			mapp[arr] = true;
			maX = max(arr,maX);
		}


		bool f = false;
		for(int i=0;i<n;i++){
			for(int j=i+1;j<n && (!f);j++){
				if(ar[i]+ar[j] > maX) break;

				if(mapp.find(ar[i]+ar[j]) != mapp.end()) f = true;
			}
			if(f || ar[i] > maX) break;
		}
		if(f) cout<<1<<"\n"; else cout<<0<<"\n";

	}
	return 0;
}