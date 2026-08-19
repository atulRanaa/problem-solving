#include <stdio.h>

int main()
{
   long int n,i;
   long int f0=0,s1=0,t2=0;
   scanf("%ld",&n);
   long int ar[n];
   for(i=0;i<n;i++) scanf("%ld",&ar[i]);
      for(i=0;i<n;i+=3){
      	 f0+=ar[i];
      	 if(i+1 < n) s1+=ar[i+1];
      	 if(i+2 < n) t2+=ar[i+2];
      }
    printf("%ld %ld %ld",f0,s1,t2);
    return 0;
}

