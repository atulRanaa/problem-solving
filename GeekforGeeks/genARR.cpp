#include <bits/stdc++.h>

using namespace std;
#define ll long long
#define mod 1000000007
#define max_sz 200000
#define S second
#define F first



int main()  {
    int q;  scanf("%d",&q);
    
    ll lim = (1<<q)-1;
    ll ar[ lim +5];

    for(int i=0;i<lim;i++) scanf("%lld",&ar[i]);
    
    sort(ar,ar+lim, greater<ll>());
    for(int i=0;i<lim;i++)  printf("%lld ",ar[i]); cout<<"\n";
    unordered_set<ll> gd; vector<ll> ans;
    
    for(int i=0;i<lim;i++){
        if( gd.find(ar[i]) == gd.end()){

            int l = ans.size();

            for(int x=0;x<l;x++){
                ll temp = ar[i];
                for(int y=x;y<l;y++){
                    temp = __gcd(temp, ans[y]); gd.insert(temp);
                }
            }
            ans.push_back(ar[i]);
        }
        for(auto ele: gd)  printf(":%lld ",ele); cout<<"\n";
        for(auto ele: ans)  printf("%lld ",ele); cout<<"\n";
    }
    set<ll> h(ans.begin() , ans.end());
    for(auto ele: h)  printf("%lld ",ele); cout<<"\n";
    return 0;
}
