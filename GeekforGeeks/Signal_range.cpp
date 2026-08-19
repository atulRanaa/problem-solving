#include<bits/stdc++.h>
#include<iostream>

using namespace std;

int main(){
  int t;
  long int n,i,j,temp,count;
  cin>>t;
      while(t--){
          cin>>n;
         long long int arr[n];
         for(i=0;i<n;i++) cin>>arr[i];
         for(i=0;i<n;i++){ 
           count=1;
           temp=arr[i];
           j=i-1;
           while(j>=0 && arr[j]<=temp)
           { count++;j--;
           }
          cout<<count<<" ";
        } 
      cout<<endl;
      }
  return 0;
}
