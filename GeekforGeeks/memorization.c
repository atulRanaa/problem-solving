#include<stdio.h>
#include<time.h>
 
long double fib(int n)
{
 static long double f[500];
 static int i=2;
  f[0] = 0;
  f[1] = 1;
  f[2] = 1;
  if(n==i)
   return f[i];
  else
   {  for(;i<=n;i++)
        f[i]=f[i-1]+f[i-2];
   return f[n];
   }
}
 
int main ()
{  clock_t start = clock();
  int num;
  printf("ENTER NTH TERM:");
  scanf("%d",&num);  
  printf("%.0Lf", fib(num));
  printf("\nTime Elapsed:%f",((double)clock() - start)/CLOCKS_PER_SEC);
  
  return 0;
}
