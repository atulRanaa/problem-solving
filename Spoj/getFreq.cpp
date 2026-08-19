#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
using namespace std;    

void getFreq(int arr[],long int size)
    {     long int i;
       for(i=0;i<size;++i)
              arr[(arr[i]-1)%(size+1)] += size+1;
       for(i=0;i<size;++i)
              cout<<"\nFrequency of "<<i+1<<" is : "<<arr[i]/(size+1);
    }
int main(){         srand(time(NULL));
long int size,i;
cout<<"enter the size of array"<<endl;
           cin>>size;
int arr[size];
 for(i=0;i<size;i++) cin>>arr[i];             //arr[i]=rand()%1000; 
      
   cout<<"here we go";
   getFreq(arr,size);
return 0;
}
