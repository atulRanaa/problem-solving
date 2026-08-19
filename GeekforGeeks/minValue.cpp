#include<bits/stdc++.h>
using namespace std;

float fun(int x){
	return (float)(((x%6)*(x%6))%7 - sin(x));
}
int main(){
    int i,index;
    float result =-10000,temp = 0;
    for(i = 1;i<=100;i++){
    		temp = fun(i);
    		if( temp > result){result = temp; index =i;}
    }
    printf("%d :%f",index,result);
	return 0;
}