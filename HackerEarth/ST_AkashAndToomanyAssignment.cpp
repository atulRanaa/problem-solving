	#include <bits/stdc++.h>
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
	int ST[4*N][26];
	char s[N];
	void build(int index,int start,int end){
	    if(start==end){
	        ST[index][ s[start]-'a' ] = 1;
	        return;
	    }
	    int mid = (start+end)>>1;
	    build(2*index,start,mid);
	    build(2*index+1,mid+1,end);
	    
	    for(int i=0;i<26;i++)
	    	ST[index][i] = (ST[2*index][i]+ST[2*index+1][i]);
	}
	void update(int index,int start,int end,int x,char ch){
	    if(start == end){
	        ST[index][ s[x]-'a']--;
	        ST[index][ ch  -'a']++;

	        s[x] = ch;
	    }else{
	        int mid = (start+end)/2;
	        if(start <= x && x <= mid)
	        	update(2*index, start,mid,x,ch);
	        else
	        	update(2*index+1,mid+1,end,x,ch);
	        
	        for(int i=0;i<26;i++)
	    		ST[index][i] = (ST[2*index][i]+ST[2*index+1][i]);
	    }
	}
	int cnt[26];
	void query(int index,int start,int end,int l,int r){
	    if(r < start || end < l) return;
	    if(l <= start &&  end <= r){
	    	for(int i=0;i<26;i++)	cnt[i] += ST[index][i];
	    	return;
	    }
	    int mid = (start+end)>>1;
	    query(2*index,start,mid,l,r);	query(2*index+1,mid+1,end,l,r);
	}
	int main(){
	    
	    memset(ST, 0 ,sizeof ST);

	    int n,q; scanf("%d %d",&n,&q);
	    scanf("%s" , s+1);

	    build(1,1,n);
	    int type,l,r,k; char ch;
	    while(q-- > 0){
	        scanf("%d",&type);
	        if(type==1){
	        	scanf("%d %d %d",&l,&r,&k);
	        	memset(cnt, 0, sizeof cnt);

	        	query(1,1,n,l,r);
	        	int Sum=0,i;
	        	for(i=0;i<26;i++){
	        		Sum += cnt[i];
	        		if(Sum >= k)
	        			break;
	        	}
	        	if(Sum >= k) printf("%c \n",(char)(i+'a'));
	        	else printf("Out of range\n");
	        }
	        else{
	        	scanf("%d%c",&l,&ch);
	        	update(1,1,n,l,ch);
	        }
	    }
	    return 0;
	}