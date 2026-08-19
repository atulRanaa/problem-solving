#include <bits/stdc++.h>

using namespace std;
#define ll long long
#define mod 1000000007
#define max_sz 200000
#define S second
#define F first



int main()  {
    int q;  scanf("%d",&q);
    
    char r1[10005], r2[1000005];
    for(int c = 0; c<q;c++){

        scanf("%s",r1);
        scanf("%s",r2);

        int l1 = strlen(r1), l2 = strlen(r2);
        int check1[26]={0}, check2[26]={0};

        for(int i=0;i<l1;i++) check1[ r1[i]-'a']++;
        for(int i=0;i<l1;i++) check2[ r2[i]-'a']++;
        
        bool f = true;
        for(int i=0;i<l2;i++){
            if( check1[ r2[i]-'a']==0) f=false;
        }


        if(!f) cout<<-1<<"\n";
        else{
            string r="";   
            for(int i=0;i<l1;i++) if(check2[ r1[i]-'a']!=0) r+=r1[i];

            int l = r.length();
            int ans=1,j=0,k=0;
            while(k < l2){
                if(r[j] == r2[k]){
                    j++; k++;

                    if(k==l2) break;
                }
                else j++;

                if(j==l){
                    ans++; j=0;
                }
            }

            cout<<ans<<"\n";
        }

    }
    return 0;
}
