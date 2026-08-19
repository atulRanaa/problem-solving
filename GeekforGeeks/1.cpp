#include <bits/stdc++.h>
using namespace std;
struct d{
    string s;
    int a;
    int b;
    d(string x,int p,int q){
        s = x;  a = p;  b = q;
    }
};
bool comp(d A, d B){
    return  (A.a > B.a) ||
            (A.a==B.a && A.b<B.b) ||
            (A.a==B.a && A.b==B.b && A.s > B.s);
}
int main(){
    int t; scanf("%d",&t);
    
    while(t-- > 0){
        
        int n; scanf("%d",&n);
        string x; int p,q;
        
        vector< d > D;
        for(int i=0;i<n;i++){
            cin>>x>>p>>q;
            D.push_back( {x,p,q} );
        }
        
        int k; scanf("%d",&k);
        sort(D.begin(),D.end(), comp);
        cout<<D[k-1].s<<" "<<D[k-1].a<<"\n";
    }
    return 0;
}