#include<stdio.h>

void printSpiral(int *arr[],int n)
{
int i,j,k;
for(i=n-1, j=0; i>0; i--, j++){
    for(k=j; k<i; k++) printf("%d ", arr[j][k]);
    for(k=j; k<i; k++) printf("%d ",arr[k][i]);
    for(k=i; k>j; k--) printf("%d ",arr[i][k]);
    for(k=i; k>j; k--) printf("%d ",arr[k][j]);

}
int middle = (n-1)/2;
if (n% 2 == 1) printf("%d", arr[middle][middle]);
}
int main(){
  int size,i,j;
  scanf("%d",&size);
int ar[size][size];
  for(i=0;i<size;i++){
     for(j=0;j<size;j++) scanf("%d ",&ar[i][j]);
  }
  printSpiral(ar,size);
  return 0;
}
