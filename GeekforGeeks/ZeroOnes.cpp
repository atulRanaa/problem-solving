#include<stdio.h>
#include<algorithm>

using namespace std;
typedef long long ll;

int ones[100005],x=0;
int zero[100005],y=0;
int index_10[1000005];
ll  count_10[1000005];

int main(){
    
    int n; 				scanf("%d",&n);
    char str[100005];	scanf("%s",str);
    int q; 				scanf("%d",&q);
    
   
    for(int i=0;i<n;i++){
        if(str[i] == '1') ones[x++] = i;
        else zero[y++] = i;
    }
    
    for(int i=0; i< x; i++){
        index_10[i] = lower_bound(zero,zero+y, ones[i] )-zero;
        count_10[i] = y-index_10[i];
    }
    for(int i=1;i<x;i++) count_10[i] += count_10[i-1];
    
    ll k;
    while(q--){
        scanf("%lld",&k);
        int ind = lower_bound(count_10,count_10+x,k)-count_10;
        
        if(ind == x) printf("-1\n");
        else {
        	 
        	int mark = k-1-(((ind-1) <0)?0:count_10[ind-1]);		 
			printf("%d %d\n", ones[ind]+1, zero[index_10[ind]+mark]+1);
		}
    }
    return 0;
}
