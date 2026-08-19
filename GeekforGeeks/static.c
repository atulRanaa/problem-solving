#include<stdio.h>

int inc(int i){
	static int count = 0;
	count += i;
	return count;
}

int main(){
	int j=0;
	for(int i=0;i<=4;i++)
		j = inc(i);

	printf("%d\n",j);
}