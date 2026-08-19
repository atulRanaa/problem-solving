
// hackerrank incom
#include <iostream>
#include <vector>
using namespace std;

#define N 1000005
#define ll long long

ll solve(ll x, ll l, ll r) {
    vector<int> z;
    for(int i=30;i >=0;i--){
        if( !(x & (1<<i)) ) z.push_back(i);
    }
    // for(int i=30;i>=0;i--){
    //     if( x & (1<<i)) cout<<1; else cout<<0;
    // } cout<<"\n";
    ll num=0, ans=0, temp =0;
    
    for(int e: z){
        temp = num | (1<<e);
        //cout<< temp <<" ";
        if( temp <= r ){
            num = temp;

            /*for(int i=30;i>=0;i--){
                if( num & (1<<i)) cout<<1; else cout<<0;
            } cout<<"\n";*/
        }
    }
    

     
    ans = max(num^x, x^l);
    ans = max(ans, x^r);

    // ll y=0,n;
    // for(int i=l;i<=r;i++){
    //     if((x^i) > y){ y = (x^i); n=i; }
    // }
    // for(int i=30;i>=0;i--){
    //     if( n & (1<<i)) cout<<1; else cout<<0;
    // } cout<<"\n";
    //cout<<"actual "<<n<<" "<<y<<" ";
    return ans;
}
int main(){
    
    int n;  cin>>n;
    int x,l,r;
    while(n--){
        cin>>x>>l>>r;
        cout << solve(x,l,r) <<"\n";
    }
    return 0;
}
