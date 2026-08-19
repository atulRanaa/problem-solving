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
    int BIT[N][26];
    char s[N];

int cnt[26];
void getSum(int l, int r){ 
    while(r > 0){   
        for(int i=0;i<26;i++) cnt[i] += BIT[r][i]; 
        r -= r&(-r);
    }
    while(l > 0){
        for(int i=0;i<26;i++) cnt[i] -= BIT[l][i];
        l -= l&(-l);
    }
}
 
void updateBIT(int n, int ind, char ch, bool f){ 
    int i = ind;
    while (ind <= n){
        if(f)
            BIT[ind][ s[i]-'a'] -= 1;
        BIT[ind][ ch-'a'] += 1;  ind += ind & (-ind);
    }
    s[i] = ch;
}
void build(int n)
{   
    for(int i=1; i<=n; i++)
        updateBIT(n, i, s[i], false);
}
 
int main()
{
    memset(BIT, 0 ,sizeof BIT);

    int n,q; scanf("%d %d",&n,&q);
    scanf("%s" , s+1);
    cout<<n<<" "<<q<<" "<<(s+1)<<"\n";
    build(n);
    
    /*for(int i=0;i<=n;i++){
        for(int j=0;j<26;j++) cout<<BIT[i][j]<<" ";
        cout<<"\n";
    }*/
    int type,l,r,k; char ch;
    while(q-- > 0){
        scanf("%d",&type);
        if(type==1){
            scanf("%d %d %d",&l,&r,&k);
            memset(cnt, 0, sizeof cnt);

            getSum(l-1,r);
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
            scanf("%d %c",&l,&ch);
            updateBIT(n,l,ch,true);
        }
    }
    return 0;
}