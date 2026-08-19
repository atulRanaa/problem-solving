#include <bits/stdc++.h>
 
using namespace std;
 
#define pb push_back
#define mp make_pair
 
const ll MAXN = 4e5+5;
 
typedef long long ll;
typedef pair<ll, ll>  pii;
typedef pair<ll, ll> pll;
 
 
ll T, N, K;
ll sol;
ll ar[100005];
struct trie
{
    ll num[2];
    ll next[2];
    trie()
    {
        num[0] = num[1] = 0;
        next[0] = next[1] = -1;
    }
};
 
vector<trie> v;
 
void add(ll x)
{
    ll vind = 0;
    for (ll i=22; i>=0; --i)
    {
        bool f = x & (1 << i);
        if (v[vind].next[f] == -1)
        {
            v[vind].next[f] = v.size();
            v.pb(trie());
        }
        v[vind].num[f]++;
        vind = v[vind].next[f];
    }
}
 
void init()
{
    v.clear();
    sol = 0;
    v.pb(trie());
    add(0);
}
 
ll query(ll x, ll K)
{
    ll vind = 0;
    ll ret = 0;
    for (ll i=22; i>=0; --i)
    {
        if (vind == -1)
            break;
        bool kbit = K & (1 << i);
        bool xbit = x & (1 << i);
        bool xneg = 1-xbit;
        if (kbit)
        {
            ret += v[vind].num[xbit];
            vind = v[vind].next[xneg];
        }
        else
        {
            vind = v[vind].next[xbit];
        }
    }
    return ret;
}
 
ll XorlessthanK(ll K){
    init();
        ll pref=0;
        for (ll i=0; i<N; ++i)
        {
            pref ^= ar[i];
            //ll k = query(pref, K);
            //cout<<k<< " ";
            sol += query(pref, K);
            add(pref);
        }
        //for (ll i=0; i<=6; ++i)
            //cout<<v[i].num[0]<<" "<<v[i].num[1]<<endl;
    return sol;
}
ll main()
{
    ll term;
    cin>>N>>term;
    
    term = (N*(N+1))/2 - term;
    for(ll i=0;i<N;i++) scanf("%d",&ar[i]);
    
    ll l=0,r = (1<<20);
    ll check1, check2; ll mid, ans;
    
    while(l <= r){
        mid = l + (r-l)/2;
        check1 = XorlessthanK(mid);
        
        if(check1 > term){
            r = mid-1;
        }
        else l = mid+1;
    }
    cout<<r<<"\n";
    return 0;
}