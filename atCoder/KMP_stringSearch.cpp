#include <bits/stdc++.h>
using namespace std;

vector<int> prefixtable(string Z) {

    int n = (int) Z.length();
    vector<int> F (n);
    F[0]=0;
    for (int i=1; i<n; ++i) {
        int j = F[i-1];
        while (j > 0 && Z[i] != Z[j])
            j = F[j-1];

        if (Z[i] == Z[j])  ++j;
        F[i] = j;
    }
    return F;
}
int kmp(const string &p,const string &t){
    int i=0,j=0,n,m,c=0;
    n=t.length();
    m=p.length();
    vector< int > F = prefixtable(p);
    
    for(int i=0;i<F.size();i++) cout<<F[i]<<" ";
    cout<<"\n";
    vector< int > E = prefixtable(t);
    for(int i=0;i<E.size();i++) cout<<E[i]<<" ";
    cout<<"\n";
    

    while(i<n){
        if(t[i]==p[j]){
            if(j==m-1){ j-=1;c++;}
            else {
                i++;
                j++;
            }
        }
        else if(j>0) j=F[j-1];
        else i++;
    }
    return c;
}
int main()
{
    string p,t;

    while(cin>>p){
    vector< int > F = prefixtable(p);
    for(int i=0;i<F.size();i++) cout<<F[i]<<" ";
    cout<<"\n";
    }
    
    // cin>>p;
    // cin>>t;
    // cout<<kmp(p,t);
    return 0;
}