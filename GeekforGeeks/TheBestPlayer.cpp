#include <bits/stdc++.h>
using namespace std;

vector< pair< long int ,string > > v;
vector< pair< long int ,string > >::iterator it;
int main()
{
    int n,t;
    long int f;
    string str;
    cin>>n>>t;
    for(int i=0;i<n;i++){
        cin>>str>>f;
        v.push_back(make_pair(f,str));
    }
    
    sort(v.begin() ,v.end());
    for(int i=0;i<n;i++){
        cout<<v[i].first<<v[i].second<<endl;
    }
    return 0;
}
