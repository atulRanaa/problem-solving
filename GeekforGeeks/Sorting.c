#include<stdio.h>
#include<stdlib.h>
#include<time.h>

void swap(long int *a,long int *b){
   long int temp;
     temp=*a;
     *a=*b;
     *b=temp;
}

void selectionSort(long int *arr,long int size){
long int i,j,min=0;
long long int count=0,compareC=0;
long int index=0;

    for(i=0;i<size;i++)
     {  min=arr[i];index=0;
            for(j=i+1;j<size;j++)
             {   compareC++;
                if(arr[j]<min) 
                 {min=arr[j];index=j;}
             }
       if(index!=0) 
       {swap(&arr[i],&arr[index]);count++;}                  
     }
  printf("selection sort\tCompareCount:%lld\tSwapCount:%lld\n",compareC,count);
}


void bubbleSort(long int *arr,long int size){
long int i,j;
long long int count=0,compareC=0;
int flag=0;

    for(i=0;i<size;i++)
     {  
            for(j=0;j<size-1-i;j++)
             {  compareC++;
                if(arr[j]>arr[j+1]) 
                 {swap(&arr[j],&arr[j+1]);flag=1;count++;}
             }
       if(flag==0) break;                  
     }
   printf("bubble sort\tCompareCount:%lld\tSwapCount:%lld\n",compareC,count);
}

void insertionSort(long int arr[],long int size){
long int i,j,temp;
long long int count=0,compareC=0;
    for(i=0;i<size;i++)
     {  temp=arr[i];
        j=i-1;
        compareC++; 
            while(j>=0 && arr[j]>temp)
            { arr[j+1]=arr[j];
              j--;
              count++;
            }
        arr[j+1]=temp;                 
     }
  printf("insertion sort\tCompareCount:%lld\tSwapCount:%lld\n",compareC,count);
}


int main(){
   long int size,i;
   printf("enter the size of array:\n"); scanf("%ld",&size);
   long int ar[size];
   
   for(i=0;i<size;i++) ar[i]=rand()%100;
    double start1=clock();
    //selectionSort(ar,size);              
  /*array can not passed as call by value in functions so to compare algorithm with same large
                        value dont use srand(time()) as due to this every time rand() function will generate random 
                        value which is differ from last generated value in our case generated values are always same
  for given range.so use function one by one*/
   
     bubbleSort(ar,size);
   //insertionSort(ar,size);
    printf("time elapsed:%f\n",(double)(clock()-start1)/CLOCKS_PER_SEC);
   
   return 0;
}
