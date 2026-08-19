#include<bits/stdc++.h>
using namespace std;

int main(){

    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    
    int t,k,len;
    char str[1010];


    scanf("%d",&t);

    for(int i=1;i<=t;i++){
        scanf("%s",str);
        scanf("%d",&k);
        
        len = strlen(str);
        int count=0,flag=0;

        for(int j=0;j<len;j++){
        	if(str[j]=='-' && j+k <= len){
        		
        		for(int a=j; a<j+k ;a++){
                   if(str[a]=='-') str[a]='+';
                   else  str[a]='-'; 
        		}
        		count++;
        	}
        }

        for(int j=0;j<len;j++){
        	if(str[j]=='-'){flag=1;break;}
        }

        if(flag==0) printf("Case #%d: %d\n",i,count);
        else printf("Case #%d: IMPOSSIBLE\n",i);
    }
	return 0;
}
