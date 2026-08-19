        #include <bits/stdc++.h>
             
        using namespace std;
         
        typedef long long ll; 
        const int N = 1e5+5;
        #define all(x) x.begin(),x.end()
        #define MOD 1000000007
        #define pii pair<int,int>
        #define pis pair<int,string>
        #define F first
        #define S second
        #define LCM(a,b) ((a*b)/__gcd(a,b))
        #define inf 1e15
        #define test ll cse;cin>>cse;for(ll I=1;I<=cse;I++)
        #define fast ios_base::sync_with_stdio(0);cin.tie(NULL);cout.tie(NULL);
        #define PI 3.14159265
        #define MAXN 1300005
        #define pb      push_back
        #define mp      make_pair
void fib(ll n, ll&x, ll&y){
    if(n==0){
        x = 0;
        y = 1;
        return ;
    }
     
    if(n&1){
        fib(n-1, y, x);
        y=(y+x);
        if(y>=MOD)
                y-=MOD;
    }else{
        ll a, b;
        fib(n>>1, a, b);
        y = (a*a+b*b)%MOD;
        x = (a*b + a*(b-a+MOD))%MOD;
    }
}
int main(){
        ll x,y;
        for(int i=0;i<=10;i++){fib(i,x,y); cout<<x<<" "<<y<<"\n";}
        return 0;
} 