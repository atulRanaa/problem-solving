#include  <bits/stdc++.h>
using namespace std;
int main()
{
	int n=10, count=1;
	for(int i=1;i<=n;i++)
	{
		int j=i;
		if(i&1){ 
			while(j-- > 1){cout<<count++<<"*";} cout<<count++<<"\n";
		}
		else{
			count += i; j=0;
			while(j++ < i-1){ cout<<count-j<<"*";} cout<<count-(j)<<"\n";
		}
	}
}
