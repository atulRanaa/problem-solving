#include<bits/stdc++.h>
using namespace std;

int main(){
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);

	long long int n,i,t3;
	int t;
	scanf("%d",&t);
	int flag,t1,t2;
	    for(long long int j=1;j<=t;j++){

	    	scanf("%lld",&n);
        
            for(i=n;i>=0;i--){

            	flag=1;
                if(i<=9) break;
                else{
                	t3=i;
	    	        t1=t3%10;
                    t3=t3/10;

	        	    while(t3){ 
	    		      t2 = t3%10;
	    		      if(t2 > t1){flag=0;break;}

                	  t3=t3/10;
                	  t1=t2;
	         	    }
     	        }
     	        if(flag==1) break;

            }
            
            printf("Case #%lld: %lld\n",j,i);
	    }
	return 0;
}