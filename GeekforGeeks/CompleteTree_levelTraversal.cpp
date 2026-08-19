#include <bits/stdc++.h>
         
    using namespace std;
     
    typedef long long ll; 
    const int N = 2e6+5; 
    #define MOD 1000000007
    #define pii pair<int,int>
    #define pis pair<int,string>
    #define F first
    #define S second
    #define LCM(a,b) ((a*b)/__gcd(a,b))
    #define inf 1e10
    ll ar[N];
    ll lvl[50];

    void levelSum(ll n){
        ll level = 0;
        while(true){
            ll left = (1<<level);
            ll right= min(left+left,n+1);
            ll sum = 0;
            while(left < right){ sum+=ar[left];left++;}
            
            lvl[level+1] = sum;
            
            if(right > n) break;
            level++;
        }
    }
    int main(){
        ios_base::sync_with_stdio(false);
        cin.tie(NULL);
        
        
        ll n,q; cin>>n>>q;
        for(int i=1;i<=n;i++) cin>>ar[i];
        
        levelSum(n);
        int type,x,lv,l,r,val;
        
        while(q-- > 0){
            cin>>type;
            if(type == 1){
                cin>>x>>lv;
                x-=1, lv-=1;  cout<<ar[(1<<lv)+x]<<"\n";
            }
            else if(type == 2){
                cin>>l>>r;
                ll ans = 0;
                for(int i=l;i<=r;i++) ans+=lvl[i];
                cout<<ans<<"\n";
            }else{
                cin>>x>>lv>>val;
                x-=1, lv-=1;
                
                ll pos = (1<<lv)+x;
                lvl[lv+1] += (val-ar[pos]);
                ar[pos] = val;
            }
        }
        return 0;
    }




