#include<bits/stdc++.h>
using namespace std;

#define lli long long int
int main(){
	ios_base::sync_with_stdio(false);cin.tie(NULL); 

/*
sys.stdin=open('input.txt','r')
sys.stdout=open('output.txt','w')
*/
   	freopen("input.txt","r",stdin);
	freopen("output.txt","w",stdout);
	int t,n,m; cin>>t;
	while(t--){
		cin>>n>>m;

		if(n<=m){
			if(n&1) cout<<"L"<<endl;
			else cout<<"R"<<endl;
		}
		else{
			if(m&1) cout<<"D"<<endl;
			else cout<<"U"<<endl;
		}
	}

	return 0;
}