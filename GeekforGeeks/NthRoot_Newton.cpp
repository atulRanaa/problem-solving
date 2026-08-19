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
 

double mul(double a, double b){
        return a*b;
}
double expo(double x, ll y){ 
    double res = 1.0;               
    for(ll i=0;i<y;i++) res = mul(res,x);
    return res;
}
double newton(ll A,ll p){
        double x=2.0;
        double term = mul(1.0/p, 1.0);
        for(int i=0;i<100;i++){
                x = term*( (p-1.0)*x + ((A*1.0)/expo(x,p)) );
                x = mul(x,1.0);
        }
        return x;
}
int main(){
        fast;
        ll n;
        test{
                cin>>n;
                ll p = log2(n)+1;

                for(ll i=p;i>=1;i--)
                        cout << newton(n,i)<<" "<< i <<"\n";

                cout<<"\n";
        }
        return 0;
} 