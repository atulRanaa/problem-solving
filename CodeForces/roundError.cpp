#include <bits/stdc++.h>
using namespace std;

#define inf 1e15
int main(){
	int t; scanf("%d",&t);

	for(int cas = 1; cas <= t; cas++){
		
		int n,l;

		scanf("%d %d",&n,&l);
		vector<int> ar;
		int temp;

		for(int i=0;i<l;i++) {
			scanf("%d",&temp);
			ar.push_back(temp);
		}
		int rem=n;
		//cout<<n<<" "<<l<<"\n";
		for(int i=0;i<l;i++) rem-=ar[i];
		//cout<<endl;
		vector<int> up;
		for(int i=1;i<=n;i++){
			if(round( (i*1.0)/n *100) > ((1.0*i)/n) *100) up.push_back(i);
		}
		//cout<<up.size()<<" ";
		if(up.size() > 0){
			vector< pair<int,int> > need;

			for(int i=0;i<l;i++){
				int ind = lower_bound(up.begin(),up.end(),ar[i])-up.begin();
				//cout<<ind<<" ";
				if(ind != l){
					if(up[ind]!=ar[i]) need.push_back( make_pair(up[ind]-ar[i], i));
				}
			}
			//cout<<"\n";
			sort(need.begin(),need.end());
			//cout<<"sdsdgf\n";
			if(need.size() > 0){

				for(int k=0;k < need.size();k++){
					if(up[0] < need[k].first) break;

					if(rem-need[k].first >= 0) {ar[ need[k].second] += need[k].first;
						rem -= need[k].first;
					}
				}
				while(rem-up[0] >= 0) {ar.push_back(up[0]); rem -= up[0];}
			}
		}
		if(rem > 0) ar.push_back(rem);

		int ans = 0;
		//for(int i=0;i< ar.size();i++) cout<<ar[i]<<" "; cout<<"\n";
		for(int i=0;i< ar.size();i++) ans += round( (ar[i]*1.0)/n *100);
		
		//cout<<ar.size();
		printf("Case #%d: %d\n",cas,ans);
	}
	// cout<<(float)round(2.49)<<endl;
	// cout<<(float)round(2.6)<<endl;
	// int ar[] = {1,3,4,6};
	// cout<<*lower_bound(ar,ar+4,5);
	// int n = 6;
	// for(int i=1;i<=n;i++) cout<<((i*1.0)/n)<<" ";
	// cout<<endl;
	// for(int i=1;i<=n;i++){
	// 	if(round( (i*1.0)/n *100) > ((1.0*i)/n) *100) cout<<i<<" ";
	// }
	return 0;
}