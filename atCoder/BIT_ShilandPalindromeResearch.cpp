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
	int main(){
	    
	    memset(BIT, 0 ,sizeof BIT);
	    int n,q;  scanf("%d%d",&n,&q);
	    scanf("%s" , s+1);
	    
	    build(n);
	    
	    int type,k,l,r; char ch;
	    while(q-- > 0){

	    	scanf("%d",&type);
	    	if(type ==1){
	    		scanf("%d %c",&k,&ch);
	        	
	        	updateBIT(n,k, s[k]-'a',-1);
	        	updateBIT(n,k, ch  -'a',1);
	        	s[k] = ch;
	    	}
	    	else{
	    		scanf("%d %d",&l,&r);
	    		int count=0, temp;
	    		for(int i=0;i<26;i++){
	    			temp = get(r,i) - get(l-1,i);
	    			if(temp&1LL) count++;
	    		}

	    		printf("%s\n",(count<=1)?("yes"):("no"));
	    	}
	        
	    }
	    //for(int i=1;i<=n;i++) if(s[i]!='+') printf("%c", s[i]);

	    return 0;
	}