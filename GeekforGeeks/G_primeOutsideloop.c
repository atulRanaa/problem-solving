#include<stdio.h>
#include<math.h>
#include<time.h>

int main(){
          clock_t begin = clock();


long int num;
printf("ENTER NUMBER UPTO WHICH NEED TO GENERATE PRIME NUMBERS:");
scanf("%ld",&num);

long int i,j,temp=0;

int flag=0;

for(i=1;i<num;i++)
{
 temp=sqrt(i);
 flag=0;
 for(j=2;j<=temp;j++)
 { if(i%j==0)
    flag=1;break;}

  if(flag==0) 
  printf("%ld ",i);
}

clock_t end = clock();
double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
printf("Elapsed: %f seconds\n",time_spent);
return 0;
}
