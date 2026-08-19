#include <bits/stdc++.h>
using namespace std;

int main()
{
 
    int t;
    cin>>t;
    int a,b,c,k;
             while(t--){
                cin>>a>>b>>c>>k;
                int u = (int)floor(sqrt((float)k/a))+1;
                while(1){
                    long long int f = a*u*u + b*u + c;
                    if(f < k || u<0){
                        cout<<u+1<<"\n";
                        break;
                    }
                    u--;
                }
             }
    return 0;
}
