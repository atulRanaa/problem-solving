#include <bits/stdc++.h>
using namespace std;

int main()
{
    int n,i,j;
    int prime[440]={0};
    prime[0]=1;
    prime[1]=1;
    long int len=sqrt(440);
    for(i=2;i<=len;i++){
        if(prime[i]==0){
            for(j=2*i;j<440;j+=i){
                prime[j]=1;
            }
        }
    }
    int ar[101][101];
    cin>>n;
    for(i=0;i<n;i++){
        for(j=0;j<n;j++) cin>>ar[i][j];
    }
    int count=0;
    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            int sum=0;
            if(i-1 >=0) sum+=ar[i-1][j]; 
            if(i+1 < n) sum+=ar[i+1][j];
            if(j-1 >=0) sum+=ar[i][j-1];
            if(j+1 < n) sum+=ar[i][j+1];
            if(prime[sum]==0) count++;
        }
    }
    cout<<count;
    return 0;
}
