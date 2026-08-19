#include<bits/stdc++.h>


using namespace std;

#define ll long long int
int main() {
	
	ll t; cin>>t;
	
	
	while(t-- > 0){
	    ll h,n; cin>>h>>n;
	    
	    ll temp = n;
	    bitset<52> code(n-1);
	    ll l=h;
        
        //cout<< code <<"\n";
        bool dir=0;
        ll ans=0;
        while(l > 0){
            if( (dir^code[l-1]) ) ans += ((1<<l)-1);
            else	
            	l--;

            dir ^= 1;
        }
        
        ans += h;
        
        cout<<ans<<"\n";
    }
	return 0;
}