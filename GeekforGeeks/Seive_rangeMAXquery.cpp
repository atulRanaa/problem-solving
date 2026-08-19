#include <bits/stdc++.h>
using namespace std;
 
#define ll long long
#define mp make_pair
#define pb push_back
#define pii pair <int, int>
#define vi vector<int>

#define FOR(i, a, b) for (int i=a; i<b; i++)
#define FORd(i, a, b) for (int i=a-1; i>=b; i--)
#define F0Rd(i, a) for (int i=a-1; i>=0; i--)
#define F0R(i, a) for (int i=0; i<a; i++)
 
#define f first
#define s second
#define be begin
#define e end
#define lb lower_bound
#define ub upper_bound
 
const int MOD = 1000000007;
/*std::cout << std::fixed;
std::cout << std::setprecision(2);*/
const int N = 1000000;
int n;
int dp[1000001];
int input[1000001], seg[1000001],t,x,a[1000001],ans=10000000;
vector<vector<int>>v(1000001);
void constructTree(ll pos,ll low,ll high){
    if(low == high){
        seg[pos] = input[low];
        return;
    }
    ll mid = (low+high)/2;
    constructTree(2*pos+1,low,mid);
    constructTree(2*pos+2,mid+1,high);
    seg[pos]=max(seg[2*pos+1],seg[2*pos+2]);
}

ll rangemaxQuery(ll pos,ll low,ll high,ll qlow,ll qhigh){
    if(qlow<=low&&qhigh>=high)
        return seg[pos];
    if(qlow>high || qhigh<low)
        return 0;
    ll mid = (low+high)/2;
    return max(rangemaxQuery(2*pos+1,low,mid,qlow,qhigh),rangemaxQuery(2*pos+2,mid+1,high,qlow,qhigh));
}

void seive() {
    F0R(i,1000001) dp[i] = 1;
    dp[0]=0,dp[1]=0;
    for (int p=2; p*p<=1000000; p++)
    {
        if (dp[p] == 1)
        {
            for (int i=p*2; i<=1000000; i += p)
                dp[i]++;
        }
    }
}

bool cmp(int x,int y) {
    return x > y;
}

int main()
{
    ios::sync_with_stdio(0), cin.tie(0);
    seive();
    cin >> x >> n;
    F0R(i,n) cin >> t,a[i]=t,input[i] = (dp[t]==1?1:dp[t]-1);
    F0Rd(i,n) v[input[i]].pb(i);
    constructTree(0,0,n-1);
    F0R(i,n-x+1) {
       int z = rangemaxQuery(0,0,n-1,i,i+x-1);
       int f = lb(v[z].begin(),v[z].end(),i+x-1,cmp)-v[z].begin();
       ans = min(a[v[z][f]],ans);
    }
    cout << ans;
    return 0;
}
