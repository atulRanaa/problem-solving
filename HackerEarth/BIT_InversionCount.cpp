#include <bits/stdc++.h>
using namespace std;

int ct[100010]={0};
int getSum(int BITree[], int index){
    int sum = 0; 
    while (index > 0){
        sum += BITree[index]; index -= index & (-index);
    }
    return sum;
}
void updateBIT(int BITree[], int n, int index, int val){
    while (index <= n)    {
       BITree[index] += val; index += index & (-index);
    }
}
void convert(int arr[], int n)
{
    int temp[n];
    for (int i=0; i<n; i++) temp[i] = arr[i];
    sort(temp, temp+n);
    for (int i=0; i<n; i++){
        arr[i] = lower_bound(temp, temp+n, arr[i]) - temp + 1;
    }
}
 
void getInvCount(int arr[], int n)
{
    int invcount = 0;
    convert(arr, n);
 
    //for(int i=0;i<n;i++) cout<<arr[i]<<" "; cout<<endl;
        
    int BIT[n+1];
    for (int i=1; i<=n; i++)  BIT[i] = 0;
    for (int i=n-1; i>=0; i--){
        invcount = getSum(BIT, arr[i]-1);
        ct[i] = invcount;
        updateBIT(BIT, n, arr[i], 1);
        
        //for(int i=0;i<=n;i++) cout<<BIT[i]<<" "; cout<<endl;
    }
}
int main()
{
    int ar[100010],arr[100010];
    int t,n;
    cin>>t;
    while(t--){
        cin>>n;
        for(int i=0;i<n;i++){ cin>>ar[i]; arr[n-1-i] = ar[i];}
        getInvCount(ar,n);
        for(int i=0;i<n;i++) cout<<ct[i]<<" "; cout<<endl;
        
        getInvCount(arr,n);
        for(int i=0;i<n;i++) cout<<ct[n-1-i]<<" "; cout<<endl;
	
    }
    return 0;
}
