#include <bits/stdc++.h>
     
using namespace std;

typedef long long ll; 
const int N = 1e5+5; 
#define MOD 1000000007
int n,a[N];
ll segtree_max[4*N];
ll segtree_min[4*N];
ll lazy[4*N];
 
void build(int low,int high,int index)
{
	if(low==high)
	{
		segtree_max[index] = a[low];
		segtree_min[index] = a[low];
		return;
	}
	int mid=(low+high)/2;
	build(low,mid,2*index);
	build(mid+1,high,2*index+1);
	segtree_max[index] = max(segtree_max[2*index],segtree_max[2*index+1]);
	segtree_min[index] = min(segtree_min[2*index],segtree_min[2*index+1]);
}
 
void lazy_prop(int start,int end,int index,int x)
{
	if(lazy[index]!=0){
		segtree_max[index] -= lazy[index];
		segtree_min[index] -= lazy[index];
		if(start==end)
		{
			a[start] -= lazy[index];
		}
		else
		{
			lazy[2*index] += lazy[index];
			lazy[2*index+1] += lazy[index];
		}
		lazy[index] = 0;
	}
	if(x>=segtree_max[index]) return;
	if(x< segtree_min[index]){
		segtree_min[index] -= 1;
		segtree_max[index] -= 1;
		if(start==end)
		{
			a[start] -= 1;
		}
		else
		{
			lazy[2*index] += 1;
			lazy[2*index+1] += 1;
		}
		return;
	}
	if(start==end) return;
	int mid = (start+end)/2;
	lazy_prop(start,mid,2*index,x);
	lazy_prop(mid+1,end,2*index+1,x);
	segtree_max[index] = max(segtree_max[2*index],segtree_max[2*index+1]);
	segtree_min[index] = min(segtree_min[2*index],segtree_min[2*index+1]);
}
 
void finl(int start,int end,int index)
{
	if(lazy[index]!=0){
		segtree_max[index] -= lazy[index];
		segtree_min[index] -= lazy[index];
		if(start==end)
		{
			a[start] -= lazy[index];
		}
		else
		{
			lazy[2*index] += lazy[index];
			lazy[2*index+1] += lazy[index];
		}
		lazy[index] = 0;	
	}
	if(start==end) return;
	int mid = (start+end)/2;
	finl(start,mid,2*index);
	finl(mid+1,end,2*index+1);
	segtree_max[index] = max(segtree_max[2*index],segtree_max[2*index+1]);
	segtree_min[index] = min(segtree_min[2*index],segtree_min[2*index+1]);	
}
 
int main()
{	
	int i,q;
	scanf("%d",&n);
	for(i=1;i<=n;i++) cin>>a[i];
	
	build(1,n,1);
	scanf("%d",&q);
	while(q--){
		int x;
		scanf("%d",&x);
		lazy_prop(1,n,1,x);
	}
	finl(1,n,1);
	for(i=1;i<=n;i++) printf("%d ",a[i]);
	return 0;
}
