// list of num
// 1 4 9 6 1 3 2 4 6 5 1 3 9 8 4 1
#include <bits/stdc++.h>

using namespace std;
int main(){
	int ar[] = {1, 4, 9, 6, 1, 3, 2, 4, 6, 5, 1, 3, 9, 8, 4, 1};

	unordered_map<int,int> mapp;
	int l = sizeof(ar) / sizeof(ar[0]);
	

	for(int i=0;i < l;i++){
		if( mapp.find(ar[i]) == mapp.end()){
			cout << ar[i] <<" ";
			mapp[ ar[i] ] =1;
		}
	}
	// 1 4 9 6 3 2 
	return 0;
}