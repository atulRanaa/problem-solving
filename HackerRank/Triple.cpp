// hackerank incom
#include <bits/stdc++.h>

using namespace std;

#define ll long long
int main() {
    ll n,x; scanf("%lld %lld",&n,&x);
    
    ll ar[n];
    for(int i=0;i<n;i++) scanf("%lld",&ar[i]);
    
    sort(ar,ar+n);
    ll ans = 0, temp1, temp2;
    
    for(int i=0;i<n;i++){
        
        for(int j=i+1; j<n;j++){

            temp1 = ar[i]*ar[j];
            //if(temp1 > x) break;
            
            for(int k = j+1; k<n;k++){

                temp2 = temp1 * ar[k];
                if(temp2 == x) ans++;
            }
        }
    }
    cout<< ans;
    return 0;
}
