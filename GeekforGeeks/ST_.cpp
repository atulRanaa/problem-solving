#include <stdio.h>
#include <string.h>
#define N 100010
int tmp[N];
void sort(int * arr,int l,int r,int (*cmp)(int,int))
{
	if(l>=r) return;
	int m = (l+r)>>1;
	sort(arr,l,m,cmp);
	sort(arr,m+1,r,cmp);
	int i=l,j = m+1;
	int sz=0;
	while(i<=m || j<=r)
	{
		if(j>r || i<=m && cmp(arr[i],arr[j])<=0)
		{
			tmp[sz++] = arr[i++];
		}
		else tmp[sz++] = arr[j++];
	}
	memcpy(&arr[l],tmp,sizeof(int)*sz);
}
int arr[N];
int idx[N];
int hits[N];
void inc(int idx)
{
	while(idx<N) hits[idx]++,idx|=idx+1;
}
int query(int idx){
	int ans=0;
	while(idx>=0) ans+=hits[idx],idx=((idx+1)&idx)-1;
	return ans;
}
int search(int x,int n)
{
	if(arr[idx[n-1]]-query(n-1)<=x) return -1;
	int l = 0,r = n-1;
	while(l!=r)
	{
		int m = (l+r)>>1;
		if(arr[idx[m]]-query(m)>x) r = m;
		else l= m+1;
	}
	return l;
}
int cmp(int l,int r)
{
	return arr[l]-arr[r];
}
int main()
{
    int n;
    scanf("%d",&n);
    for(int i=0;i<n;++i)
    {
    	scanf("%d",&arr[i]);
    	idx[i]=i;
    }
    sort(idx,0,n-1,&cmp);
    int m;
    scanf("%d",&m);
    while(m--)
    {
    	int x;
    	scanf("%d",&x);
    	int i = search(x,n);
    	if(i!=-1) inc(i);
    }
    for(int i=0;i<n;++i)
    {
    	arr[idx[i]]-=query(i);
    }
    for(int i=0;i<n;++i) printf("%d ",arr[i]);
    puts("");
    return 0;
}