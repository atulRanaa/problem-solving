#include<bits/stdc++.h>
using namespace std;

#define lli long long int

lli L[100001],O[100001],W[100001];
lli l[100001],r[100001]; 

lli arr[500000]={0};
lli seg[500000]={0};
lli sum[500000]={0};
int n,q;
void constructTree(lli pos,lli low,lli high){
    if(low == high){
        seg[pos] = sum[low];
        return;
    }
    lli mid = (low+high)/2;
    constructTree(2*pos+1,low,mid);
    constructTree(2*pos+2,mid+1,high);
    seg[pos]=max(seg[2*pos+1],seg[2*pos+2]);
}

lli rangemaxQuery(lli pos,lli low,lli high,lli qlow,lli qhigh){
    if(qlow<=low&&qhigh>=high)
        return seg[pos];
    if(qlow>high || qhigh<low)
        return 0;
    lli mid = (low+high)/2;
    return max(rangemaxQuery(2*pos+1,low,mid,qlow,qhigh),rangemaxQuery(2*pos+2,mid+1,high,qlow,qhigh));
}
void convert(int N)
{
    lli temp[N];
    for (int i=0; i<N; i++) temp[i] = arr[i];
    sort(temp, temp+N);
    for(int i=0; i<n; i++){
        L[i] = lower_bound(temp, temp+N, L[i]) - temp;
        O[i] = lower_bound(temp, temp+N, O[i]) - temp;
    }
    for(int i=0; i<q; i++){
    	l[i] = lower_bound(temp, temp+N, l[i]) - temp;
        r[i] = lower_bound(temp, temp+N, r[i]) - temp;
    }
}
int main(){
	

   	freopen("input.txt","r",stdin);
	freopen("output.txt","w",stdout);
	scanf("%d %d",&n,&q);
	
	for(int i=0;i<n;i++) scanf("%lld %lld %lld",&L[i],&O[i],&W[i]);
	for(int i=0;i<q;i++) scanf("%lld %lld",&l[i],&r[i]);

	int N=0;
	for(int i=0;i<n;i++){arr[N++] = L[i];arr[N++] = O[i];}
	for(int i=0;i<q;i++){arr[N++] = l[i];arr[N++] = r[i];}

	convert(N);
	
	for(int i=0;i<n;i++){
		sum[ L[i]] += W[i];
		sum[ O[i]+1] -= W[i];
	}
	for(int i=1;i<=N;i++) sum[i] += sum[i-1];
	
	constructTree(0,0,N-1);
	for(int i=0;i<q;i++){

		lli ans = rangemaxQuery(0,0,N-1,l[i],r[i]);
		printf("%lld\n",ans);
	}
	return 0;
}
