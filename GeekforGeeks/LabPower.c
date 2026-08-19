#include <stdio.h>
#include <math.h>
#define MAX 1000000007
long int prime(long int num,long int m){
	long int i,j,temp,count=0;
	   for(i=num;i<=m;i++){
	   	   temp=sqrt(num);
	   	       for(j=2;j<temp;j++){
	   	       	   if(num%j==0){ count++;break;}
	   	       }
	   }
 printf("%ld",count);
 return count;
}
int main()
{
    long int t,n,m,i,ways=1,value,num;
    scanf("%ld",&t);
         while(t--){
         	scanf("%ld %ld",&n,&m);
         	           for(i=1;i<=n;i++){
         	           	 scanf("%ld",&num);
         	           	 value=prime(num,m);
         	           	 if(value-n+i>0) ways*=(value-n+i);
         	           }
        printf("%ld\n",ways);
         }
    return 0;
}
