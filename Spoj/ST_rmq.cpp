#include<bits/stdc++.h>
using namespace std;

#define N 1000005
#define ll long long
int ar[4*N],ST[4*N];

void build(int index,int start,int end){
    if(start==end){
        ST[index] = start;
        return;
    }
    int mid = (start+end)>>1;
    build(2*index,start,mid);
    build(2*index+1,mid+1,end);
    
    int a = ST[2*index], b = ST[2*index+1];
    ST[index] = (ar[a]<ar[b])?(a):(b);
}
int query(int index,int start,int end,int l,int r){
    if(r < start || end < l) return -1;
    if(l <= start &&  end <= r) return ST[index];

    int mid = (start+end)>>1;

    int a = query(2*index,start,mid,l,r);
    int b = query(2*index+1,mid+1,end,l,r);
    
    if(a == -1) return b;
    if(b == -1) return a;
    return (ar[a]<ar[b])?(a):(b);
}
int main(){
    
    int t; cin>>t;
    while(t-- > 0){
        int n,q; cin>>n;
        for(int i=1;i <= n;i++) cin>>ar[i];
        
        build(1,1,n);
        int x,y; cin>>q;
        while(q-- >0){
            cin>>x>>y;
            cout<< query(1,1,n,x,y) <<"\n";
        }
    }
    return 0;
}