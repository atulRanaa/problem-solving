#include<bits/stdc++.h>
using namespace std;

int main(){
    
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    
    int t;
    long long int n,k,tm;
    long long int j,ans,t1;
    scanf("%d",&t);

    for(int i=1;i<=t;i++){
       scanf("%lld %lld",&n,&k);


        queue< long long int > q;
        q.push(n);
        k--; 
        while(k){
          tm = q.front();
          q.pop();
          t1 = tm/2;
          if(tm&1){
             q.push(t1);
             q.push(t1);
          }else{
             q.push(t1);
             q.push(t1-1);
          }
          k--;
        }
        tm = q.front();
        ans = tm/2;
        if(tm&1){
            printf("Case #%d: %lld %lld\n",i,ans,ans);
        }
        else{
            printf("Case #%d: %lld %lld\n",i,ans,ans-1);
        }
        
    }
	return 0;
}
