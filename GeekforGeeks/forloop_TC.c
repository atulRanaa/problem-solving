//Program to check....which for loop give better performance.
#include<stdio.h>

int main(){

int i,j;
long int count=0,count1=0;

for(i=0,++count;i<100 && ++count;i++,++count)
 for(j=0,++count;j<10 && ++count;j++,++count);



for(i=0,++count1;i<10 && ++count1;i++,++count1)
 for(j=0,++count1;j<100 && ++count1;j++,++count1);



printf("%ld %ld",count,count1);
}
