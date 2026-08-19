#include<bits/stdc++.h>
using namespace std;

int main(){
    ios_base::sync_with_stdio(false);cin.tie(0);
    int l,i;	cin>>l;
    string s,p;
    map<char,int>m;
    map<char,int>::reverse_iterator rit;
    cin>>s;
    for(i=0;i<l;i++)	m[s[i]]++;
    
    rit=m.rbegin();
    p="";
    for(i=0;i<l;i++)
    {
        if(s[i]==rit->first)
        {  
            p=p+s[i];
            m.erase(s[i]);         
        }
        else {
            m[s[i]]--;
            if(m[s[i]]==0)	m.erase(s[i]);
        }
    }
    cout<<p;
    return 0;
}
