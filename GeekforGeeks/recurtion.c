#include<stdio.h>
#include<time.h>
long double fib(int n)
{
   if (n <= 1) return n;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  
   return fib(n-1) + fib(n-2);
}
 
int main ()
{ clock_t start = clock();
  int num;
  printf("ENTER THE NTH TERM:");
  scanf("%d",&num);
  printf("%.0Lf", fib(num));
  
 printf("\nTime Elapsed:%f",((double)clock() - start)/CLOCKS_PER_SEC);
 getchar();
  return 0;
} 
