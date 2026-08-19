#include<bits/stdc++.h>


using namespace std;

#define ll long long int
#define MAXN 1001


//int phi[MAXN + 1];
int prime[MAXN];

bitset <MAXN + 1> mark;
int size=0;
void sieve(){
    for (int i = 2; i <= MAXN; i++ ){
        if(!mark[i]){
            //phi[i] = i-1;
            prime[size++]=i; cout << i <<" ";
        }
        for (int j=0; j<size && prime[j]*i <= MAXN; j++ ){
            mark[prime[j]*i]=1;
            if(i%prime[j]==0){
                //phi[i*prime[j]] = phi[i]*prime[j];
                break;
            }
            //else phi[i*prime[j]] = phi[i]*(prime[j]-1 );
        }
    }
}
int N = 10000;
int main() {
	
	ll n; cin>>n;
    ll ar[n+1]; for(int i=1;i<=n;i++) cin>>ar[i];

    sieve();
    

    int h[size][N];
    memset(h, 0, sizeof(h));

    for(int i=0;i<size;i++) for(int j=1;j<=n;j++){
        int cnt=0;
        while(ar[j] % prime[i] == 0){
            cnt++;
            ar[j] /= prime[i];
        }

        h[i][j] = cnt;
    }
    for(int i=0;i<size;i++) for(int j=1;j<=n;j++){
        h[i][j] += h[i][j-1];
    }



    cout << size <<"\n";
	return 0;
}