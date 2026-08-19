#include <bits/stdc++.h>
using namespace std;


int main(){


    int b; cin>>b;
    int n; cin>>n;
    int tmp;
    vector<int> ar;
    for(int i=0;i<n;i++) { cin>>tmp; ar.push_back(tmp); }


    stack<int> S;

    for(int i=0;i<n;i++){
        if(S.size() >= 2){

            int v2 = S.top();
            S.pop();
            int v1 = S.top();


            if(v1==-1 && v2 == 1 && ar[i]==-1){
                S.pop();
            }
            else{
                S.push(v2);
                S.push(ar[i]);
            }
        }
        else{
            S.push(ar[i]);
        }
    }
    cout << max(0, (int)S.size() - b)<<"\n";
    return 0;
}