#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const ll N = 1e6 + 5;
const ll MOD = 1e9 + 7;
#define all(x) x.begin(),x.end()
#define pii pair<int,int>
#define pis pair<ll,string>
#define F first
#define S second
#define LCM(a,b) ((a*b)/__gcd(a,b))
#define inf 1e15
#define test ll cse;cin>>cse;for(ll _i=1;_i<=cse;_i++)
#define PI 3.14159265
#define fast ios_base::sync_with_stdio(false);cin.tie(NULL);cout.tie(NULL);
const double EPS = 1E-9;
typedef vector< vector<double> > matrix;
typedef vector< vector<int> > array_2D;
typedef pair<int, pair<int, int> > ppi;
typedef vector<int> vi;
#define MAXN 105 // maximum value of n here

// dp[i] = number of max heaps for i distinct integers
int dp[MAXN];
int nCk[MAXN][MAXN];

// log2[i] = floor of logarithm of base 2 of i
int Log2[MAXN];

// to calculate nCk
int choose(int n, int k){

    if (k >  n) return 0;
    if (n <= 1) return 1;
    if (k == 0) return 1;

    if (nCk[n][k] != -1)
        return nCk[n][k];

    long long answer = (choose(n - 1, k - 1) + choose(n - 1, k)) % MOD;
    nCk[n][k] = answer;
    return answer;
}

// calculate l for give value of n
int getLeft(int n){
    if (n == 1)
        return 0;

    int h = Log2[n];
    // max number of elements that can be present in the hth level of any heap
    int numh = (1 << h); //(2 ^ h)

    // number of elements that are actually present in last level(hth level) (2^h - 1)
    int last = n - ((1 << h) - 1);

    // if more than half-filled
    if (last >= (numh >> 1))
        return numh - 1; // (2^h) - 1
    else
        return numh - 1 - ((numh >> 1) - last);
}

// find maximum number of heaps for n
int numberOfHeaps(int n)
{
    if (n <= 1)
        return 1;

    if (dp[n] != -1)
        return dp[n];

    int left = getLeft(n);

    long long answer = 1LL;
    answer = ( answer * choose(n - 1, left)) % MOD;
    answer = ( answer * numberOfHeaps(left)) % MOD;
    answer = ( answer * numberOfHeaps(n - 1 - left)) % MOD;
    dp[n] = answer;
    return answer;
}

// function to initialize arrays
void init(int n) {
    int currLog2 = -1;
    int currPower2 = 1;

    // for each power of two find logarithm
    for (int i = 1; i <= n; i++) {
        if (currPower2 == i) {
            currLog2++;
            currPower2 = (currPower2 << 1);
        }
        Log2[i] = currLog2;
    }
    for(int i = 0; i <= n; i++) dp[i] = -1;
    for(int i = 0; i <= n; i++) for(int j = 0; j <= n; j++) {
        nCk[i][j] = -1;
    }
}

// driver function
int main()
{
    int n = 100;
    init(n);
    cout << numberOfHeaps(n) << endl;
    return 0;
}