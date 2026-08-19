#include<stdio.h>
#include<math.h>
long long  unsigned int fibo(int n)
{
  if(n==1) return 0;
 else if(n==2 || n==3) return 1;
 else return(fibo(n-2)+(fibo(n-1)*fibo(n-1)));
}
int main()
{
  int i,n;  
  printf("\nEnter the number of fibonacci numbers\n");
  scanf("%llu",&n);
  long long unsigned int a[n];
    for(i=1;i<=n;i++) a[i-1]=fibo(i);
    
  for(i=0;i<n;i++) printf("%llu ",a[i]);
  return 0;  
}
