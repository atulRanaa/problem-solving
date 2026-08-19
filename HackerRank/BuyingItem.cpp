#include <bits/stdc++.h>

using namespace std;

#define ll long long int
map< pair<int,int>, ll> mapp;
ll n,m,k;
ll buyingItem(int ar[],int i,int pur){
    if(pur == m) return 0;
    if(i == n && pur != m) return n*k;
    
    if(mapp.find({i,pur}) != mapp.end()) return mapp[{i,pur}];
    ll ans = 0,ch=0;

    if(pur == 0) ch = 1;
    if(ar[i] == 1){
        if(pur+1 != m) ans += pur*k + min( k + buyingItem(ar,i+1,pur+1),ch + buyingItem(ar,i+1,pur));
    }
    else ans += pur*k + ch + buyingItem(ar,i+1,pur);

    mapp[{i,pur}] = ans;
    cout<<i<<" "<<ans<<"\n";
    return ans;
}
int main(){
    cin>>n>>m>>k;
    int arr[n+10], count = 0; 
    
    for(int i=1;i<=n;i++){ cin>>arr[i];
        if(arr[i] == 1) count++;
    }
    if(count < m) cout<<-1<<"\n";
    else cout<< buyingItem(arr,1, 0)<<"\n";

    //for(auto ele: mapp) cout<<ele.first<<" "<<ele.second<<"\n";
    return 0;
}