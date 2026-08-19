        #include <cstring>
        #include <stdio.h>
        #include <string>
        #include <iostream>
        using namespace std;
     
        const int N = 1e6+5;
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
     
    ll getSum(int ind){ 
        ll sum = 0;
        while(ind > 0){
            sum += BIT[ind];  ind -= ind & (-ind);
        }
        return sum;
    }
     
    void updateBIT(int n, int ind, int val){ 
        while(ind <= n){
            //cout<<ind<<" ";
            BIT[ind] += val;  ind += ind & (-ind);
        }
       // cout<<"\n";
        //for(int i=1;i<=n;i++) cout<<BIT[i]<<" "; cout<<"\n";
    }
    void build(int n)
    {   
        for(int i=1; i<=n; i++)  updateBIT(n, i, 1);
    }
     
    int bs(ll k, int n){
        int l=1,mid,r=n;
        ll check;    
        while(l <= r){
            mid = (l+r)>>1;
            
            check = ar[mid]+getSum(mid);
            if(check == k)      return 1;
            else if( check > k) r = mid-1;
            else                l = mid+1;
        }
        return 0;
    }
        int main(){
            memset(BIT, 0, sizeof BIT);
            int n,q;
            scanf("%d",&n);
            
            
            
            for(int i=1;i<=n;i++) scanf("%lld",ar+i);
     
            //for(int i=1;i<=n;i++) cout<<BIT[i]<<" "; cout<<"\n";
            //build(n);
     
            //for(int i=1;i<=n;i++) cout<<BIT[i]<<" "; cout<<"\n";
            ll x,type; scanf("%d",&q);
            while(q-- > 0){
                scanf("%lld %lld",&type,&x);
     
                //cout<< type <<" "<< k<<"\n";
                if(type == 1){
                    updateBIT(n, 1, -1);
                    updateBIT(n, x+1,1);
                    //for(int i=1;i<=n;i++) cout<<getSum(i)<<" "; cout<<"\n";
                }
                else{
                    printf("%s\n",bs(x,n)?("yes"):("no"));
                }
            }
            return 0;
        }