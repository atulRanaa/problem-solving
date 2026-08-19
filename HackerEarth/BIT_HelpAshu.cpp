    #include <cstring>
    #include <stdio.h>
    #include <string>
    #include <iostream>
    using namespace std;

    const int N = 1e5+5;
    #define ll long long
    #define MOD 1000000007
    #define pii pair<int,int>
    #define pis pair<int,string>
    #define F first
    #define S second
    #define LCM(a,b) ((a*b)/__gcd(a,b))
    #define inf 1e15
    ll BIT[N];
    ll ar[N];
ll get(int ind){ 
    ll sum = 0;
    for(;ind > 0; ind -= ind & (-ind))  sum += BIT[ind];
    return sum;
}
 
void updateBIT(int n, int ind, int val){ 
    for(;ind <= n;ind += ind & (-ind))  BIT[ind] += val;
    //for(int i=1;i<=n;i++) cout<<BIT[i]<<" "; cout<<"\n";
}
void build(int n)
{   
    for (int i=1; i<=n; i++){
        if(ar[i]&1LL) updateBIT(n,i,1);
    }
}
    int main(){


        int n;  scanf("%d",&n);
            for(int i=1;i<=n;i++) scanf("%lld",ar+i);
        int q;  scanf("%d",&q);
        ll type,x, y,ans;
        
        build(n);
        while(q-- > 0){
            scanf("%lld %lld %lld",&type,&x,&y);

            if(type==0){
                if(y&1 && ar[x]%2==0){
                    updateBIT(n,x,1);
                }
                else if(y%2==0 && ar[x]&1){
                    updateBIT(n,x,-1);
                }
                ar[x] = y;
            }
            else if(type==1){
                ans = y-x+1 - (get(y)-get(x-1));
                printf("%lld\n", ans);
            }
            else{
                ans = get(y)-get(x-1);
                printf("%lld\n", ans);
            }
            
        }
        return 0;
    }