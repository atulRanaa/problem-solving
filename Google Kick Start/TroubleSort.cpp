#include<stdio.h>
#include<algorithm>
#include<iostream>
#include<string.h>
#include<set>
#include<map>
#include<vector>
using namespace std;
#define X first
#define Y second
typedef long long ll;
const ll MAX = 1e5+5;
const ll MOD = 1e9+7;

void mix(vector<ll> mixed,ll even[MAX],ll odd[MAX],int n){
	
}
void troubleSort(ll even[MAX], ll odd[MAX], int n){
	vector<ll> num;
	int x=0,y=0;
	for(int i=0;i<n;i++){
		if(i&1) num.push_back(odd[y++]);
		else num.push_back(even[x++]);
	}

	bool done = false;
	while(!done){
		done = true;
		for(int i=0;i < n-2; i++){
			if(num[i] > num[i+2]){
				done = false;
				ll temp = num[i];
				num[i] = num[i+2];
				num[i+2] = temp;
			}
		}
	}
	for(int i=0;i<n;i++) cout<<num[i]<<" ";
	cout<<"\n";
}
int main(){

	int t; scanf("%d",&t);
	ll odd[MAX];
	ll even[MAX];
	for(int c=1; c <= t; c++){
		int n;	scanf("%d",&n);
		int x=0,y=0;
		for(int i=0;i<n;i++){
			if(i&1) scanf("%lld",&odd[y++]);
			else scanf("%lld",&even[x++]);
		}
		

		//troubleSort(even, odd, n);

		int l1 = (n&1)?(n/2 +1):(n/2);
		int l2 = n-l1;
		sort(even,even+l1);
		sort(odd,odd + l2);

		
		vector<ll> mixed;
		x=0,y=0;
		for(int i=0;i<n;i++){
			if(i&1) mixed.push_back(odd[y++]);
			else mixed.push_back(even[x++]);
		}
		//for(int i=0;i<n;i++) cout<<mixed[i]<<" ";
		//cout<<"\n";

		int ans = -1;
		bool f = true;
		for(int i=1; i < n;i++){
			if(mixed[i-1] > mixed[i]){
				f = false;
				ans = i-1;
				break;
			}
		}
		if(f) printf("Case #%d: OK\n",c);
		else printf("Case #%d: %d\n",c,ans);
	}
	return 0;
}