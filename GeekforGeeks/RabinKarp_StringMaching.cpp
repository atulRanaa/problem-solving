using namespace std;
#include <bits/stdc++.h>
#define ll long long
#define mod 1000000007
#define max_sz 200000
#define S second
#define F first


const int kBase =26, kMod = 997;
int RabinKarp(const string &t, const string &s){
    if(s.size() > t.size()) return -1;
    int t_hash =0, s_hash=0, power_s=1;

    for(int i=0;i< s.size();++i){
        power_s = i?(power_s*kBase)%kMod : 1;
        
        s_hash = (s_hash*kBase + s[i])%kMod;
        t_hash = (t_hash*kBase + t[i])%kMod;
    }

    for(int i=s.size(); i<t.size();i++){
        if(t_hash == s_hash && !t.compare(i-s.size(), s.size(),s))
            return i-s.size();

        t_hash -= (t[i-s.size()]*power_s)%kMod;
        if(t_hash < 0 ) t_hash += kMod;
        t_hash = (t_hash*kBase + t[i])%kMod;
    }
    if(t_hash == s_hash && t.compare(i-s.size(), s.size(),s)==0)
        return t.size()-s.size();
    return -1;
}
int main(){

    ios::sync_with_stdio(0), cin.tie(0);
	int t; cin>>t;

    for(int c=0;c<t;c++){
        string t,s;
        cin>>t;
        cin>>s;

        if(RabinKarp(t,s)) cout<<"YES\n"; else cout<<"NO\n";
    }
    return 0;
}
