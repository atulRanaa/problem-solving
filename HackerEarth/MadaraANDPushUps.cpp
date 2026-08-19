#include<bits/stdc++.h>
using namespace std;

int main(){
ios_base::sync_with_stdio(false);
cin.tie(NULL);
     int t;
     cin>>t;
     while(t--){
        int n,a,d,m;
        cin>>n>>a>>d>>m;
        int count =0;
        while(n > 0){
              n-=a;
              a-=d;
              if(a < m) a = m;

              count++;
        }
        cout<<count<<endl;
     }
return 0;
}