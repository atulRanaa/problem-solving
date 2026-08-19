#include<bits/stdc++.h>

using namespace std;
#define ll long long int

ll merge(int *arr, int l,int mid,int r){
    

    ll inv=0;
    int L[mid-l+1],R[r-mid+1],p=0,q=0,i,j,k;
    for(i=l;  i<=mid;i++) L[p++] = arr[i];
    for(i=mid+1;i<=r;i++) R[q++] = arr[i];
    i=0,j=0,k=l;

    while(i < p && j < q){
        if(L[i] <= R[j]) arr[k++] = L[i++];
        else arr[k++] = R[j++], inv += (p-i);
    }
    while(i < p) arr[k++] = L[i++];
    return inv;
}

ll mergeSort(int *arr, int l,int r){
    ll ans = 0;
    if(l < r){
        int mid = l+(r-l)/2;
        ans = mergeSort(arr, l , mid) + mergeSort(arr,mid+1, r) + merge(arr,l,mid,r);
    }
    return ans;
}

int main(){
    int t; cin>>t;
    while(t--){
        int n; cin>>n;
        int ar[n];
        for(int i=0;i<n;i++) cin>>ar[i];

        cout<<mergeSort(ar, 0, n-1)<<"\n";
    }
    return 0 ;
}