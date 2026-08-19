#include<stdio.h>
const char* isLeapYear(int year)
{ 
    if(year%100==0){
        if(year%400==0){
            return "Leap year";
        }
    }
    else if(year%4==0){
     return "Leap year";
    } 
    else{ 
      return "Not a leap year";
    }
}
int main(){
  int year;
  scanf("%d",&year);
const char *str=isLeapYear(year);  
printf("%s",str);
 return 0;
}
