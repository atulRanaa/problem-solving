#include <bits/stdc++.h>
using namespace std;
int trie[10001*31][2], nxt;

void add(int x)
{
    int root=1;
    for(int i=20; i>=0; i--)
    {
        int b=(x>>i)&1;
        if(!trie[root][b])
            trie[root][b]=nxt++;
        root=trie[root][b];
    }
}
int ask(int x)
{
    int ret=0, root=1;
    for(int i=20; i>=0; i--)
    {
        int b=(x>>i)&1;
        if(trie[root][b^1])
        {
            ret|=1<<i;
            root=trie[root][b^1];
        }
        else
            root=trie[root][b];
    }
    return ret;
}

int dp[902];

int main(){
    int n;  cin>>n;
    int ans=0;
    
    int ar[n+5];
    for(int i=1; i<=n; i++) cin>>ar[i];
    dp[0] = 0;
    for(int i=1; i<=n; i++) dp[i] = dp[i-1]+ar[i-1];
    
    memset(trie , 0, sizeof(trie));
    nxt = 2;
    add(0);
    for(int i=1;i<n;i++){
        int s = 0;
        for(int j=i;j>=1;j--){
            s  += ar[j];
            add(s);
        }
        s = 0;
        for(int j =i+1;j<=n;j++)
        {
            s+=ar[j];
            ans = max(ans,ask(s));
        }
    }
    cout<<ans<<endl;
return 0;
}