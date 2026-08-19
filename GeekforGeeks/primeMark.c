#include <stdio.h>
#include <math.h>
int main()
{   long int i,j;
    int prime[1000010]={0};
    prime[0]=1;
    prime[1]=1;
    long int len=sqrt(1000010);
    
    for(i=2;i<=len;i++){
        if(prime[i]==0){
            for(j=2*i;j<1000001;j+=i){
                prime[j]=1;
            }
        }
    }
    for(i=0;i<21;i++) printf("%ld ",prime[i]);
 return 0;
}
