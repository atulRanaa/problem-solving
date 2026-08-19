#include<stdio.h>
//using namespace std;
#define lli long long int
 
lli p[10000001];
lli d[10000001];
lli digitSum(lli num){
    lli sum =0,temp =num;
    while(temp > 0){
        sum += temp%10;
        temp /= 10;
    }
    return sum;
}
int main(){
    
    for(lli i=2; i < 10000001;i++) d[i] = i;
    
            for(lli j = 2; j < 10000001; j += 2){
                while(d[j]%2 == 0 && d[j] > 1){
                   p[j] += 2;
                   d[j] /= 2;
                }
            }
    for(lli i=3; i*i < 10000001; i += 2){
        if(p[i] == 0){
            lli sum = digitSum(i);
            for(lli j = i; j < 10000001; j += i){
               while(d[j]%i == 0 && d[j] > 1){
                   p[j] += sum;
                   d[j] /= i;
               }
            }
        }
    }
   /* for(int i=20;i<40;i++) printf("%lld ",d[i]);
    printf("\n");
    for(int i=0;i<20;i++) printf("%lld ",p[i]);
    printf("\n");*/
    for(lli i=2;i < 10000001; i++){
        lli sum = digitSum(i);
        if(d[i] > 1) p[i] += digitSum(d[i]);
        if(p[i] == sum) p[i] = 1+p[i-1];
        else p[i] = p[i-1];
    }
    /*for(int i=0;i<20;i++) printf("%lld ",p[i]);
    printf("\n");*/
    lli l,r,t; scanf("%lld",&t);
   
    while(t--){
        scanf("%lld %lld",&l,&r);
        printf("%lld\n",p[r]-p[l-1]);
    }
}