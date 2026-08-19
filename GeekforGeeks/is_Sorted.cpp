#include<bits/stdc++.h>
using namespace std;

int main(){
ios_base::sync_with_stdio(false);
cin.tie(NULL);
     long n,i,temp;
     vector< long int > v;
     
     cin>>n;
     for(i=0;i<n;i++){
         cin>>temp;
         v.push_back(temp);
     }
     if(is_sorted(v.begin(),v.end())) cout<<"Sorted";
     else cout<<"NO";
return 0;
}
