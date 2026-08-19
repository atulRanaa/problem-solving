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
    int BIT[N];

int getSum(int ind){ 
    int sum = 0;
    while(ind > 0){
        sum += BIT[ind];  ind -= ind & (-ind);
    }
    return sum;
}
 
void updateBIT(int n, int ind, int val){ 
    while(ind <= n){
        BIT[ind] += val;  ind += ind & (-ind);
    }
    //for(int i=1;i<=n;i++) cout<<BIT[i]<<" "; cout<<"\n";
}
void build(int n)
{   
    for(int i=1; i<=n; i++)  updateBIT(n, i, 1);
}

int bs(int k,int n){
    int l=1,mid,r=n, ans = -1;
    int check;    
    while(l <= r){
        mid = (l+r)/2;
        
        check = getSum(mid);
        if( check >= k){
            ans = mid;
            r = mid-1;
        }
        else
            l = mid+1;
    }
    return ans;
}
    int main(){
        memset(BIT, 0, sizeof BIT);
        int n,q;
        scanf("%d",&n);
        scanf("%d",&q);
        int ar[n+1];
        
        for(int i=0;i<=n;i++) ar[i]=1;

        //for(int i=1;i<=n;i++) cout<<BIT[i]<<" "; cout<<"\n";
        build(n);

        //for(int i=1;i<=n;i++) cout<<BIT[i]<<" "; cout<<"\n";
        int type, k, temp=n;
        while(q-- > 0){
            scanf("%d %d",&type, &k);

            //cout<< type <<" "<< k<<"\n";
            if(type == 0){
                if(ar[k]==1) updateBIT(n,k,-1);
                ar[k]=0;
            }
            else{
                printf("%d\n", bs(k,n));
            }
        }
        return 0;
    }