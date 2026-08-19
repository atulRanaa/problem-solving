        #include <bits/stdc++.h>
             
        using namespace std;
         
        typedef long long ll; 
        const int N = 1e6+5; 
        #define MOD 1000000007
        #define pii pair<int,int>
        #define pis pair<int,string>
        #define F first
        #define S second
        #define LCM(a,b) ((a*b)/__gcd(a,b))
        #define inf 1e15
        ll ar[N];
        ll lvl[50];
        
        void sink(int k,int n){
            while((2*k) <= n){            
                int j = (2*k);
                /* max heap
                if(j < n && ar[j]<ar[j+1])  j++;
                if(ar[k]>=ar[j])            break;
                */
                if(j < n && ar[j]>ar[j+1])  j++;
                if(ar[k]<=ar[j])            break;
                swap(ar[k], ar[j]);
                k = j;
            }
        }
        void swim(int k){
            /* max heap
            while(k > 1 && ar[(k/2)] < ar[k]){
                swap(ar[(k/2)] , ar[k]);    k/=2;
            }
            */
            while(k > 1 && ar[(k/2)] > ar[k]){
                swap(ar[(k/2)] , ar[k]);    k/=2;
            }
        }
        void HeapSort(int n){
            for(int k=(n/2); k>=1;k--)  sink(k,n);
            
            while(n>1){
                swap(ar[1],ar[n--]);    sink(1,n);
            }
            
        }
        int main(){
            ios_base::sync_with_stdio(false);
            cin.tie(NULL);
            
            
            int n; cin>>n;
            for(int i=1;i<=n;i++)   cin>>ar[i];

            HeapSort(n);
                    for(int i=1;i<=n;i++)   cout<<ar[i]<<" "; cout<<"\n";
            return 0;
        }
