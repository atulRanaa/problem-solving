    #include <bits/stdc++.h>
    using namespace std;
    #define pb push_back
    
    const int N = 3e5 + 5;
       
    int ar[N],tin[N],tout[N],t;
    int BIT[N];
    vector< vector<int> > graph(N);
     
int get(int ind){ 
    int sum = 0;
    for(;ind > 0; ind -= ind & (-ind))  sum += BIT[ind];
    return sum;
}
void updateBIT(int n, int ind,int val){ 
    for(;ind <= n;ind += ind & (-ind))  BIT[ind] += val;
    //for(int i=1;i<=n;i++) cout<<BIT[i]<<" "; cout<<"\n";
}
     
     bool isprime(int n){
          if(n <= 1) return false;
          if(n <= 3) return true;
          if(n%2 == 0 || n%3 == 0) return false;

          for(int i=5;i*i <= n; i+=6)
              if(n%i==0 || n%(i+2)==0) return false;
          return true;
     }
     void dfs(int i, int p)
     {
        tin[i] = ++t;
        for(int j : graph[i]) if(j != p) dfs(j, i);
        tout[i] = t;
    }
    void init(){
        for(int i=0;i<N;i++) graph[i].clear();
        memset(tin, 0 , sizeof tin);
        memset(tout, 0 , sizeof tout);
        memset(BIT, 0 , sizeof BIT);
        t=0;
    }
    int main(){
      
      int cs; scanf("%d",&cs);
      while(cs-- > 0){
        init();
        int i,x,y,n,q;
        scanf("%d%d",&n,&q);
        
        for(int i=1;i<=n;i++) scanf("%d", ar+i);
        
        for(i=1;i<n;i++){
            scanf("%d %d",&x,&y);
            graph[x].pb(y);
            graph[y].pb(x);
        }
     
        dfs(1,-1);
        for(int i=1;i<=n;i++)
            if(isprime(ar[i])) updateBIT(N-5, tin[i],1);

        while(q-- > 0)
        {
            int type;
            scanf("%d",&type);
            if(type==2){
                scanf("%d",&x);
                 int ans=get(tout[x]);
                 
                 //printf("%intd\n", query(tout[x])-query(tin[x]-1));
                if(tin[x]>1)
                     ans-=get(tin[x]-1);
                printf("%d\n",ans);
            }
            else{
                scanf("%d%d",&x,&y);
                if(isprime(ar[x]))
                        updateBIT(N-5,tin[x],-1);
                ar[x]=y;

                if(isprime(ar[x]))
                        updateBIT(N-5,tin[x],1);
            }
        }
        //cout<<"t"<<t<<"\n";
      }
      return 0;
    }