#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;
void mostFreq3(int *a, int n, int b[3]) {

   int i, key, j;
   for (i = 1; i < n; i++)
   {
       key = a[i];
       j = i-1;
       while (j >= 0 && a[j] > key)
       {
           a[j+1] = a[j];
           j = j-1;
       }
       a[j+1] = key;
   }
  i=0;
  b[0]=0;
  b[1]=0;
  b[2]=0;
  int ar[3]={0};
  int count,temp;

  i=0;
  while(i < n){
    count =0;
    temp = a[i];
    while(a[i]==temp && i<n) {count++;i++;}
    if(count >= ar[0]){
       ar[2]=ar[1];
       ar[1]=ar[0];
       ar[0]=count;

       b[2]=b[1];
       b[1]=b[0];
       b[0]=a[i-1];
    }
    else if(count >= ar[1]){
       ar[2]=ar[1];
       ar[1]=count;
       
       b[2]=b[1];
       b[1]=a[i-1];
    }
    else if(count >= ar[2]){
       ar[2]=count;
       b[2]=a[i-1];
    }
 //   cout<<a[i-1]<<" "<<count<<endl;
  }
}
int main()
{
	int n;
	int a[100];
	int b[3];
	int i=0;

	cin >> n;
	for(i=0;i<n;i++){
		cin >> a[i];
	}

	mostFreq3(a,n,b);
        for(i=0; i<3; i++)
            cout << b[i] << " ";

	return 0;
}
