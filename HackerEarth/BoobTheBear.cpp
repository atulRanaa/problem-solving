#include "bits/stdc++.h"
using namespace std;


typedef long long int ll;
const int MAX = 1005;
ll len[MAX], tim[MAX];
//int mapp[MAX][1005];

int main(){
	int n; scanf("%d",&n);
	for(int i=0;i<n;i++) scanf("%lld",&len[i]);//cin>>len[i];
	for(int i=0;i<n;i++) scanf("%lld",&tim[i]);//cin>>tim[i];

	/*set< ll > s;
	for(int i=0;i<n;i++){
		s.insert(tim[i]), s.insert(tim[i]+len[i]);
	}*/
	int k=0;
	bitset<1001> mapp[MAX];
	//int ma = 0,ind;
	for(int i=0;i<n;i++){

		//int temp=0,temp1=0;
		//string S="",S1="";
		for(int j=0;j<n;j++){
			if(tim[i] >= tim[j] && tim[i] <= (tim[j]+len[j])) mapp[k][j]=1;//,temp++;

			
			if((tim[i]+len[i]) >= tim[j] && (tim[i]+len[i]) <= (tim[j]+len[j])) mapp[k+1][j]=1;//,temp1++;
			
			
		}
		/*if(temp > ma) ma=temp,ind=k;
		if(temp1 > ma) ma=temp1,ind=k+1;*/
		k+=2;
	}
	int N = 2*n,ans = 0;
	//cout<<ind<<"\n";
	//for(int i=0;i<N;i++) cout<<mapp[i]<<"\n";
	
	for(int i=0;i<N;i++){
		for(int j=i+1;j<N;j++){
			bitset<1001> c = mapp[i] | mapp[j];
			ans = max(ans,(int)c.count());
		}
	}
	printf("%d\n",ans);//cout<<ans<<"\n";
	//for(int i=0;i<2*n;i++) cout<<mapp[i]<<"\n";
	/*int start[n],fin[n];
	for(int i=0;i<n;i++){
		start[i] = distance(s.begin(),s.find(tim[i]));
		fin[i] = distance(s.begin(),s.find(tim[i],len[i]));
	}

	int dp[5000]={0};
	for(int i=0;i<n;i++){
		dp[ start[i] ] += 1, dp[fin[i] +1] -= 1;
	}
	for(int i=1;i<5000;i++) dp[i]+= dp[i-1];*/
}