#include <bits/stdc++.h>
using namespace std;

#define N 1000005
#define ll long long
#define MOD 1000000007
ll ar[N];
vector<int> tree[4*N];

void merge(int cur,int lt,int rt){
    int i=0,j=0,x = tree[lt].size(), y= tree[rt].size();

    while(i < x && j < y){
        if(tree[lt][i] <= tree[rt][j])
            tree[cur].push_back(tree[lt][i++]);
        else
            tree[cur].push_back(tree[rt][j++]);
    }
    while(i < x) tree[cur].push_back(tree[lt][i++]);
    while(j < y) tree[cur].push_back(tree[rt][j++]);
}

void build(int cur,int l,int r ){
    if( l==r ){
            tree[cur].push_back( ar[ l ] );
            return ;
    }
    int mid = l+(r-l)/2;
    build(2*cur, l , mid);          // Build left tree 
    build(2*cur+1 , mid+1 , r);     // Build right tree
    merge(cur, 2*cur, 2*cur+1);     //Merging the two sorted arrays
}
ll query( int cur, int l, int r, int x, int y, int k){
        if( r < x || l > y ){
               return 0; //out of range
        }
        if( x<=l && r<=y )
        {
              //Binary search over the current sorted vector to find elements smaller than K
                int i = upper_bound(tree[cur].begin(),tree[cur].end(),k)-tree[cur].begin();
                return (i-1>=0)?(tree[cur][i-1]):(0);
        }
        int mid=l+(r-l)/2;
        return max(query(2*cur,l,mid,x,y,k), query(2*cur+1,mid+1,r,x,y,k));
}

int main(){
    
    int t; scanf("%d",&t);
    while(t-- > 0){

        for(int i=0;i < 4*N;i++) tree[i].clear();

        int n,q; scanf("%d %d",&n,&q);
        for(int i=1;i<=n;i++) scanf("%lld", ar+i);
        int j;

        build(1,1,n);
        while(q-- > 0){
            scanf("%d",&j);
            ll ans = query(1,1,n,1,j-1, ar[j]) * ar[j] * query(1,1,n,j+1,n, ar[j]);

            printf("%lld\n",  ans);
        }
    }
    return 0;
}
