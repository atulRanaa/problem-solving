#include<stdio.h>
long int count1=0,count2=0;
void swap(long int *a,long int *b){ 
  long int temp = *a; 
   *a = *b; 
   *b = temp; 
} 
int partition1(long int ar[],long int low,long int high){
 long int i,j,x;
 x=ar[low];
 i=high+1;
  
 for(j=high;j>low;j--){
   if(ar[j]>x){
    i--;
    if(j!=i){ swap(&ar[i],&ar[j]);
               count1++;
             }
   }
 }
  if(ar[i-1]!=ar[low]){ swap(&ar[i-1],&ar[low]);
                        count1++;
                      }

 return i-1;
}
void quickSort1(long int ar[],long int low,long int high) 
{   long int q; 
    if (low < high) 
    {   q=partition1(ar,low,high); 
        quickSort1(ar,low,q-1); 
        quickSort1(ar,q+1,high); 
    } 
} 
int partition2(long int ar[],long int low,long int high) 
{  long int i,j,x; 
    x=ar[high];    
    i=(low-1);  
 
    for(j=low;j<high;j++) 
    {  if (ar[j]<x) 
        {   i++;    
            if(j!=i) {swap(&ar[i], &ar[j]); count2++;}
        } 
    } 
    if(ar[i+1]!=ar[high]) {swap(&ar[i+1],&ar[high]); count2++;} 
    return (i+1); 
} 
void quickSort2(long int ar[],long int low,long int high) 
{   long int q; 
    if (low < high) 
    {   q=partition2(ar,low,high); 
        quickSort2(ar,low,q-1); 
        quickSort2(ar,q+1,high); 
    } 
} 
int main()
{
    int t;
    scanf("%d",&t);
       while(t--){
           long int n;
            scanf("%ld",&n);

           long int i,ar1[n],ar2[n];
  
             for(i=0;i<n;i++) {scanf("%ld",&ar1[i]);
                               ar2[i]=ar1[i];}
   quickSort1(ar1,0,n-1);
   quickSort2(ar2,0,n-1);
   printf("%ld %ld\n",count1,count2); 
     count1=0;count2=0;
       }
    return 0;
}

