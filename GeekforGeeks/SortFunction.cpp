#include<iostream>
#include<algorithm>
using namespace std;

int main()
{  
  int array[9]={2334,4,536,5,7,3,23,12,-34};
  sort(array,array+9);
     for(int i=0;i<9;i++) cout<<array[i]<<" ";
   
 return 0;
}
