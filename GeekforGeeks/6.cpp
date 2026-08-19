        #include <bits/stdc++.h>
        #define all(x) x.begin(),x.end()
        #define F first
        #define S second

        using namespace std;
        typedef long long ll;
        typedef vector<int> vi;
        typedef pair<int,int> pii;


const int MAX = 1e5 + 5;
int id[MAX];
int sz[MAX];

// multiset<int> sett;
// vector< vector< pii > > adj(MAX);
// vector< pii > nodes;
// bool visited[MAX];


void initialize(int n){
    sett.clear();
    // adj.clear();
    // nodes.clear(); 
    // memset(visited, false, sizeof visited);
    
    for(int i = 1;i <=n;++i){   
         id[i]=i;
         sz[i]=1; //sett.insert(1);
     }
}
int findset(int x){
    while(id[x] != x){
        id[x] = id[id[x]];
        x = id[x];
    }
    return x;
}
 
void combine(int x, int y){
    int p = findset(x), q = findset(y);
    if(p==q) return;
    if(sz[q] >sz[p]) swap(p,q);
    
        //sett.erase( sett.find(sz[p])); sett.erase( sett.find(sz[q]));
        id[q]=p; sz[p]+=sz[q];

        sz[q]=-1;
        //sett.insert(sz[p]);
}
int continuous1(vector<int> &dp,int n,int k){
    int l=1,r=1,zero=0;
    int ans=0;
    while(r <= n){
        if(r <=n && zero <= k){
            if(dp[r]==0) zero++;
            r++;
        }
        if(l <=n && zero > k){
            if(dp[l]==0) zero--;
            l++;
        }
        ans = max(ans, r-l);
    }
    return ans;
}
int main(){
    
    int t; scanf("%d",&t);
    while(t-- > 0){
        int n,l,k; scanf("%d %d %d",&n,&l,&k);
        
        initialize(n);
        int x,y;
        for(int i=0;i <l;i++){
            scanf("%d %d",&x,&y);

            combine(x,y);
        }
        vector<int> com(n+1,0);
        vector<int> h;
        for(int i=1;i<=n;i++){
            com[i] = findset(i);
            if(id[i]==i) h.push_back(i);
        }

        int ans=0;
        for(int _id: h){

            vector<int> dp(n+1,0);
            for(size_t i=0;i<com.size();i++)
                if( com[i]==_id ) dp[i]=1;

            ans = max(ans, continuous1(dp,n,k));
        }
        cout << ans <<"\n";
    }
    return 0;
}