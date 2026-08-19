#include <bits/stdc++.h>
using namespace std;
#define F first
#define S second
#define pii pair<int,int>

int main(){
    int t; cin>>t;
    while(t-- > 0){

        int q, k; cin>>q>>k;
        queue<int> line[q]; int n,m, maxN=0; 
        
        for(int i=0;i<q;i++){
            cin>>n;  maxN += n;
            for(int j=0;j<n;j++){
                cin>>m; line[i].push(m);
            }
        }
        long long ans =0;
        k = min(maxN, k);

        priority_queue < pii,vector< pii >,greater< pii > > pq;
        for(int i=0;i<q;i++)
            pq.push({line[i].front(),i});
        

        while(k > 0){
            pii p= pq.top();
            pq.pop();

            line[p.S].pop();
            if(!line[p.S].empty())
                pq.push({line[p.S].front(),p.S});
            ans += p.F;
            k--;
        }
        cout<<ans<<"\n";
    }
    return 0;
}