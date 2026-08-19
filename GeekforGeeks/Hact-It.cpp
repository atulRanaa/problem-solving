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
 
int main(){
        fast;
        
        test{
            string s; cin>>s;
            int l=s.length();
            
            
            char ans[l+1];
            
            ans[l]='\0';
            int i = l-1;
            
            if(l&1) i-=1; 
            int j=0;
            for(;i>=0 && j<l;i-=2,j++) ans[i]=s[j];
            
            i = l-1;
            if(!(l&1)) i-=1;
            for(;i>=0 && j<l;i-=2,j++) ans[i]=s[j];
            
            cout << ans <<"\n";
        }
        return 0;
} 