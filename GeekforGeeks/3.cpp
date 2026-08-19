#include <vector>
#include <iostream>
#include <string>
#include <cstring>
#define mod 1000000007
#define ll long long

using namespace std;
int main(){
    ll n,m; char ch; cin>>n;
    
    vector<ll> fol[26];
    for(ll i=0;i<26;i++){
        cin>>m;
        for(ll j=0;j<m;j++){
            cin>>ch; fol[i].push_back(ch-'a');
        }
    }
    ll A=0,B=0;
    vector<ll> conn[26];
    vector<ll> scl;
    for(ll i=0;i<26;i++){
        for(ll j: fol[i]){
            if(j == i){
                A++;
                scl.push_back(i);
                conn[i].push_back(i);

                cout<<(char)(i+'a')<<" "<<(char)(i+'a')<<"\n";
            }
            else{
                for(ll k: fol[j]){
                    if(k == i){
                        B++;
                        conn[i].push_back(j);
                        cout<<(char)(i+'a')<<" "<<(char)(j+'a')<<"\n";
                    }
                }
            }
        }
    }
    if(n==1) cout<<26<<"\n";
    else if(n==2) cout<<A<<"\n";
    else{
        long long ans=0;
        ll l = (n+1)/2, val = A+B;
        long long H[26][l];
        
        memset(H, 0, sizeof H);

        for(ll i=0;i<26;i++) H[i][0]=1LL;
        for(ll i=1;i<l;i++){
            for(ll j=0;j<26;j++){
                for(ll k: conn[j])
                    H[k][i] = (H[k][i]+H[j][i-1])%mod;
            }
        }
        if(n&1LL)
            for(ll i=0;i<26;i++) ans = (ans+H[i][l-1])%mod;
        else
            for(ll i: scl) ans = (ans+H[i][l-1])%mod;
        cout<< ans<<"\n";
    }
    //cout<<A<<" "<<B<<"\n";
    return 0;
}