#include "stdio.h"
int main(){

	int num = 1,i;
	char *c = (char *)&num;
	if(c ) printf("Little Endian\n"); // left to right
	else printf("Big Endian\n");      // right to left
	
	for(i = 0; i < sizeof(num); i++) printf("%x ",c[i]);

	num = 0x01234567;
	c = (char *)&num;
	for(i = 0; i < sizeof(num); i++) printf("%x ",c[i]);
}
