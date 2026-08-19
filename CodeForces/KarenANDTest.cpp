#include<bits/stdc++.h>
using namespace std;

#define mod 1000000007

int main(){
ios_base::sync_with_stdio(false);
cin.tie(NULL);
     
    vector<long long int> v;
    int t;
    cin>>t;
    long long int c;
    for(int i=0;i<t;i++){
	    cin>>c;
	    v.push_back(c);
    }
     
    int k=1;
    while(v.size()>1){
     
	    int l=v.size();     
	    for(int i=1;i<l;i++){
		if(k == 1) v.push_back((v[i]+v[i-1])%mod);
		else v.push_back((v[i-1]-v[i])%mod);
		
		k*=-1;
	    }     
	    v.erase(v.begin(),v.begin()+l);
     
    }
    cout<<v[0]%mod;
return 0;
}
