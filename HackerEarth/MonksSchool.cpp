#include <bits/stdc++.h>
using namespace std;

map< string , vector< pair<int,string> > > m;
vector< pair<int,string> >::iterator it;
vector< string > v;
int main()
{
   // cout << "Hello World!" << endl;
    int n,ma,age;
    string name,tname;
    cin>>n>>ma;
    for(int i=0;i<n;i++){
        cin>>tname;
        v.push_back(tname);
    }
    sort(v.begin(),v.end());
    for(int i=0;i<ma;i++){
        cin>>tname>>name>>age;
        m[tname].second.push_back(make_pair(1,"name"));
    
    //   cout<<m[tname].first<<" "<<m[tname].second<<endl;
    }
    
    for(int i=0;i<n;i++){
        cout<<v[i]<<endl;
        sort(m[v[i]].begin(),m[v[i]].end());
        for(it=m[v[i]].begin();it!=m[v[i]].end();it++){
            cout<<it->second<<" "<<it->first<<endl;
        }
    }
    return 0;
}
