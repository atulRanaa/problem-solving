#include <bits/stdc++.h>
using namespace std;
int N, M;
int A[10001][21];
int B[10001];
int trie[10001*31][2], nxt;

void add(int x)
{
    int root=1;
    for(int i=30; i>=0; i--)
    {
        int to=(x>>i)&1;
        if(!trie[root][to])
            trie[root][to]=nxt++;
        root=trie[root][to];
    }
}
int ask(int x)
{
    int ret=0;
    int root=1;
    for(int i=30; i>=0; i--)
    {
        int to=(x>>i)&1;
        if(trie[root][to^1])
        {
            ret|=1<<i;
            root=trie[root][to^1];
        }
        else
            root=trie[root][to];
    }
    return ret;
}
int main()
{
    scanf("%d%d", &N, &M);
    for(int i=1; i<=N; i++)
    {
        for(int j=1; j<=M; j++)
        {
            scanf("%d", A[i]+j);
            A[i][j]^=A[i][j-1];
        }
    }
    int ans=0;
    for(int j=1; j<=M; j++)
    {
        for(int k=j; k<=M; k++)
        {
            memset(trie, 0, sizeof trie);
            nxt=2;
            add(0);
            for(int i=1; i<=N; i++)
            {
                B[i]=B[i-1]^A[i][j-1]^A[i][k];
                ans=max(ans, ask(B[i]));
                add(B[i]);
            }
        }
    }
    printf("%d\n", ans);
    return 0;
}