#include<bits/stdc++.h>
using namespace std;

int main(){
	int n,m;
	int p,a,b,c,d;
	cin>>n;
	cin>>m;

	int cumXor[n+1][n+1];
	memset(cumXor,0,sizeof(cumXor));
	for(int i=0;i<m;i++){
		cin>>p>>a>>b>>c>>d;
		
		for(int j=b; j<=d; j++) cumXor[a][j] ^= p;
		if(c+1 <=n){
			for(int j=b; j<=d; j++) cumXor[c+1][j] ^= p;
		}
		/*for(int i=1;i <=n ;i++){
			for(int j=1;j<=n;j++) cout<<cumXor[i][j]<<" ";
			cout<<"\n";
		}
		cout<<"\n";*/
	}

	for(int i=1;i <= n;i++)
	{
	    for(int j=2;j <= n;j++)
	    {

	        cumXor[j][i] ^= cumXor[j-1][i];
	    }
	}
	for(int i=1;i <=n ;i++){
		for(int j=1;j<=n;j++) cout<<cumXor[i][j]<<" ";
		cout<<"\n";
	}
	return 0;
}