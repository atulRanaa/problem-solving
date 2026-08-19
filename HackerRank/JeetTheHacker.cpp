#include<bits/stdc++.h>
 
using namespace std;
 
#define pb push_back
#define all(x) x.begin(),x.end()
#define ll long long int
#define vi vector<long long int>
 
int main(){
    ios_base::sync_with_stdio(false);cin.tie(0);
    ll n,p=0,q=0;
    cin>>n;
    if(n%2==1)  p=n*((n+1)/2)*(n+1)-n;
    else  p=(n/2)*(1+n*n);
    cout<<p;
return 0;
