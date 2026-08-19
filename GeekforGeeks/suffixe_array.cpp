#include <bits/stdc++.h>
#define INF 0x7fffffff
#define INFLL 1e17
#define PI 2*acos(0.0)
using namespace std;
 
#define FS first
#define SC second
#define PB(t) push_back(t)
#define ALL(t) t.begin(),t.end()
#define MP(x, y) make_pair((x), (y))
#define Fill(a,c) memset(&a, c, sizeof(a))
 
typedef pair<int, int> II;
typedef vector<int> VI;
typedef vector<II> VII;
 
string T;
const int MAX = 5000;
int SA[ MAX ], tempSA[ MAX ];
int RA[ MAX ], tempRA[ MAX ];
int N;
//int c[ MAX ];
 
int Phi[ MAX ];
int PLCP[ MAX ];
int LCP[ MAX ];
 
void countingSort( int k ){
    int i, sum, maxi = max(N*5, N);
    //memset(c, 0, sizeof c);
    map<long long, long long>c;
    map<long long, long long>::iterator it;
 
    for( i=0; i < N; i++){
        c[ (i+k<N)?RA[i+k]:0 ]++;
    }
    it = c.begin();
    for( sum=0; it != c.end(); it++){
        int t = it->second;//c[i];
        c[it->first] = sum;
        sum+=t;
    }
 
    for( i=0; i < N; i++)
        tempSA[ c[ (SA[i]+k<N)?RA[ SA[i]+k ]:0 ]++ ] = SA[i];
 
    for( i=0; i < N; i++)
        SA[i] = tempSA[i];
}
 
void constructSA() {
    int i, k, r;
    for( i=0; i < N; i++){
        RA[i] = T[i];
        SA[i] = i;
    }
 
    for( k=1; k< N; k<<=1 ){
        countingSort( k );
        countingSort( 0 );
 
        tempRA[ SA[0] ] = r = 0;
 
        for( i=1; i < N; i++) {
            tempRA[ SA[i] ] = (RA[ SA[i] ]==RA[SA[i-1]]
                               && RA[ SA[i]+k ]==RA[ SA[i-1]+k ]) ? r: ++r;
        }
 
        for( i=0; i < N; i++) {
            RA[ i ] = tempRA[ i ];
        }
        if( RA[ SA[N-1] ] == N-1)
            break;
    }
}
 
void computeLCP(){
    int i, L;
    Phi[ SA[0] ] = -1;
 
    for( i=1; i < N; i++){
        Phi[ SA[i] ] = SA[ i-1 ];
    }
 
    for( i=L=0; i < N; i++){
        if( Phi[i]==-1 ){
            PLCP[i] = 0;
            continue;
        }
        while( T[i+L]==T[ Phi[i]+L] )
            L++;
 
        PLCP[ i ] = L;
        L = max(L-1, 0);
    }
    for( i=0; i < N; i++) {
        LCP[ i ] = PLCP[ SA[ i ] ];
    }
}
 
int main( ){
#ifndef ONLINE_JUDGE
//   freopen("input.txt", "rt", stdin);
//   freopen("output.txt", "wt", stdout);
#endif
    ios::sync_with_stdio( false );
    cin.tie( NULL );
//
    for( string s1; cin>>s1;  ){
        int n, m;
        n = s1.size();
        T = s1+"$";
        //T.PB(0);
        N = T.size();
        constructSA();
        computeLCP();
        //cout<<T.size()<<":T\n";
        long long res = 0;
        for( int i=0; i < N; i++){
            res+=N-SA[i]-LCP[i]-1;
        }
        cout<<res<<"\n";
        printf("i\tSA[i]\tLCP[i]\tSuffix\n");
        for (int i = 0; i < N; i++){
            printf("%d\t%d\t%d\t%s\n", i, SA[i], LCP[i], T.begin());
        }
 
    }
return 0;
}
