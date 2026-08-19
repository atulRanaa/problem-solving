#include<iostream>
#include<stdio.h>
#include<math.h>

using namespace std;

long int fact(int num){
if(num<=1)
return 1;
else
return num*fact(num-1);
}
int main(){

for(int i=100 ;i<120;i++){
printf("%.2f\t%d\t%.2f\t%.2f\t%.2f\t%d\t%.2f\t%.2f\t%.2f\t%d\t%ld\n",log(i),i,i*log(i),pow(2,log(i)),i*i,pow(i,2),i*pow(2,i),pow(2,i),i*i*i,fact(i),pow(i,i));
}

return 0;
}
