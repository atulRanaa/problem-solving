#include<bits/stdc++.h>

using namespace std;
#define ll long long int


int A,B;

map< pair<int,int> , bool > m;
bool abbtion(char *a,int i, char *b,int j){
    
    if(i == A && j == B) return true;
    if(i < A && j == B){
        for(int k = i; k < A; k++){
            if(isupper(a[k])) return false;
        }
        return true;
    }
    if(i == A && j < B) return false;
    
    if(m.find({i,j}) != m.end()) return m[{i,j}];
    bool ans;
    if(a[i] == b[j]) ans = abbtion(a,i+1,b,j+1);
    else if(toupper(a[i]) == b[j]) ans = (abbtion(a,i+1,b,j+1) || abbtion(a,i+1,b,j));
    else if(islower(a[i]) && a[i] != b[j]) ans = abbtion(a,i+1,b,j); 
    else ans = false;
    
    m[{i,j}] = ans;
}

int main(){
    int t; cin>>t;
    while(t--){
        m.clear();
        char a[1005],b[1005];
        cin>>a;
        cin>>b;
        A = strlen(a), B = strlen(b);
        string ans = abbtion(a,0,b,0)?"YES":"NO";
        cout<<ans<<"\n";
    }
    return 0 ;
}