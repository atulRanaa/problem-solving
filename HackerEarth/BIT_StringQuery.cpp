    #include <cstring>
    #include <stdio.h>
    #include <string>
    #include <iostream>
    using namespace std;

	const int N = 1e6+5;
	#define ll long long
	#define MOD 1000000007
	#define pii pair<int,int>
	#define pis pair<int,string>
	#define F first
	#define S second
	#define LCM(a,b) ((a*b)/__gcd(a,b))
	#define inf 1e15
	
	ll BIT[N][26];
    char s[N];

ll get(int ind,int i){ 
    ll sum = 0;
    for(;ind > 0; ind -= ind & (-ind))  sum += BIT[ind][i];
    return sum;
}
 
void updateBIT(int n, int ind,int i, int val){ 
    for(;ind <= n;ind += ind & (-ind))  BIT[ind][i] += val;
    //for(int i=1;i<=n;i++) cout<<BIT[i]<<" "; cout<<"\n";
}
void build(int n)
{   
    for (int i=1; i<=n; i++){
        updateBIT(n,i,s[i]-'a',1);
    }
}
int bs(int n,int k,int ch){
	int l=1,mid,r=n,check, ans;

	while(l <= r){
		mid = (l+r)>>1;
		check = get(mid, ch);

		if(check >= k){
			ans = mid, r = mid-1;
		}
		else
			l = mid+1;
	}
	return ans;
}
	int main(){
	    
	    memset(BIT, 0 ,sizeof BIT);

	    scanf("%s" , s+1);
	    
	    int n = strlen(s+1);
	    //cout<<n<<"\n";
	    build(n);
	    int q;  scanf("%d",&q);
	    int k; char ch;
	    while(q-- > 0){
	        scanf("%d %c",&k,&ch);
	        int ind = bs(n,k,ch-'a');

	        updateBIT(n,ind, ch-'a',-1);

	        s[ind] = '+';
	    }
	    for(int i=1;i<=n;i++) if(s[i]!='+') printf("%c", s[i]);

	    return 0;
	}