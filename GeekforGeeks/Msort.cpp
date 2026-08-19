#include<iostream>
#include<stdio.h>
#define MAX 100
using namespace std;
/*....................................................*/
void Merge(int ar[],long int p,long int q,long int r){
   long int i=p,j=q,k=0;
   int copyarray[r-p];
      while(i<q && j<r){
          if(ar[i] <= ar[j]) copyarray[k++]=ar[i++];
          else copyarray[k++]=ar[j++]; 
      }
    
     if(i<q){
              while(i<q) {
                 copyarray[k++]=ar[i++];
                         }
            }
     if(j<r){
              while(j<r) {
                copyarray[k++]=ar[j++];
                         }
            }
     
     for(int n=p;n<r;n++) {
            ar[n]=copyarray[n];
                          }
}
/*....................................................*/

/*....................................................*/
void mergeSort(int ar[],long int p,long int r){
  cout<<".";
  long int q;
  if(p<r){
         q=((p+r)/2);
         }
  mergeSort(ar,p,q); 
        for(int i=p;i<q;i++) 
            cout<<ar[i]<<" ";
  cout<<endl;
  mergeSort(ar,q+1,r);
 
        for(int i=q+1;i<r;i++) 
            cout<<ar[i]<<" ";

  Merge(ar,p,q,r);
}
/*....................................................*/

int main(){
 long int n,i;
 int ar[MAX];
 cout<<"enter the size of array:\n";
 cin>>n;

       for(i=0;i<n;i++) {
          cin>>ar[i];
       } //ar[i]=rand()%1000;
       cout<<"main()  .......................\n ";
 mergeSort(ar,0,n);
       for(i=0;i<n;i++) 
         cout<<ar[i]<<" ";
return 0;
}
