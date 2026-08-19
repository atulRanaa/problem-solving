#include<bits/stdc++.h>

using namespace std;

#define pb push_back
#define all(x) x.begin(),x.end()
#define ll long long int
#define vi vector<long long int>
#define max INT_MAX   //2147483647
#define min INT_MIN  //-2147483647

bool cmp(const pair<int,int> &a, const pair<int,int> &b){ return (a.first < b.first);}

int main(){
    ios_base::sync_with_stdio(false);cin.tie(0);
    ll n,x;   cin>>n;

    vector< pair<ll,ll> >v;
    vector< pair<ll,ll> >::iterator it;
    for (int i = 1; i <= n; ++i){
    	cin>>x;
    	v.push_back(make_pair(x,i));
    }
    ll sum;  cin>>sum;
    sort(v.begin(),v.end(),cmp);

    ll temp=0,p,q,c=0;
    for (it = v.begin(); it !=v.end(); ++it){
    	p=it->first;
        q=it->second;

    	temp=temp+p*q;
    	c=c+q;
    	if(temp>sum){
    		while(temp>sum){
    			temp=temp-p;
    			c--;
    		}
    		cout<<c; return 0;
    	}
    	//cout<<temp<<"\n";
    }
    cout<<c;
    return 0;
}













