
#include <iostream>
using namespace std;

bool isPrime(int n){
	
	if(n <= 1) return false;
	if(n <= 3) return true;
	if(n%2 ==0 || n%3 == 0) return false;

	for(int i=5;i*i <= n;i++){
		if( n%i == 0) return false;
	}
	return true;
}

int main(){
	/*int n; cin>>n;
	if( isPrime(n) ) cout<<"Prime\n";
	else cout<<"Not Prime\n";
*/

	int arr[] = {-1,2,3,4,5,6,12,11,16,13,14,20};


	int a=-10,b=-10;
	for(int i=0;i<12;i++) cout<<arr[i]<<" "; cout<<"\n";
	for(int i=0;i<12;i++){
		
		if(arr[i] >= a){
			b = a;
			a = arr[i];
		}
		else if(arr[i] > b ){
			b = arr[i];
		}
	}
	cout<< b<<"\n";
}