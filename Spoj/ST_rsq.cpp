#include <bits/stdc++.h>
using namespace std;

#define N 1000005
#define ll long long
#define MOD 1000000007
ll ar[4*N]={0},ST[4*N]={0};

void build(int index,int start,int end){
    if(start==end){
        ST[index] = start;
        return;
    }
    int mid = (start+end)>>1;
    build(2*index,start,mid);
    build(2*index+1,mid+1,end);
    
    ST[index] = (ST[2*index]+ST[2*index+1]) % MOD;
}
void update(int index,int start,int end,int x,int y){
    if(start == end){
        ST[index] = max(0LL,ST[index]+y);
    }else{
        int mid = (start+end)/2;
        if(start <= x && x <= mid)
        	update(2*index, start,mid,x,y);
        else
        	update(2*index+1,mid+1,end,x,y);
        ST[index] = (ST[2*index]+ST[2*index+1]);
    }
}
ll query(int index,int start,int end,int l,int r){
    if(r < start || end < l) return -1;
    if(l <= start &&  end <= r) return ST[index];

    int mid = (start+end)>>1;

    ll a = query(2*index,start,mid,l,r);
    ll b = query(2*index+1,mid+1,end,l,r);
    
    if(a == -1) return b;
    if(b == -1) return a;
    return (a+b);
}
int main(){
    
    int n,q; scanf("%d %d",&n,&q);
    int x,y;
    while(q-- > 0){
        
        scanf("%d",&x);
        if(x==1){
        	scanf("%d",&y);	update(1,1,n,y,1LL);
        }
        else if(x==2){
        	scanf("%d",&y);	update(1,1,n,y,-1LL);
        }
        else{
        	scanf("%d %d",&x,&y); cout<< query(1,1,n,x,y) <<"\n";
        }
    }
    return 0;
}