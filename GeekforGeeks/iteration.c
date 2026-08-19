#include<stdio.h>
#include<time.h>
 
int main()
{ clock_t start = clock();
  long double num,first = 0, second = 1, next=0,i;
 
   printf("Enter the nth terms:\n");
   scanf("%Lf",&num);
   for(i=0;i<=num;i++)
   {
      if (i<=1)
         next=i;
      else
      {
         next = first + second;
         first = second;
         second = next;
      }
      if(i==num)
       printf("%.0Lf",next);
   }
   printf("\nTime Elapsed:%f",((double)clock() - start)/CLOCKS_PER_SEC);
   return 0;
}
