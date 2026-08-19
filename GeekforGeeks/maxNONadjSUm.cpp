#include <bits/stdc++.h>

using namespace std;


unordered_map<int , int> m;
int n;
int maxNonAdjSubsetSum(int ar[],int i){
    if(i >= n) return 0;
    if(m.find(i) != m.end()) return m[i];
    int ans = 0;
    if(ar[i] > 0){
        ans = max(ar[i] + maxNonAdjSubsetSum(ar, i+2),maxNonAdjSubsetSum(ar,i+1));
    }
    else ans = max(ans, maxNonAdjSubsetSum(ar,i+1));
    m[i] = ans;

    return ans;
}
int main(){
    n; cin>>n;
    int arr[n]; 
    
    for(int i=0;i<n;i++){ cin>>arr[i];}
    cout<< maxNonAdjSubsetSum(arr,0)<<"\n";

    for(auto ele: m) cout<<ele.first<<" "<<ele.second<<"\n";
    return 0;
}