#include <stdio.h>

int main()
{
   int t,n;
   long int z,i;
   scanf("%d",&t);
        while(t--){
         int flag=0;
         scanf("%ld %d",&z,&n);
         long int ar[n];
               for(i=0;i<n;i++) {scanf("%ld",&ar[i]);
              // for(i=0;i<n;i++){
                                z=z&ar[i];
               	                if(z==0) flag=1;
               	                       //break;
               	                       // }
                               }
        if(flag == 1) printf("Yes\n");
        else printf("No\n");
        }
    return 0;
}

