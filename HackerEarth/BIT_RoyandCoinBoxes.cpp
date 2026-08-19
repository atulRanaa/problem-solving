// hackerearth
        #include <bits/stdc++.h>
             
        using namespace std;
         
        typedef long long ll; 
        const int N = 1e6+5; 
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
    ll x = 0;
    while(ind > 0){
        x += BIT[ind];  ind -= ind & (-ind);
    }
    return x;
}
 
void updateBIT(int n, int ind, ll val){ 
    while(ind <= n){
        BIT[ind] = (BIT[ind]+val);  ind += ind & (-ind);
    }
}
int main(){
    memset(BIT, 0, sizeof BIT);
    memset(ar, 0, sizeof ar);
    int n; scanf("%d",&n);
    int m; scanf("%d",&m);
    int x,y; char ch;
    
    while(m-- > 0){
        scanf("%d %d",&x,&y);
        updateBIT(n,  x, 1);
        updateBIT(n,y+1,-1);
        
        //for(int i=1;i <= n;i++) cout<<ar[i]<<" "; cout<<"\n";
    }
    for(int i=1;i <= n;i++) ar[i]= (get(i));
    int q; scanf("%d",&q);
    
    sort(ar+1,ar+n+1);
    while(q-- > 0){
        scanf("%d",&x);

        int val = n-distance(ar+1, lower_bound(ar+1,ar+n+1,x));

        printf("%d\n", val);
    }
    return 0;
}