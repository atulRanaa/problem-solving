#include<stdio.h>
bool isPrime(int n)
{
    if (n <= 1)  return false;
    if (n <= 3)  return true;
    if (n%2 == 0 || n%3 == 0) return false;
 
    for (int i=5; i*i<=n; i=i+6)
        if (n%i == 0 || n%(i+2) == 0) return false;
 
    return true;
}
int main(){
    freopen("input.txt","r",stdin);
    freopen("output.txt","w",stdout);
    for(long long int i=49280000;i <= 98562124-i ; i++){
        if(isPrime(i) && isPrime(98562124-i))
            printf("%lld %lld\n",i,98562124-i);
    }
    return 0;
}