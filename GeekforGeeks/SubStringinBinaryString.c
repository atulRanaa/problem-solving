#include<stdio.h>
int main()
{
    int t,i,j,n;
    scanf("%d",&t);
    while(t--){
        scanf("%d",&n);
            int count=0;
            int temp=n;
            while(temp>0){
                if(temp%16 ==15) count++;
                temp=temp/2;
            }
        printf("%d\n",count);
    }
return 0;
}