#include<bits/stdc++.h>


using namespace std;

#define ll long long int
#define MAXN 1001


int main() {
	
	int t; cin>>t; 

    while(t-- > 0){
        int n; cin>>n;
        unordered_map<int,int> MAP;
        int ar[n];
        for(int i=0;i<n;i++){ cin>>ar[i]; MAP[ar[i]]++;}
        int am; cin>>am; 
        
        int diff=1000005,ans=-1;
        for(int i=0;i<n;i++) if(MAP.find(am-ar[i]) != MAP.end()){
            if(am-ar[i] != ar[i]){
                if( abs(am-ar[i]-ar[i])<diff){ ans=(ar[i]); diff=abs(am-ar[i]-ar[i]); }
            }
            else{
                if(MAP[ar[i]] > 1){
                    ans = ar[i];
                    diff= 0;
                }
            }
        }

        if(ans == -1) cout<< -1 <<"\n";
        else cout<< "Deepak should buy roses whose prices are "<<min(ans,am-ans)<<" and "<<max(ans,am-ans)<<".\n";
    }
	return 0;
}