#include<bits/stdc++.h>
using namespace std;


void rmDublicate(int *ar,int size){
   int i,j;
       for(i=0;i<size;i++){
           for(j=i+1;j<size;j++){
               if(ar[i]==ar[j]) ar[i]=-1;
           }
       }
       for(i=0;i<size;i++){
           if(ar[i]==-1){
                 j=i;
                 while(ar[i]==-1){
                    ar[i]=ar[++j];
                 }
                 i=j;
           }
       }
}

int main(){
    int size,i;
             cout<<"enter the size of array:\n";
             cin>>size; 
    int ar[size];
 
      for(i=0;i<size;i++)
      cin>>ar[i];
     
      cout<<endl;
      rmDublicate(ar,size);
      
      for(i=0;i<size;i++)
      cout<<ar[i]<<" ";

return 0;
}
