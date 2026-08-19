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
    ll BIT[N], ans = 0LL;
    ll cnt[N];
    int ar[N];
ll getSum(int ind){ 
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
    for (int i=n; i>=1; i--){
        cnt[i]  = getSum(ar[i]-1);
        ans += cnt[i];
        updateBIT(n, ar[i], 1);
    }
    memset(BIT, 0, sizeof BIT);
    for (int i=1; i<=n; i++){
        cnt[i]  += getSum(ar[i]-1);  updateBIT(n, ar[i], 1);
    }
}
    int main(){


        int t; scanf("%d",&t);
        while(t-- > 0){
            
            int n;  scanf("%d",&n);
            for(int i=1;i<=n;i++) scanf("%d",ar+i);

            //for(int i=1;i<=n;i++) cout<<BIT[i]<<" "; cout<<"\n";
            //for(int i=1;i<=n;i++) cout<<cnt1[i]<<" "; cout<<"\n";

            
            build(n);
            //for(int i=1;i<=n;i++) cout<<cnt[i]<<" "; cout<<"\n";
            //for(int i=1;i<=n;i++) ans += cnt[i];

            //for(int i=1;i<=n;i++) cout<<cnt[i]<<" "; cout<<"\n";
            int temp = ans;
            //cout<<temp <<"\n";
            for(int i=2;i<=n;i++){
                temp = (temp- cnt[i-1] + n-1-cnt[i-1]);
                ans ^= temp;
                //cout<<temp<<"\n";
            }
            printf("%lld\n", ans);
        }
        return 0;
    }