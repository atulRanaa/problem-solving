#include<stdio.h>

int compare(int a,int b){
   if(a > b) return 1;
   else return -1;
}
int swap(int *a,int *b){
   int temp;
   temp = *a;
   *a = *b;
   *b = temp;
}
int main(){
 int t;
 scanf("%d",&t);
 while(t--){
   int n;
   scanf("%d",&n);
   int ar[n];
   int i=0;
   for(i=0;i<n;i++) scanf("%d",&ar[i]);
   int comp=0 , swp =0 ;
   for(i=1;i <= n;i++){
     int j=i-1;
     while( j > 0 ){
       comp++;
       if( compare(ar[j-1],ar[j]) > 0){
         swp++;
         swap(&ar[j],&ar[j-1]);
         j = j-1; 
       }
       else break;
     }
   }
   printf("%d\n",comp-swp);
 }
return 0;
}

