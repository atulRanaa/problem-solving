#include <stdio.h>
#include <vector>
using namespace std;

vector<int> factors(int n) {
    vector<int> f;
    if(n%2 == 0){
        f.push_back(2);
        while(n%2 == 0) n /=2;
    }
    if(n%3 == 0){
    	f.push_back(3);
        while(n%3 == 0) n /=3;
    }
    for (int x = 5; x*x <= n; x += 6) {
        if(n%x == 0) {
            f.push_back(x);
            while(n%x == 0) n /= x;
        }
        if(n%(x+2) == 0){
            f.push_back(x+2);
            while(n%(x+2) == 0) n /= (x+2);
        }
    }
    if (n > 1) f.push_back(n);
    return f;
}
int main(){
   
    int t,n; scanf("%d",&t);
    while(t--){
        scanf("%d",&n);
        int ans = 1e6;
        vector<int> p = factors(n);
        for(int i=1;i < p.size() ;i++) ans = min(ans,p[i]-p[i-1]);
        printf("%d\n",(ans == 1e6)?-1LL:ans);
    }
    return 0;
}
