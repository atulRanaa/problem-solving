#include <bits/stdc++.h>
using namespace std;

bool isPrime(int n)
{
    // Corner cases
    if (n <= 1)  return false;
    if (n <= 3)  return true;
 
    // This is checked so that we can skip 
    // middle five numbers in below loop
    if (n%2 == 0 || n%3 == 0) return false;
 
    for (int i=5; i*i<=n; i=i+6)
        if (n%i == 0 || n%(i+2) == 0)
           return false;
 
    return true;
}
vector<int> primesum(int A) {
    vector<int> ans(2);
   
    for (int i=0; i<A; i++)
    {
        if (isPrime(i) && isPrime(A-i))
        {
            ans[0] = i;
            ans[1] = (A-i);
            return ans;
        }
    }
}
int main(){
	vector<int> it = primesum(16777214) ;
	cout<<it[0]<<" "<<it[1];
}